#ifndef METARECORDCOMPARABLE_H
#define METARECORDCOMPARABLE_H

#include "metarecordable.h"

class METARECORDQT_EXPORT MetaRecordComparable : public MetaRecordable
{
  Q_OBJECT
public:
  enum ComparaisonType
  {
    Likeness  = 0,
    Identical = 1
  };
  Q_ENUM(ComparaisonType)

  explicit MetaRecordComparable(QObject *parent = nullptr);

  Q_INVOKABLE virtual bool compare(const QVariantMap&, int comparaisonType = Likeness) const;
};

#endif // METARECORDCOMPARABLE_H
