#ifndef  METARECORD_HASONE_H
# define METARECORD_HASONE_H

# include <QObject>
# include <QByteArray>
# include "metarecordable.h"

template<typename TYPE>
void setHasOneRelationship(QObject* self, QByteArray& uid, TYPE*& value, const TYPE* newValue)
{
  if (value)
    delete value;
  if (newValue)
  {
    value = MetaRecordable::factory<TYPE>(newValue->toVariantMap(), self);
    uid   = value->getUuid();
  }
  else
  {
    value = nullptr;
    uid   = QByteArray();
  }
}

# define METARECORD_MODEL_SETTER_BY_COPY_WITH_SIGNAL(TYPE, VARNAME, METHODNAME, SIGNALNAME) \
  void METHODNAME(const TYPE* ptr) \
  { \
    if (VARNAME != ptr) \
    { \
      setHasOneRelationship(this, VARNAME##Id, VARNAME, ptr); \
      emit SIGNALNAME(); \
    } \
  }

# define METARECORD_MODEL_SETTER_BY_COPY(TYPE, VARNAME, METHODNAME) \
  METARECORD_MODEL_SETTER_BY_COPY_WITH_SIGNAL(TYPE, VARNAME, METHODNAME, attributeChanged)

#endif
