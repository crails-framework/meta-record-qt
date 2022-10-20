#ifndef  METARECORDTABLEMODEL_H
# define METARECORDTABLEMODEL_H

# include <QAbstractTableModel>
# include <QQmlListProperty>
# include <QJSValue>
# include "metarecordable.h"

class QJSEngine;

class MetaRecordTableModel : public QAbstractTableModel
{
  Q_OBJECT

  Q_PROPERTY(QByteArray rowHeaderColumn MEMBER rowHeaderColumn NOTIFY rowHeaderColumnChanged)
  Q_PROPERTY(QQmlListProperty<MetaRecordable> models READ modelList NOTIFY listChanged)
  Q_PROPERTY(int columnCount READ columnCount NOTIFY columnsChanged)
  Q_PROPERTY(int currentIndex MEMBER m_currentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

  enum TableRoles
  {
    HeaderRole = Qt::UserRole + 1,
  };

public:
  enum State
  {
    ReadState, CreateState, UpdateState
  };
  Q_ENUM(State)
protected:

  struct Column
  {
    QByteArray   property;
    QString      label;
    QJSValue     handler;
    unsigned int enforcedWidth = 0;
    unsigned int decimals = 2;

    explicit Column(const QByteArray& property);
    explicit Column(QJSValue);
    Column() {}
    Column(const Column& col) : property(col.property), label(col.label), handler(col.handler) {}

    bool operator==(const QByteArray& value) const { return property == value; }
  };

  QVector<MetaRecordable*> list;
  QVector<Column>          columns;
  QByteArray               rowHeaderColumn;
  int                      m_currentIndex = 0;
  State                    m_state = ReadState;
  QJSEngine*               javascriptEngine = nullptr;
public:
  explicit MetaRecordTableModel(QObject* parent = nullptr);

  void initializeRowHeaderColumn(const MetaRecordable& ref);
  void initializeColumns(const MetaRecordable& ref, QStringList blacklist = QStringList() << "uuid");
  void appendRows(const QVector<MetaRecordable*>&, int index = -1);
  void replaceRows(const QVector<MetaRecordable*>&);
  void removeRows(const QStringList& uids);
  void removeRow(unsigned int index);
  void removeRow(const MetaRecordable*);
  void clear();

  Q_INVOKABLE void setColumns(QJSValue);

  int indexOf(QByteArray uid);
  inline int count() const { return rowCount(); }
  int rowCount(const QModelIndex& = QModelIndex()) const override;
  int columnCount(const QModelIndex& = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant tableData(const QModelIndex& index) const;
  Q_INVOKABLE QVariant tableData(unsigned int row, unsigned int column) const { return tableData(createIndex(row, column)); }
  QVariant columnData(int column) const;
  QHash<int, QByteArray> roleNames() const override;
  QQmlListProperty<MetaRecordable> modelList();
  const QVector<MetaRecordable*>& models() const { return list; }
  int currentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int value) { m_currentIndex = value; emit currentIndexChanged(); }
  State state() const { return m_state; }
  void setState(State value) { m_state = value; emit stateChanged(); }

  Q_INVOKABLE QByteArray        propertyAt(unsigned int column) const;
  Q_INVOKABLE MetaRecordable* modelAt(unsigned int row) const;
  Q_INVOKABLE MetaRecordable* currentModel() const { return modelAt(m_currentIndex); }
  Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  Q_INVOKABLE bool isEditableRow(int index) const { return index == m_currentIndex && m_state != ReadState; }

signals:
  void rowHeaderColumnChanged();
  void listChanged();
  void columnsChanged();
  void currentIndexChanged();
  void stateChanged();

protected:
  int  getColumnIndex(const QByteArray&) const;
  void appendColumn(QJSValue);
};

#endif
