#include "metarecordcomparable.h"
#include <QMetaProperty>
#include <QRegularExpression>
#include <QDate>
#include <QDebug>

typedef bool (CompareFunction)(const QVariant&, const QVariant&);
typedef bool (CompareStrategy)(const QVariant&, const QVariant&, const QMetaType::Type);

MetaRecordComparable::MetaRecordComparable(QObject *parent) : MetaRecordable(parent)
{

}

static bool numericalCompare(const QVariant& a, const QVariant& b)
{
  QString bString = b.toString();

  if (!bString.isEmpty() && b != "NaN" && b != 0)
  {
    QRegularExpression regex("^" + bString);

    return a.toString().indexOf(regex) >= 0;
  }
  return true;
}

static bool flagCompare(const QVariant& a, const QVariant& b)
{
  QString bString = b.toString();

  if (!bString.isEmpty() && b != "NaN")
    return a.toUInt() == b.toUInt();
  return true;
}

static bool stringCompare(const QVariant& a, const QVariant& b)
{
  //QRegularExpression regex(b.toString(), QRegularExpression::CaseInsensitiveOption);
  //return a.toString().indexOf(regex) >= 0;
  const auto strA = a.toString().toUpper().trimmed();
  const auto strB = b.toString().toUpper().trimmed();

  return strA.length() > strB.length() ? strA.indexOf(strB) >= 0 : strB.indexOf(strA) >= 0;
}

static bool dateCompare(const QVariant& a, const QVariant& b)
{
  QDate comparedTo = b.toDate();

  return !comparedTo.isValid() || a.toDate() == comparedTo;
}

static bool boolCompare(const QVariant& a, const QVariant& b)
{
  return a.toBool() == b.toBool();
}

const QMap<QMetaType::Type, CompareFunction*> compareFunctions = {
  {QMetaType::Int,        &numericalCompare},
  {QMetaType::UInt,       &numericalCompare},
  {QMetaType::Double,     &numericalCompare},
  {QMetaType::LongLong,   &numericalCompare},
  {QMetaType::ULongLong,  &numericalCompare},
  {QMetaType::QByteArray, &stringCompare},
  {QMetaType::QString,    &stringCompare},
  {QMetaType::QDate,      &dateCompare},
  {QMetaType::Bool,       &boolCompare},
  {QMetaType::UShort,     &flagCompare}
};

static bool compareLikeness(const QVariant& a, const QVariant& b, const QMetaType::Type type)
{
  if (compareFunctions.contains(type))
    return compareFunctions[type](a, b);
  else
    qDebug() << "MetaRecordComparable: comparing property of unknown type" << type;
  return true;
}

static bool compareIdentical(const QVariant& a, const QVariant& b, const QMetaType::Type)
{
  return a == b;
}

static CompareStrategy* getCompareStrategy(MetaRecordComparable::ComparaisonType type)
{
  switch (type)
  {
    case MetaRecordComparable::Likeness:
      break ;
    case MetaRecordComparable::Identical:
      return &compareIdentical;
  }
  return &compareLikeness;
}

bool MetaRecordComparable::compare(const QVariantMap& vars, int compareType) const
{
  const QMetaObject& metaObject = *this->metaObject();
  CompareStrategy* strategy = getCompareStrategy(static_cast<ComparaisonType>(compareType));

  for (int i = 0 ; i < metaObject.propertyCount() ; ++i)
  {
    const QMetaProperty property = metaObject.property(i);
    const QString propertyName(property.name());

    if (vars.contains(propertyName))
    {
      QVariant value = this->property(property.name());
      QVariant comparedTo = vars[propertyName];
      QMetaType::Type propertyType;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      propertyType = static_cast<QMetaType::Type>(property.type());
#else
      propertyType = static_cast<QMetaType::Type>(property.typeId());
#endif
      if (!strategy(value, comparedTo, propertyType))
        return false;
    }
  }
  return true;
}
