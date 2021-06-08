#include "WordsCounter.h"

#include <QFile>
#include <QDateTime>

// can be used to update chart while loading file
void WordsCounter::TopWordsHolder::recalculate(const Dictionary& dictionary)
{
    for (auto dictIter = dictionary.begin(); dictIter != dictionary.end(); dictIter++)
    {
        // if not full and no such key then add
        if (wordsFrequence.size() < maxCount && !wordsFrequence.contains(dictIter.key()))
        {
            wordsFrequence.insert(dictIter.key(), dictIter.value());
            continue;
        }

        // update value of existing key
        if (wordsFrequence.contains(dictIter.key()))
        {
            wordsFrequence[dictIter.key()] = dictIter.value();
            continue;
        }

        // find min element
        auto minWordsIter =  wordsFrequence.begin();
        for (auto wordsIter = wordsFrequence.begin(); wordsIter != wordsFrequence.end(); wordsIter++)
        {
            if (wordsIter.value() < minWordsIter.value())
                minWordsIter = wordsIter;
        }

        // replace min element if new is higher
        if (dictIter.value() > minWordsIter.value())
        {
            wordsFrequence.remove(minWordsIter.key());
            wordsFrequence.insert(dictIter.key(), dictIter.value());
        }
    }
}

WordsCounter::WordsCounter(const QString& filePath, int topWordsCount, QObject* parent)
    : QObject(parent)
    , _filePath(filePath)
    , _dictionary(Dictionary())
    , _topWordsHolder(TopWordsHolder{topWordsCount, {}})
{
}

void WordsCounter::process()
{
    QFile file(_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit canNotOpenFile();
        emit finished();
        return;
    }

    const auto rows = totalRows(file);
    quint64 lineCounter = 0;
    auto lastUpdate = QDateTime::currentDateTime();
    while (!file.atEnd())
    {
        auto line = QString::fromUtf8(file.readLine());
        lineCounter++;
        for (auto word : line.split(' '))
            addToDictionary(normalizeWord(word));

        // emit progress signal every 100+ms
        if (lastUpdate.msecsTo(QDateTime::currentDateTime()) > 100)
        {
            lastUpdate = QDateTime::currentDateTime();
            emit progressed(lineCounter / static_cast<double>(rows));
        }
    }

    _topWordsHolder.recalculate(_dictionary);

    emit progressed(1.);
    emit finished();
}

QStringList WordsCounter::getWords() const
{
    return _topWordsHolder.wordsFrequence.keys();
}

QVariantList WordsCounter::getCounts() const
{
    auto values = _topWordsHolder.wordsFrequence.values();
    QVector<QVariant> vector(values.size());
    std::copy(values.begin(), values.end(), vector.begin());

    return vector.toList();
}

quint64 WordsCounter::getMaxFrequence() const
{
    quint64 max = 0;
    for (auto iter = _topWordsHolder.wordsFrequence.begin(); iter != _topWordsHolder.wordsFrequence.end(); iter++)
        max = std::max(max, iter.value());

    return max;
}

QString WordsCounter::normalizeWord(const QString& word) const
{
    enum State {
        Begin,
        Letter
    };

    QString nWord;
    State state = Begin;
    for (const auto ch : word)
    {
        switch (state)
        {
        case Begin :
            if (ch.isLetter())
            {
                nWord.append(ch.toLower());
                state = Letter;
            }
            break;

        case Letter :
            if (ch.isLetter() || ch == '-')
                nWord.append(ch.toLower());
            else
                return nWord;

            break;
        }
    }

    return nWord;
}

void WordsCounter::addToDictionary(const QString& word)
{
    if (word.isEmpty())
        return;

    _dictionary[word]++;
}

quint64 WordsCounter::totalRows(QFile& file) const
{
    quint64 total = 0;
    while (!file.atEnd())
    {
        file.readLine();
        total++;
    }

    file.seek(0);
    return total;
}
