#ifndef ROOTKITSCANWORKER_H
#define ROOTKITSCANWORKER_H

#include <QObject>
#include <QSet>
#include <QVector>
#include "RootkitDetector.h"

class RootkitScanWorker : public QObject
{
    Q_OBJECT

public:
    explicit RootkitScanWorker(QObject *parent = nullptr);

public slots:
    void runScan(const QSet<int> &visibleProcessIds);

signals:
    void scanFinished(const QVector<int> &hiddenProcessIds);

private:
    RootkitDetector *rootkitDetector;
};

#endif
