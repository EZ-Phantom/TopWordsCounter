#include "Controller.h"
#include "WordsCounter.h"

#include <QThread>
#include <QUrl>

Controller::Controller(QObject* parent)
    : QObject(parent)
    , _topWordsCount(15)
{
}

void Controller::calculateFile(const QUrl& filePath)
{
    auto thread = new QThread;
    auto worker = new WordsCounter(filePath.toLocalFile(), _topWordsCount);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &WordsCounter::process);

    connect(worker, &WordsCounter::progressed, this, &Controller::progress);
    connect(worker, &WordsCounter::canNotOpenFile, this, &Controller::canNotOpenFile);

    connect(worker, &WordsCounter::finished, [this, worker]() {
        _words = worker->getWords();
        _counts = worker->getCounts();
        _maxCount = worker->getMaxFrequence();

        emit readyToFetch();
    });
    connect(worker, &WordsCounter::finished, thread, &QThread::quit);
    connect(worker, &WordsCounter::finished, worker, &WordsCounter::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

QStringList Controller::getWords() const
{
    return _words;
}

QVariantList Controller::getCounts() const
{
    return _counts;
}

quint64 Controller::getMaxFrequence() const
{
    return _maxCount;
}

int Controller::getTopWordsCount() const
{
    return _topWordsCount;
}
