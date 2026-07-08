#include "MemInfoReader.h"
#include "ProcFsUtils.h"

qint64 SystemMemorySnapshot::usedMemoryKb() const
{
    return totalMemoryKb - availableMemoryKb;
}

qint64 SystemMemorySnapshot::usedSwapKb() const
{
    return swapTotalKb - swapFreeKb;
}

double SystemMemorySnapshot::usedMemoryPercent() const
{
    if (totalMemoryKb <= 0) {
        return 0.0;
    }
    return (static_cast<double>(usedMemoryKb()) / static_cast<double>(totalMemoryKb)) * 100.0;
}

MemInfoReader::MemInfoReader(QObject *parent)
    : QObject(parent)
{
}

qint64 MemInfoReader::extractValueKb(const QString &line)
{
    QStringList fields = ProcFsUtils::splitFields(line);
    if (fields.size() < 2) {
        return 0;
    }
    return fields.at(1).toLongLong();
}

SystemMemorySnapshot MemInfoReader::readMemorySnapshot()
{
    SystemMemorySnapshot snapshot;
    QStringList lines = ProcFsUtils::readFileLines("/proc/meminfo");

    for (const QString &line : lines) {
        if (line.startsWith("MemTotal:")) {
            snapshot.totalMemoryKb = extractValueKb(line);
        } else if (line.startsWith("MemFree:")) {
            snapshot.freeMemoryKb = extractValueKb(line);
        } else if (line.startsWith("MemAvailable:")) {
            snapshot.availableMemoryKb = extractValueKb(line);
        } else if (line.startsWith("Buffers:")) {
            snapshot.buffersKb = extractValueKb(line);
        } else if (line.startsWith("Cached:") && !line.startsWith("SwapCached:")) {
            snapshot.cachedKb = extractValueKb(line);
        } else if (line.startsWith("SwapTotal:")) {
            snapshot.swapTotalKb = extractValueKb(line);
        } else if (line.startsWith("SwapFree:")) {
            snapshot.swapFreeKb = extractValueKb(line);
        }
    }

    return snapshot;
}
