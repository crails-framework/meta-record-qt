#include "metarecordable.h"
#include <utility>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QDebug>

static void injectQProperties(const QObject& object, QVariantMap& map, const QStringList& blacklist = {})
{
  const QMetaObject& metaObject = *(object.metaObject());

  for (int i = 0 ; i < metaObject.propertyCount() ; ++i)
  {
    const QMetaProperty property = metaObject.property(i);
    const QString propertyName(property.name());
    QVariant value;

    if (!blacklist.contains(propertyName))
      value = object.property(property.name());
    if (!value.isNull())
      map.insert(propertyName, value);
  }
}

static void importQProperties(QObject& object, const QVariantMap& map, const QStringList& blacklist = {})
{
  const QMetaObject& metaObject = *(object.metaObject());

  for (int i = 0 ; i < metaObject.propertyCount() ; ++i)
  {
    const QMetaProperty property = metaObject.property(i);
    const QString       propertyName(property.name());
    QVariant            value    = map[propertyName];

    if (!value.isNull() && !blacklist.contains(propertyName))
      object.setProperty(property.name(), value);
  }
}

MetaRecordable::MetaRecordable(QObject* parent) : QObject(parent)
{
}

const QStringList& MetaRecordable::virtualPropertyList() const
{
  static const QStringList properties{"type"};

  return properties;
}

void MetaRecordable::fromJson(const QByteArray& text)
{
  fromJson(QJsonDocument::fromJson(text).object());
}

void MetaRecordable::fromVariantMap(const QVariantMap& map)
{
  importQProperties(*this, map, virtualPropertyList());
  for (const auto& relationship : std::as_const(relationships))
    relationship->fromVariant(map[relationship->name].toList());
}

QVariantMap MetaRecordable::toVariantMap(int) const
{
  QVariantMap map;

  injectQProperties(*this, map, virtualPropertyList());
  if (usePolymorphism())
    map.insert("_type", getType());
  for (const auto& relationship : std::as_const(relationships))
    map.insert(relationship->name, relationship->toVariant());
  return map;
}

bool MetaRecordable::copy(const MetaRecordable* source, int)
{
  if (source && metaObject()->className() == source->metaObject()->className())
  {
    fromVariantMap(source->toVariantMap());
    return true;
  }
  else
    qDebug() << "MetaRecordable: cannot copy" << source << "into" << this;
  return false;
}

QString MetaRecordable::inspect() const
{
  return QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
}
