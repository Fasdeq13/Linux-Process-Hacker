#ifndef MEMINFOREADER_H
#define MEMINFOREADER_H

#include <QObject>

struct SystemMemorySnapshot
{
    qint64 totalMemoryKb = 0;
    qint64 freeMemoryKb = 0;
    qint64 availableMemoryKb = 0;
    qint64 buffersKb = 0;
    qint64 cachedKb = 0;
    qint64 swapTotalKb = 0;
    qint64 swapFreeKb = 0;

    qint64 usedMemoryKb() const;
    qint64 usedSwapKb() const;
    double usedMemoryPercent() const;
};

class MemInfoReader : public QObject
{
    Q_OBJECT

public:
    explicit MemInfoReader(QObject *parent = nullptr);

    SystemMemorySnapshot readMemorySnapshot();

private:
    qint64 extractValueKb(const QString &line);
};

#endif
