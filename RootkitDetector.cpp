#include "RootkitDetector.h"
#include "ProcFsUtils.h"

#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>

RootkitDetector::RootkitDetector(QObject *parent)
    : QObject(parent)
{
}

int RootkitDetector::readMaxPidValue()
{
    std::optional<qint64> maxPidValue = ProcFsUtils::readLongLongValue("/proc/sys/kernel/pid_max");
    if (maxPidValue.has_value() && maxPidValue.value() > 0) {
        return static_cast<int>(maxPidValue.value());
    }
    return 32768;
}

bool RootkitDetector::doesProcessExistViaSignal(int pid)
{
    int resultCode = ::kill(static_cast<pid_t>(pid), 0);

    if (resultCode == 0) {
        return true;
    }

    if (errno == EPERM) {
        return true;
    }

    return false;
}

QVector<int> RootkitDetector::findHiddenProcessIds(const QSet<int> &visibleProcessIds)
{
    QVector<int> hiddenProcessIds;
    int maxPidValue = readMaxPidValue();

    for (int candidatePid = 1; candidatePid <= maxPidValue; candidatePid++) {
        if (visibleProcessIds.contains(candidatePid)) {
            continue;
        }

        if (doesProcessExistViaSignal(candidatePid)) {
            hiddenProcessIds.append(candidatePid);
        }
    }

    return hiddenProcessIds;
}
