#pragma once

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QMap>

class QFile;
using Dictionary = QHash<QString, quint64>;

class WordsCounter : public QObject
{
    Q_OBJECT

public:
    explicit WordsCounter(const QString& filePath, int topWordsCount, QObject* parent = nullptr);

    void process();

    QStringList getWords() const;
    QVariantList getCounts() const;
    quint64 getMaxFrequence() const;

signals:
    void progressed(double);
    void finished();
    void canNotOpenFile();

private:
    struct TopWordsHolder
    {
        int maxCount;
        QMap<QString, quint64> wordsFrequence;

        void recalculate(const Dictionary& dictionary);
    };

private:
    QString normalizeWord(const QString& word) const;
    void addToDictionary(const QString& word);
    quint64 totalRows(QFile& file) const;

private:
    const QString _filePath;
    Dictionary _dictionary;
    TopWordsHolder _topWordsHolder;
};

