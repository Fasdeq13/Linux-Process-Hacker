#ifndef PROCESSMEMORYDUMPER_H
#define PROCESSMEMORYDUMPER_H

#include <QObject>
#include <QString>
#include <QVector>
#include "MemoryRegion.h"

struct ExtractedStringEntry
{
    quint64 address = 0;
    QString text;
};

class ProcessMemoryDumper : public QObject
{
    Q_OBJECT

public:
    explicit ProcessMemoryDumper(QObject *parent = nullptr);

    bool dumpRegionToFile(int pid, const MemoryRegion &region, const QString &outputFilePath, QString &errorMessage);
    bool dumpFullProcessToFile(int pid, const QVector<MemoryRegion> &regions, const QString &outputFilePath, QString &errorMessage);
    QVector<ExtractedStringEntry> extractStringsFromRegion(int pid, const MemoryRegion &region, int minimumStringLength = 4);

private:
    QByteArray readRawMemoryRange(int pid, quint64 startAddress, quint64 length, bool &success);
};

#endif
