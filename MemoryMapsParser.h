#ifndef MEMORYMAPSPARSER_H
#define MEMORYMAPSPARSER_H

#include <QObject>
#include <QVector>
#include "MemoryRegion.h"

class MemoryMapsParser : public QObject
{
    Q_OBJECT

public:
    explicit MemoryMapsParser(QObject *parent = nullptr);

    QVector<MemoryRegion> parseMapsForProcess(int pid);

private:
    bool parseMapLine(const QString &line, MemoryRegion &region);
};

#endif
