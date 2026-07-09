#ifndef MEMORYREGION_H
#define MEMORYREGION_H

#include <QString>
#include <QVector>
#include <cstdint>

struct MemoryRegion
{
    quint64 startAddress = 0;
    quint64 endAddress = 0;
    QString permissions;
    QString pathName;

    quint64 sizeInBytes() const;
    bool isReadable() const;
};

#endif
