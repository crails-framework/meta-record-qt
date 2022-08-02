#ifndef  METARECORDABLE_H
# define METARECORDABLE_H

# include <QObject>
# include <QByteArray>
# include <QVariantMap>
# include <QJsonObject>
# include "MetaRecordQt_global.h"

class METARECORDQT_EXPORT MetaRecordable : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QByteArray type READ getType CONSTANT)
public:
  explicit MetaRecordable(QObject *parent = nullptr);

  virtual const QByteArray& getUuid() const = 0;
  virtual QByteArray getType() const { return metaObject()->className(); }
  virtual void       fromVariantMap(const QVariantMap&);
  void               fromJson(const QByteArray&);
  void               fromJson(const QJsonObject& value) { fromVariantMap(value.toVariantMap()); }
  void               fromValue(const QJsonValue& value) { fromVariantMap(value.toObject().toVariantMap()); }
  Q_INVOKABLE void   fromValue(const QVariant& value)   { fromVariantMap(value.toMap()); }

  virtual QVariantMap  toVariantMap(int flag = 0) const;
  Q_INVOKABLE QVariant toVariant(int flag = 0) const { return toVariantMap(flag); }
  QJsonObject          toJson(int flag = 0) const { return QJsonObject::fromVariantMap(toVariantMap(flag)); }
  Q_INVOKABLE QString  inspect() const;

  template<typename FINAL_TYPE>
  static const char* collectionName() { return FINAL_TYPE().metaObject()->className(); }

  template<typename FINAL_TYPE>
  static FINAL_TYPE* factory(const QJsonObject& json, QObject* parent = nullptr) { return factory<FINAL_TYPE>(json.toVariantMap(), parent); }

  template<typename FINAL_TYPE>
  static FINAL_TYPE* factory(const QVariantMap& map, QObject* parent = nullptr)
  {
    auto* model = new FINAL_TYPE(parent);

    model->fromVariantMap(map);
    return model;
  }

protected:
  virtual bool usePolymorphism() const { return false; }
  virtual const QStringList& virtualPropertyList() const;
};

#define METARECORD_VIRTUAL_PROPERTIES(parent, list) \
  virtual const QStringList& virtualPropertyList() const override \
  { \
    static const QStringList value = parent::virtualPropertyList() + QStringList(list); \
    return value; \
  }

#endif // METARECORDABLE_H
