#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <QString>
#include <QMetaType>

enum class ProcessState
{
    Running,
    Sleeping,
    DiskSleep,
    Zombie,
    Stopped,
    TracingStop,
    Dead,
    Unknown
};

class ProcessInfo
{
public:
    ProcessInfo();

    int pid = 0;
    int parentPid = 0;
    int userId = 0;
    QString userName;
    QString name;
    QString commandLine;
    QString executablePath;
    ProcessState state = ProcessState::Unknown;

    qint64 virtualMemoryBytes = 0;
    qint64 residentMemoryBytes = 0;

    qint64 utimeTicks = 0;
    qint64 stimeTicks = 0;
    qint64 startTimeTicks = 0;

    double cpuUsagePercent = 0.0;
    double memoryUsagePercent = 0.0;

    int threadCount = 0;
    int niceValue = 0;
    int priority = 0;

    static ProcessState stateFromChar(QChar stateChar);
    static QString stateToString(ProcessState state);
};

Q_DECLARE_METATYPE(ProcessInfo)

#endif
