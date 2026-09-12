#include "tablemodel.h"
#include <QMetaProperty>
#include <QJSEngine>
#include <qi18n.h>
#include <cmath>

static void disconnectWatcher(QMap<MetaRecordable*, QMetaObject::Connection>& watchers, MetaRecordable* model)
{
  auto it = watchers.find(model);
  if (it != watchers.end())
  {
    QObject::disconnect(it.value());
    watchers.erase(it);
  }
}

static void disconnectAllWatchers(const QVector<MetaRecordable*>& list, QMap<MetaRecordable*, QMetaObject::Connection>& watchers)
{
  for (auto it = watchers.begin() ; it != watchers.end() ; ++it)
  {
    if (list.indexOf(it.key()) >= 0)
      QObject::disconnect(it.value());
  }
  watchers.clear();
}

MetaRecordTableModel::MetaRecordTableModel(QObject* parent) : QAbstractTableModel(parent)
{
  connect(this, &MetaRecordTableModel::listChanged, this, &MetaRecordTableModel::updateDestroyWatchers);
}

void MetaRecordTableModel::updateDestroyWatchers()
{
  disconnectAllWatchers(list, destroyWatchers);
  for (MetaRecordable* model : list)
    watchRow(model);
}

void MetaRecordTableModel::watchRow(MetaRecordable* model)
{
  if (!model || destroyWatchers.contains(model))
    return;
  destroyWatchers.insert(
    model,
    QObject::connect(model, &QObject::destroyed, this, [this, model]() { removeRow(model); })
  );
}

void MetaRecordTableModel::initializeRowHeaderColumn(const MetaRecordable& sample)
{
  QVector<QByteArray> rowHeaderCandidates({"code", "name", "uid"});

  for (const auto& candidate : rowHeaderCandidates)
  {
    if (sample.property(candidate.constData()).isValid())
    {
      rowHeaderColumn = candidate;
      break ;
    }
  }
  emit rowHeaderColumnChanged();
}

MetaRecordTableModel::Column::Column(const QByteArray& property) : property(property)
{
  label = QI18n::get()->t("properties." + property);
}

MetaRecordTableModel::Column::Column(QJSValue object)
{
  QJSValue callback = object.property("display");

  property = object.property("property").toString().toUtf8();
  if (object.hasProperty("label"))
    label  = object.property("label").toString();
  else
    label  = QI18n::get()->t("properties." + property);
  if (callback.isCallable())
    handler = callback;
  if (object.hasProperty("width"))
    enforcedWidth = object.property("width").toUInt();
  if (object.hasProperty("decimals"))
    decimals = object.property("decimals").toUInt();
}

void MetaRecordTableModel::initializeColumns(const MetaRecordable& sample, QStringList blacklist)
{
  const auto* meta = sample.metaObject();
  QVector<Column> newColumns;

  blacklist << rowHeaderColumn << "objectName";
  for (int i = 0 ; i < meta->propertyCount() ; ++i)
  {
    QMetaProperty property = meta->property(i);
    QByteArray    propertyName(property.name());

    if (blacklist.indexOf(propertyName) < 0)
      newColumns << Column(QByteArray(propertyName));
  }
  replaceColumns(newColumns);
}

void MetaRecordTableModel::appendColumn(QJSValue object, QVector<Column>& target) const
{
  if (object.isString())
    target << Column(object.toString().toUtf8());
  else if (object.isObject())
    target << Column(object);
}

void MetaRecordTableModel::setColumns(QJSValue object)
{
  unsigned int length = object.property("length").toUInt();
  QVector<Column> newColumns;

  for (unsigned int i = 0 ; i < length ; ++i)
    appendColumn(object.property(i), newColumns);
  replaceColumns(newColumns);
}

void MetaRecordTableModel::replaceColumns(const QVector<Column>& newColumns)
{
  if (columns.count() > 0)
  {
    beginRemoveColumns(QModelIndex(), 0, columns.count() - 1);
    columns.clear();
    endRemoveColumns();
  }
  if (newColumns.count() > 0)
  {
    beginInsertColumns(QModelIndex(), 0, newColumns.count() - 1);
    columns = newColumns;
    endInsertColumns();
  }
  emit columnsChanged();
}

void MetaRecordTableModel::appendRows(const QVector<MetaRecordable*>& entries, int index)
{
  if (entries.isEmpty())
    return;
  if (index < 0)
    index = rowCount();
  beginInsertRows(QModelIndex(), index, index + entries.length() - 1);
  for (auto it = entries.rbegin() ; it != entries.rend() ; ++it)
    list.insert(index, *it);
  for (auto* model : entries)
    watchRow(model);
  endInsertRows();
}

void MetaRecordTableModel::replaceRows(const QVector<MetaRecordable*>& entries)
{
  beginResetModel();
  list = entries;
  endResetModel();
  emit listChanged();
}

void MetaRecordTableModel::removeRows(const QStringList& uids)
{
  for (const QString& uid : uids)
  {
    int index = indexOf(uid.toUtf8());

    if (index >= 0)
      removeRow(static_cast<unsigned int>(index));
  }
}

void MetaRecordTableModel::removeRow(unsigned int uindex)
{
  int index = static_cast<int>(uindex);
  if (index < list.size())
  {
    beginRemoveRows(QModelIndex(), index, index);
    disconnectWatcher(destroyWatchers, list.at(index));
    list.remove(index);
    endRemoveRows();
  }
}

void MetaRecordTableModel::removeRow(const MetaRecordable* model)
{
  int index = list.indexOf(const_cast<MetaRecordable*>(model));

  if (index >= 0)
    removeRow(static_cast<unsigned int>(index));
}

int MetaRecordTableModel::indexOf(QByteArray uid)
{
  for (int i = 0 ; i < list.size() ; ++i)
  {
    if (list[i]->getUid() == uid)
      return i;
  }
  return -1;
}

void MetaRecordTableModel::clear()
{
  beginResetModel();
  disconnectAllWatchers(list, destroyWatchers);
  list.clear();
  endResetModel();
}

int MetaRecordTableModel::rowCount(const QModelIndex&) const
{
  return list.length();
}

int MetaRecordTableModel::columnCount(const QModelIndex&) const
{
  return columns.count();
}

QVariant MetaRecordTableModel::data(const QModelIndex& index, int role) const
{
  switch (role)
  {
  case Qt::DisplayRole:
    return tableData(index);
  case HeaderRole:
    return columnData(index.column());
  case PropertyRole:
    return columnProperty(index.column());
  case ModelRole:
    return QVariant::fromValue(list.size() > index.row() ? static_cast<QObject*>(list.at(index.row())) : nullptr);
  }
  return QVariant();
}

QVariant MetaRecordTableModel::tableData(const QModelIndex& index) const
{
  MetaRecordable* model        = list.size() > index.row() ? list.at(index.row()) : nullptr;
  const Column&   column       = columns[index.column()];
  QVariant        value;

  if (model)
  {
    if (javascriptEngine && column.handler.isCallable())
    {
      QJSValueList args;

      args << javascriptEngine->newQObject(model);
      args << QString(column.property);
      value = QJSValue(column.handler).call(args).toVariant();
    }
    else
    {
      value = model->property(column.property.constData());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      if (value.typeId() == QMetaType::Double)
#else
      if (static_cast<QMetaType::Type>(value.type()) == QMetaType::Double)
#endif
      {
        unsigned int op = std::pow(10, column.decimals);
        value = std::round(value.toDouble() * op) / op;
      }
    }
  }
  return value;
}

QVariant MetaRecordTableModel::columnData(int column) const
{
  if (columns.count() > column)
    return columns[column].label;
  return QVariant(column);
}

QByteArray MetaRecordTableModel::columnProperty(int column) const
{
  if (columns.count() > column)
    return columns[column].property;
  return QByteArray();
}

QHash<int, QByteArray> MetaRecordTableModel::roleNames() const
{
  return {
    {Qt::DisplayRole, "displayValue"},
    {HeaderRole,      "header"},
    {ModelRole,       "model"},
    {PropertyRole,    "propertyName"}
  };
}

QVariant MetaRecordTableModel::headerData(int section, Qt::Orientation orientation, int) const
{
  switch (orientation)
  {
    case Qt::Horizontal:
      return columnData(section);
    case Qt::Vertical:
      if (list.size() > section && rowHeaderColumn.length() > 0)
        return list.at(section)->property(rowHeaderColumn.constData());
  }
  return QVariant(section);
}

QByteArray MetaRecordTableModel::propertyAt(unsigned int column) const
{
  if (columns.count() > column)
    return columns[column].property;
  return "";
}

MetaRecordable* MetaRecordTableModel::modelAt(unsigned int row) const
{
  if (list.size() > row)
    return list.at(row);
  return nullptr;
}

int MetaRecordTableModel::getColumnIndex(const QByteArray& propertyName) const
{
  for (int i = 0 ; i < columns.size() ; ++i)
  {
    if (columns[i] == propertyName)
      return i;
  }
  return -1;
}

void MetaRecordTableModel::refreshModel(const MetaRecordable* model)
{
  auto row = list.indexOf(const_cast<MetaRecordable*>(model));

  if (row >= 0)
  {
    QModelIndex topLeft = createIndex(row, 0, const_cast<MetaRecordable*>(model));
    QModelIndex bottomRight = createIndex(row, columnCount(), const_cast<MetaRecordable*>(model));

    emit dataChanged(topLeft, bottomRight);
  }
}

void MetaRecordTableModel::refreshModelCell(const MetaRecordable* model, const char* propertyName)
{
  auto row = list.indexOf(const_cast<MetaRecordable*>(model));
  auto column = getColumnIndex(propertyName);

  if (row >= 0 && column >= 0)
  {
    QModelIndex index = createIndex(row, column, const_cast<MetaRecordable*>(model));

    emit dataChanged(index, index);
  }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
typedef int RowSizeType;
#else
typedef long long int RowSizeType;
#endif

static RowSizeType qmlRowCount(QQmlListProperty<MetaRecordable>* p)
{
  return reinterpret_cast<MetaRecordTableModel*>(p->object)->rowCount();
}

static MetaRecordable* qmlRowGet(QQmlListProperty<MetaRecordable>* p, RowSizeType row)
{
  return reinterpret_cast<MetaRecordTableModel*>(p->object)->modelAt(row);
}

QQmlListProperty<MetaRecordable> MetaRecordTableModel::modelList()
{
  return QQmlListProperty<MetaRecordable>(this, this, &qmlRowCount, &qmlRowGet);
}
