#ifndef SYSTEMSTATREADER_H
#define SYSTEMSTATREADER_H

#include <QObject>
#include <QVector>

struct CpuTimeSnapshot
{
    qint64 userTicks = 0;
    qint64 niceTicks = 0;
    qint64 systemTicks = 0;
    qint64 idleTicks = 0;
    qint64 ioWaitTicks = 0;
    qint64 irqTicks = 0;
    qint64 softIrqTicks = 0;
    qint64 stealTicks = 0;

    qint64 totalTicks() const;
    qint64 activeTicks() const;
};

class SystemStatReader : public QObject
{
    Q_OBJECT

public:
    explicit SystemStatReader(QObject *parent = nullptr);

    CpuTimeSnapshot readOverallCpuSnapshot();
    QVector<CpuTimeSnapshot> readPerCoreCpuSnapshots();
    double computeCpuUsagePercent(const CpuTimeSnapshot &previous, const CpuTimeSnapshot &current);

private:
    CpuTimeSnapshot parseCpuLine(const QString &line);
};

#endif
