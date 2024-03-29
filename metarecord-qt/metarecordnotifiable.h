#ifndef  METARECORDNOTIFIABLE_H
# define METARECORDNOTIFIABLE_H

#include "metarecordvalidatable.h"

class METARECORDQT_EXPORT MetaRecordNotifiable : public MetaRecordValidatable
{
  Q_OBJECT
public:
  explicit MetaRecordNotifiable(QObject* parent = nullptr);

  virtual void fromNotification(const QJsonObject& payload);

signals:
  void attributeChanged();
  void updatedRemotely();
};

#endif
