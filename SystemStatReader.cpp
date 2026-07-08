#include "SystemStatReader.h"
#include "ProcFsUtils.h"

qint64 CpuTimeSnapshot::totalTicks() const
{
    return userTicks + niceTicks + systemTicks + idleTicks +
           ioWaitTicks + irqTicks + softIrqTicks + stealTicks;
}

qint64 CpuTimeSnapshot::activeTicks() const
{
    return totalTicks() - idleTicks - ioWaitTicks;
}

SystemStatReader::SystemStatReader(QObject *parent)
    : QObject(parent)
{
}

CpuTimeSnapshot SystemStatReader::parseCpuLine(const QString &line)
{
    CpuTimeSnapshot snapshot;
    QStringList fields = ProcFsUtils::splitFields(line);

    if (fields.size() < 8) {
        return snapshot;
    }

    snapshot.userTicks = fields.at(1).toLongLong();
    snapshot.niceTicks = fields.at(2).toLongLong();
    snapshot.systemTicks = fields.at(3).toLongLong();
    snapshot.idleTicks = fields.at(4).toLongLong();
    snapshot.ioWaitTicks = fields.at(5).toLongLong();
    snapshot.irqTicks = fields.at(6).toLongLong();
    snapshot.softIrqTicks = fields.at(7).toLongLong();

    if (fields.size() > 8) {
        snapshot.stealTicks = fields.at(8).toLongLong();
    }

    return snapshot;
}

CpuTimeSnapshot SystemStatReader::readOverallCpuSnapshot()
{
    QStringList lines = ProcFsUtils::readFileLines("/proc/stat");
    for (const QString &line : lines) {
        if (line.startsWith("cpu ")) {
            return parseCpuLine(line);
        }
    }
    return CpuTimeSnapshot();
}

QVector<CpuTimeSnapshot> SystemStatReader::readPerCoreCpuSnapshots()
{
    QVector<CpuTimeSnapshot> snapshots;
    QStringList lines = ProcFsUtils::readFileLines("/proc/stat");

    for (const QString &line : lines) {
        if (line.startsWith("cpu") && !line.startsWith("cpu ")) {
            snapshots.append(parseCpuLine(line));
        }
    }
    return snapshots;
}

double SystemStatReader::computeCpuUsagePercent(const CpuTimeSnapshot &previous, const CpuTimeSnapshot &current)
{
    qint64 totalDelta = current.totalTicks() - previous.totalTicks();
    qint64 activeDelta = current.activeTicks() - previous.activeTicks();

    if (totalDelta <= 0) {
        return 0.0;
    }

    return (static_cast<double>(activeDelta) / static_cast<double>(totalDelta)) * 100.0;
}
