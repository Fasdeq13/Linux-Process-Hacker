#ifndef ROOTKITDETECTOR_H
#define ROOTKITDETECTOR_H

#include <QObject>
#include <QSet>
#include <QVector>

class RootkitDetector : public QObject
{
    Q_OBJECT

public:
    explicit RootkitDetector(QObject *parent = nullptr);

    QVector<int> findHiddenProcessIds(const QSet<int> &visibleProcessIds);

private:
    int readMaxPidValue();
    bool doesProcessExistViaSignal(int pid);
};

#endif
