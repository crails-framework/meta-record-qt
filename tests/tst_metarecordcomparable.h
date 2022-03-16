#ifndef  MYTEST_H
# define MYTEST_H

# include <QObject>
# include <QTest>
# include "metarecord/metarecordcomparable.h"

class MetaRecordComparableSample : public MetaRecordComparable
{
  Q_OBJECT

  Q_PROPERTY(int number MEMBER number CONSTANT)
  Q_PROPERTY(QString string MEMBER string CONSTANT)
  Q_PROPERTY(bool boolean MEMBER boolean CONSTANT)
public:
  MetaRecordComparableSample(QObject* parent = nullptr) : MetaRecordComparable(parent)
  {
    number = 0;
    string = "toto";
    boolean = true;
  }

  int number;
  QString string;
  bool boolean;
};

class MetaRecordComparableTest : public QObject
{
  Q_OBJECT
private slots:
  void compareShouldReturnTrueWithEmptyParam()
  {
    MetaRecordComparableSample model;
    QVERIFY(model.compare({}));
  }

  void compareShouldReturnTruwWithIdenticalParams()
  {
    MetaRecordComparableSample model;
    for (auto strategy : {MetaRecordComparable::Likeness, MetaRecordComparable::Identical})
    {
      QVERIFY(model.compare({
        {"number", 0},
        {"string", "toto"},
        {"boolean", true}
      }, strategy));
    }
  }

  void compareShouldReturnFalseWithDifferentParams()
  {
    MetaRecordComparableSample model;
    QVERIFY(!model.compare({
      {"boolean", false}
    }));
  }

  void likenessCompareShouldReturnTrueWithNumberStartingIdentically()
  {
    MetaRecordComparableSample model;
    model.number = 4271;
    QVERIFY(model.compare({
      {"number", 42}
    }));
  }

  void likenessCompareShouldReturnTrueWithSimilarStrings()
  {
    MetaRecordComparableSample model;
    model.string = "chapopointu";
    QVERIFY(model.compare({
      {"string", "popo"}
    }));
  }

  void identiclCompareShouldReturnFalseWithSimilarStrings()
  {
    MetaRecordComparableSample model;
    model.string = "chapopointu";
    QVERIFY(!model.compare({
      {"string", "popo"}
    }, MetaRecordComparable::Identical));
  }
};

#endif

