#ifndef PROCESSREADER_H
#define PROCESSREADER_H

#include <QObject>
#include <QVector>
#include "ProcessInfo.h"

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
    QString resolveUserName(int userId);
};

#endif
