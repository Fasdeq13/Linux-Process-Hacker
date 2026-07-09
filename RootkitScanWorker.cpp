#include "RootkitScanWorker.h"

RootkitScanWorker::RootkitScanWorker(QObject *parent)
    : QObject(parent)
    , rootkitDetector(new RootkitDetector(this))
{
}

void RootkitScanWorker::runScan(const QSet<int> &visibleProcessIds)
{
    QVector<int> hiddenProcessIds = rootkitDetector->findHiddenProcessIds(visibleProcessIds);
    emit scanFinished(hiddenProcessIds);
}
