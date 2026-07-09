#include "MemoryMapsParser.h"
#include "ProcFsUtils.h"

MemoryMapsParser::MemoryMapsParser(QObject *parent)
    : QObject(parent)
{
}

bool MemoryMapsParser::parseMapLine(const QString &line, MemoryRegion &region)
{
    QStringList fields = ProcFsUtils::splitFields(line);
    if (fields.size() < 2) {
        return false;
    }

    QStringList addressParts = fields.at(0).split('-');
    if (addressParts.size() != 2) {
        return false;
    }

    bool startOk = false;
    bool endOk = false;
    region.startAddress = addressParts.at(0).toULongLong(&startOk, 16);
    region.endAddress = addressParts.at(1).toULongLong(&endOk, 16);

    if (!startOk || !endOk) {
        return false;
    }

    region.permissions = fields.at(1);

    if (fields.size() >= 6) {
        QStringList pathComponents = fields.mid(5);
        region.pathName = pathComponents.join(' ');
    }

    return true;
}

QVector<MemoryRegion> MemoryMapsParser::parseMapsForProcess(int pid)
{
    QVector<MemoryRegion> regions;
    QString mapsPath = ProcFsUtils::procPath(pid, "maps");
    QStringList lines = ProcFsUtils::readFileLines(mapsPath);

    for (const QString &line : lines) {
        MemoryRegion region;
        if (parseMapLine(line, region)) {
            regions.append(region);
        }
    }

    return regions;
}
