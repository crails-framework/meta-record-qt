#ifndef  METARECORDABLE_H
# define METARECORDABLE_H

# include <QObject>
# include <QByteArray>
# include <QVariantMap>
# include <QJsonObject>
# include "MetaRecordQt_global.h"
# include "relationship.h"

class METARECORDQT_EXPORT MetaRecordable : public QObject
{
  Q_OBJECT
  typedef QVector<std::shared_ptr<IMetaRecordRelationship>> Relationships;

  Q_PROPERTY(QByteArray type READ getType CONSTANT)
public:
  explicit MetaRecordable(QObject *parent = nullptr);

  const QByteArray&  getUid() const { return uid; };
  void               setUid(const QByteArray& val) { uid = val; emit uidChanged(); }
  virtual QByteArray getType() const { return metaObject()->className(); }
  bool               isPersistent() const { return !uid.isEmpty() && !uid.startsWith("NULL"); }
  bool               isSame(const MetaRecordable& other) const { return !uid.isEmpty() && uid == other.getUid(); }
  bool               isSame(const MetaRecordable* other) const { return other && isSame(*other); }
  virtual void       fromVariantMap(const QVariantMap&);
  void               fromJson(const QByteArray&);
  void               fromJson(const QJsonObject& value) { fromVariantMap(value.toVariantMap()); }
  void               fromValue(const QJsonValue& value) { fromVariantMap(value.toObject().toVariantMap()); }
  Q_INVOKABLE void   fromValue(const QVariant& value)   { fromVariantMap(value.toMap()); }

  Q_INVOKABLE virtual bool copy(const MetaRecordable*, int options = 0);

  virtual QVariantMap  toVariantMap(int flag = 0) const;
  Q_INVOKABLE QVariant toVariant(int flag = 0) const { return toVariantMap(flag); }
  QJsonObject          toJson(int flag = 0) const { return QJsonObject::fromVariantMap(toVariantMap(flag)); }
  Q_INVOKABLE QString  inspect() const;

  static bool areSame(const MetaRecordable* a, const MetaRecordable* b)
  {
    return a && b && a->isSame(b);
  }

  template<typename FINAL_TYPE>
  static const char* collectionName()
  {
    const QMetaType metaType = QMetaType::fromType<FINAL_TYPE>();
    const auto* metaObject = metaType.metaObject() ? metaType.metaObject() : FINAL_TYPE().metaObject();
    return metaObject ? metaObject->className() : "";
  }

  template<typename FINAL_TYPE>
  static FINAL_TYPE* factory(const QJsonObject& json, QObject* parent = nullptr) { return factory<FINAL_TYPE>(json.toVariantMap(), parent); }

  template<typename FINAL_TYPE>
  static FINAL_TYPE* factory(const QVariantMap& map, QObject* parent = nullptr)
  {
    auto* model = new FINAL_TYPE(parent);

    model->fromVariantMap(map);
    return model;
  }

signals:
  void uidChanged();

protected:
  virtual bool usePolymorphism() const { return false; }
  virtual const QStringList& virtualPropertyList() const;

  template<typename MODEL>
  void registerRelationship(const QString& name, QList<MODEL*>& list)
  {
    relationships << std::make_shared<MetaRecordRelationship<MODEL>>(this, name, list);
  }

  QByteArray uid;
  Relationships relationships;
};

#define METARECORD_VIRTUAL_PROPERTIES(parent, list) \
  virtual const QStringList& virtualPropertyList() const override \
  { \
    static const QStringList value = parent::virtualPropertyList() + QStringList(list); \
    return value; \
  }

#endif // METARECORDABLE_H
