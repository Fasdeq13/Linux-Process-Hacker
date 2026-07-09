#include <unistd.h>

#ifndef PROCESSREADER_H
#define PROCESSREADER_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QElapsedTimer>
#include "ProcessInfo.h"

struct ProcessIoSnapshot
{
    qint64 readBytes = 0;
    qint64 writeBytes = 0;
};

class ProcessReader : public QObject
{
    Q_OBJECT

public:
    explicit ProcessReader(QObject *parent = nullptr);

    QVector<ProcessInfo> readAllProcesses();
    std::optional<ProcessInfo> readSingleProcess(int pid);

private:
    bool parseStatFile(int pid, ProcessInfo &processInfo);
    bool parseStatusFile(int pid, ProcessInfo &processInfo);
    bool parseCmdlineFile(int pid, ProcessInfo &processInfo);
    bool parseExeLink(int pid, ProcessInfo &processInfo);
    bool parseIoFile(int pid, ProcessInfo &processInfo);
    void computeIoDeltaPerSecond(ProcessInfo &processInfo);
    QString resolveUserName(int userId);

    QHash<int, ProcessIoSnapshot> previousIoSnapshots;
    QElapsedTimer ioDeltaTimer;
    bool ioDeltaTimerStarted = false;
};

#endif