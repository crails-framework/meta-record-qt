#ifndef  METARECORDVALIDATABLE_H
# define METARECORDVALIDATABLE_H

# include "metarecordcomparable.h"

class METARECORDQT_EXPORT MetaRecordValidatable : public MetaRecordComparable
{
  Q_OBJECT
public:
  MetaRecordValidatable(QObject* parent = nullptr) : MetaRecordComparable(parent) {}

  Q_INVOKABLE virtual bool        isValid() const { return validate().size() == 0; }
  Q_INVOKABLE virtual QVariantMap validate() const { return QVariantMap(); }
};

#endif
