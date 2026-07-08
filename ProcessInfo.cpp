#include "ProcessInfo.h"

ProcessInfo::ProcessInfo()
{
}

ProcessState ProcessInfo::stateFromChar(QChar stateChar)
{
    switch (stateChar.toLatin1()) {
        case 'R': return ProcessState::Running;
        case 'S': return ProcessState::Sleeping;
        case 'D': return ProcessState::DiskSleep;
        case 'Z': return ProcessState::Zombie;
        case 'T': return ProcessState::Stopped;
        case 't': return ProcessState::TracingStop;
        case 'X': return ProcessState::Dead;
        default: return ProcessState::Unknown;
    }
}

QString ProcessInfo::stateToString(ProcessState state)
{
    switch (state) {
        case ProcessState::Running: return QStringLiteral("Running");
        case ProcessState::Sleeping: return QStringLiteral("Sleeping");
        case ProcessState::DiskSleep: return QStringLiteral("Disk Sleep");
        case ProcessState::Zombie: return QStringLiteral("Zombie");
        case ProcessState::Stopped: return QStringLiteral("Stopped");
        case ProcessState::TracingStop: return QStringLiteral("Tracing Stop");
        case ProcessState::Dead: return QStringLiteral("Dead");
        default: return QStringLiteral("Unknown");
    }
}
