#ifndef PROCESSSIGNALSENDER_H
#define PROCESSSIGNALSENDER_H

#include <QObject>
#include <QString>

enum class ProcessSignalType
{
    Terminate,
    Kill,
    Suspend,
    Resume
};

class ProcessSignalSender : public QObject
{
    Q_OBJECT

public:
    explicit ProcessSignalSender(QObject *parent = nullptr);

    bool sendSignal(int pid, ProcessSignalType signalType);

    static int signalTypeToPosixValue(ProcessSignalType signalType);
    static QString signalTypeToLabel(ProcessSignalType signalType);
};

#endif
