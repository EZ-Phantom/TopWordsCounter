#pragma once

#include <QObject>
#include <QVariant>

class QUrl;

class Controller : public QObject
{
    Q_OBJECT

public:

    explicit Controller(QObject* parent = nullptr);

    Q_INVOKABLE void calculateFile(const QUrl& filePath);

    Q_INVOKABLE QStringList getWords() const;
    Q_INVOKABLE QVariantList getCounts() const;
    Q_INVOKABLE quint64 getMaxFrequence() const;

    Q_INVOKABLE int getTopWordsCount() const;

signals:
    void progress(double value);
    void readyToFetch();
    void canNotOpenFile();

private:
    const int _topWordsCount;
    QStringList _words;
    QVariantList _counts;
    quint64 _maxCount;
};

