#ifndef METARECORD_RELATIONSHIP_H
#define METARECORD_RELATIONSHIP_H

# include <QObject>
# include <QString>
# include <QVariantList>
# include <QVariantMap>

class IMetaRecordRelationship
{
public:
  IMetaRecordRelationship(QObject* parent, const QString& name) : parent(parent), name(name)
  {
  }

  virtual ~IMetaRecordRelationship() {}

  virtual void fromVariant(const QVariantList& array) = 0;
  virtual QVariantList toVariant() const = 0;

  const QString name;
protected:
  QObject* parent;
};

template<class MODEL>
class MetaRecordRelationship : public IMetaRecordRelationship
{
public:
  MetaRecordRelationship(QObject* parent, const QString& name, QList<MODEL*>& list) : IMetaRecordRelationship(parent, name), list(list)
  {
  }

  inline void fromVariant(const QVariantList& array) { fromList(array); }

  inline QVariantList toVariant() const { return convertTo<QVariantList, QVariant>(&MODEL::toVariant); }

private:
  template<typename LIST_TYPE>
  void fromList(const LIST_TYPE& array)
  {
    list.clear();
    for (auto it = array.begin() ; it != array.end() ; ++it)
    {
      MODEL* newItem = new MODEL(parent);

      newItem->fromValue(*it);
      if (!newItem->getUid().isEmpty())
        remove(newItem->getUid());
      list << newItem;
    }
  }

  void remove(const QByteArray& uid)
  {
    for (auto it = list.begin() ; it != list.end() ; ++it)
    {
      if ((*it)->getUid() == uid)
      {
        MODEL* model = *it;

        list.erase(it);
        delete model;
        return ;
      }
    }
  }

  template<typename LIST_TYPE, typename ITEM_TYPE>
  LIST_TYPE convertTo(ITEM_TYPE (MODEL::*modelConverter)(int) const) const
  {
    LIST_TYPE array;

    for (auto it = list.begin() ; it != list.end() ; ++it)
    {
      MODEL* item = *it;

      array << (item->*modelConverter)(0);
    }
    return array;
  }

  QList<MODEL*>& list;
};

#endif // METARECORD_RELATIONSHIP_H
