#include "ProcessSignalSender.h"

#include <signal.h>
#include <sys/types.h>
#include <QDebug>

ProcessSignalSender::ProcessSignalSender(QObject *parent)
    : QObject(parent)
{
}

int ProcessSignalSender::signalTypeToPosixValue(ProcessSignalType signalType)
{
    switch (signalType) {
        case ProcessSignalType::Terminate: return SIGTERM;
        case ProcessSignalType::Kill: return SIGKILL;
        case ProcessSignalType::Suspend: return SIGSTOP;
        case ProcessSignalType::Resume: return SIGCONT;
        default: return SIGTERM;
    }
}

QString ProcessSignalSender::signalTypeToLabel(ProcessSignalType signalType)
{
    switch (signalType) {
        case ProcessSignalType::Terminate: return QStringLiteral("Terminate (SIGTERM)");
        case ProcessSignalType::Kill: return QStringLiteral("Kill (SIGKILL)");
        case ProcessSignalType::Suspend: return QStringLiteral("Suspend (SIGSTOP)");
        case ProcessSignalType::Resume: return QStringLiteral("Resume (SIGCONT)");
        default: return QString();
    }
}

bool ProcessSignalSender::sendSignal(int pid, ProcessSignalType signalType)
{
    int posixSignalValue = signalTypeToPosixValue(signalType);
    int resultCode = ::kill(static_cast<pid_t>(pid), posixSignalValue);

    if (resultCode != 0) {
        qWarning() << "Failed to send signal" << posixSignalValue << "to PID" << pid;
        return false;
    }

    return true;
}
