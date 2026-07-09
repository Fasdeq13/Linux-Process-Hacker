#include "MemoryRegion.h"

quint64 MemoryRegion::sizeInBytes() const
{
    if (endAddress <= startAddress) {
        return 0;
    }
    return endAddress - startAddress;
}

bool MemoryRegion::isReadable() const
{
    return permissions.startsWith('r');
}
