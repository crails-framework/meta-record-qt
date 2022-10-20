#include "metarecordnotifiable.h"

MetaRecordNotifiable::MetaRecordNotifiable(QObject* parent) : MetaRecordValidatable(parent)
{
}

void MetaRecordNotifiable::fromNotification(const QJsonObject& payload)
{
  fromJson(payload);
  emit attributeChanged();
  emit updatedRemotely();
}
