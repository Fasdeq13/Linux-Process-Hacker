#include "ProcessMemoryDumper.h"
#include "ProcFsUtils.h"

#include <QFile>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

ProcessMemoryDumper::ProcessMemoryDumper(QObject *parent)
    : QObject(parent)
{
}

QByteArray ProcessMemoryDumper::readRawMemoryRange(int pid, quint64 startAddress, quint64 length, bool &success)
{
    success = false;
    QByteArray resultBuffer;

    QString memPath = ProcFsUtils::procPath(pid, "mem");
    int fileDescriptor = ::open(memPath.toLocal8Bit().constData(), O_RDONLY);

    if (fileDescriptor < 0) {
        return resultBuffer;
    }

    resultBuffer.resize(static_cast<int>(length));

    ssize_t bytesRead = ::pread(fileDescriptor,
                                 resultBuffer.data(),
                                 static_cast<size_t>(length),
                                 static_cast<off_t>(startAddress));

    ::close(fileDescriptor);

    if (bytesRead < 0) {
        resultBuffer.clear();
        return resultBuffer;
    }

    resultBuffer.resize(static_cast<int>(bytesRead));
    success = true;

    return resultBuffer;
}

bool ProcessMemoryDumper::dumpRegionToFile(int pid, const MemoryRegion &region, const QString &outputFilePath, QString &errorMessage)
{
    if (!region.isReadable()) {
        errorMessage = QStringLiteral("Memory region is not readable");
        return false;
    }

    bool readSuccess = false;
    QByteArray rawData = readRawMemoryRange(pid, region.startAddress, region.sizeInBytes(), readSuccess);

    if (!readSuccess) {
        errorMessage = QStringLiteral("Failed to read process memory, root privileges may be required");
        return false;
    }

    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        errorMessage = QStringLiteral("Failed to open output file for writing");
        return false;
    }

    outputFile.write(rawData);
    outputFile.close();

    return true;
}

bool ProcessMemoryDumper::dumpFullProcessToFile(int pid, const QVector<MemoryRegion> &regions, const QString &outputFilePath, QString &errorMessage)
{
    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        errorMessage = QStringLiteral("Failed to open output file for writing");
        return false;
    }

    bool anyRegionWritten = false;

    for (const MemoryRegion &region : regions) {
        if (!region.isReadable()) {
            continue;
        }

        bool readSuccess = false;
        QByteArray rawData = readRawMemoryRange(pid, region.startAddress, region.sizeInBytes(), readSuccess);

        if (readSuccess && !rawData.isEmpty()) {
            outputFile.write(rawData);
            anyRegionWritten = true;
        }
    }

    outputFile.close();

    if (!anyRegionWritten) {
        errorMessage = QStringLiteral("No readable memory regions could be dumped, root privileges may be required");
        return false;
    }

    return true;
}

QVector<ExtractedStringEntry> ProcessMemoryDumper::extractStringsFromRegion(int pid, const MemoryRegion &region, int minimumStringLength)
{
    QVector<ExtractedStringEntry> extractedStrings;

    if (!region.isReadable()) {
        return extractedStrings;
    }

    bool readSuccess = false;
    QByteArray rawData = readRawMemoryRange(pid, region.startAddress, region.sizeInBytes(), readSuccess);

    if (!readSuccess || rawData.isEmpty()) {
        return extractedStrings;
    }

    QByteArray currentRun;
    quint64 currentRunStartAddress = region.startAddress;

    for (int byteIndex = 0; byteIndex < rawData.size(); byteIndex++) {
        unsigned char currentByte = static_cast<unsigned char>(rawData.at(byteIndex));
        bool isPrintableCharacter = (currentByte >= 32 && currentByte <= 126);

        if (isPrintableCharacter) {
            if (currentRun.isEmpty()) {
                currentRunStartAddress = region.startAddress + static_cast<quint64>(byteIndex);
            }
            currentRun.append(static_cast<char>(currentByte));
        } else {
            if (currentRun.size() >= minimumStringLength) {
                ExtractedStringEntry entry;
                entry.address = currentRunStartAddress;
                entry.text = QString::fromLatin1(currentRun);
                extractedStrings.append(entry);
            }
            currentRun.clear();
        }
    }

    if (currentRun.size() >= minimumStringLength) {
        ExtractedStringEntry entry;
        entry.address = currentRunStartAddress;
        entry.text = QString::fromLatin1(currentRun);
        extractedStrings.append(entry);
    }

    return extractedStrings;
}
