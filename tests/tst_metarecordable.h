#ifndef  MYTEST_H
# define MYTEST_H

# include <QObject>
# include <QTest>
# include "metarecord/metarecordable.h"

class MetaRecordableSample : public MetaRecordable
{
  Q_OBJECT

  Q_PROPERTY(int number MEMBER number NOTIFY numberChanged)
  Q_PROPERTY(QString string MEMBER string NOTIFY stringChanged)
  Q_PROPERTY(bool boolean MEMBER boolean NOTIFY booleanChanged)
public:
  MetaRecordableSample(QObject* parent = nullptr) : MetaRecordable(parent)
  {
    number = 0;
    string = "toto";
    boolean = true;
  }

  const QByteArray& getUuid() const override { return uuid; }

  int number;
  QString string;
  bool boolean;
  QByteArray uuid;

signals:
  void numberChanged();
  void stringChanged();
  void booleanChanged();
};

class MetaRecordableSampleWithBlacklist : public MetaRecordableSample
{
  Q_OBJECT
public:
  MetaRecordableSampleWithBlacklist(QObject* parent = nullptr) : MetaRecordableSample(parent)
  {}

private:
  const QStringList& virtualPropertyList() const override { static const QStringList properties{"boolean","number"}; return properties; }
};

class MetaRecordableTest : public QObject
{
  Q_OBJECT
private slots:
  void initTestCase()
  {
  }

  void fromVariantMapUpdatesRecordable()
  {
    MetaRecordableSample model;
    model.fromVariantMap({
      {"number", 42},
      {"string", "coucou"},
      {"boolean", false}
    });
    QCOMPARE(model.number, 42);
    QCOMPARE(model.string, QString("coucou"));
    QCOMPARE(model.boolean, false);
  }

  void fromVariantMapDoesNotUpdateUndefinedValues()
  {
    MetaRecordableSample model;
    model.fromVariantMap({
      {"number", 51}
    });
    QCOMPARE(model.string, QString("toto"));
    QCOMPARE(model.boolean, true);
  }

  void fromJsonString()
  {
    MetaRecordableSample model;
    model.fromJson("{\"number\": 121}");
    QCOMPARE(model.number, 121);
  }

  void toVariantMap()
  {
    MetaRecordableSample model;
    auto map = model.toVariantMap();
    QCOMPARE(model.number, map["number"].toInt());
    QCOMPARE(model.string, map["string"].toString());
    QCOMPARE(model.boolean, map["boolean"].toBool());
  }

  void shouldNotLoadVirtualPropertiesTest()
  {
    MetaRecordableSampleWithBlacklist model;
    model.fromVariantMap({
      {"number", 42},
      {"string", "coucou"},
      {"boolean", false}
    });
    QCOMPARE(model.number, 0);
    QCOMPARE(model.boolean, true);
  }

  void shouldLoadNonVirtualPropertiesTest()
  {
    MetaRecordableSampleWithBlacklist model;
    model.fromVariantMap({
      {"number", 42},
      {"string", "coucou"},
      {"boolean", false}
    });
    QCOMPARE(model.string, QString("coucou"));
  }

  void shouldNotExportVirtualPropertiesTest()
  {
    MetaRecordableSampleWithBlacklist model;
    auto map = model.toVariantMap();
    QVERIFY(map["number"].isNull());
    QVERIFY(map["boolean"].isNull());
  }

  void shouldExportNonVirtualPropertiesTest()
  {
    MetaRecordableSampleWithBlacklist model;
    auto map = model.toVariantMap();
    QCOMPARE(map["string"].toString(), QString("toto"));
  }

  void cleanupTestCase()
  {
  }
};

#endif
