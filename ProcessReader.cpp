#include <unistd.h>
#include "ProcessReader.h"
#include "ProcFsUtils.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <pwd.h>

ProcessReader::ProcessReader(QObject *parent)
    : QObject(parent)
{
}

QVector<ProcessInfo> ProcessReader::readAllProcesses()
{
    QVector<ProcessInfo> processList;
    const QVector<int> processIds = ProcFsUtils::listProcessIds();

    if (!ioDeltaTimerStarted) {
        ioDeltaTimer.start();
        ioDeltaTimerStarted = true;
    }

    for (int pid : processIds) {
        std::optional<ProcessInfo> processInfo = readSingleProcess(pid);
        if (processInfo.has_value()) {
            processList.append(processInfo.value());
        }
    }

    ioDeltaTimer.restart();

    return processList;
}

std::optional<ProcessInfo> ProcessReader::readSingleProcess(int pid)
{
    ProcessInfo processInfo;
    processInfo.pid = pid;

    if (!parseStatFile(pid, processInfo)) {
        return std::nullopt;
    }

    parseStatusFile(pid, processInfo);
    parseCmdlineFile(pid, processInfo);
    parseExeLink(pid, processInfo);
    parseIoFile(pid, processInfo);
    computeIoDeltaPerSecond(processInfo);

    return processInfo;
}

bool ProcessReader::parseStatFile(int pid, ProcessInfo &processInfo)
{
    QString statPath = ProcFsUtils::procPath(pid, "stat");
    QString content = ProcFsUtils::readFileContent(statPath);
    if (content.isEmpty()) {
        return false;
    }

    int nameStart = content.indexOf('(');
    int nameEnd = content.lastIndexOf(')');
    if (nameStart < 0 || nameEnd < 0 || nameEnd < nameStart) {
        return false;
    }

    processInfo.name = content.mid(nameStart + 1, nameEnd - nameStart - 1);

    QString remainder = content.mid(nameEnd + 2);
    QStringList fields = ProcFsUtils::splitFields(remainder);

    if (fields.size() < 20) {
        return false;
    }

    processInfo.state = ProcessInfo::stateFromChar(fields.at(0).at(0));
    processInfo.parentPid = fields.at(1).toInt();
    processInfo.utimeTicks = fields.at(11).toLongLong();
    processInfo.stimeTicks = fields.at(12).toLongLong();
    processInfo.priority = fields.at(15).toInt();
    processInfo.niceValue = fields.at(16).toInt();
    processInfo.threadCount = fields.at(17).toInt();
    processInfo.startTimeTicks = fields.at(19).toLongLong();

    if (fields.size() > 21) {
        processInfo.virtualMemoryBytes = fields.at(20).toLongLong();
    }
    if (fields.size() > 22) {
        qint64 residentPages = fields.at(21).toLongLong();
        processInfo.residentMemoryBytes = residentPages * sysconf(_SC_PAGESIZE);
    }

    return true;
}

bool ProcessReader::parseStatusFile(int pid, ProcessInfo &processInfo)
{
    QString statusPath = ProcFsUtils::procPath(pid, "status");
    QStringList lines = ProcFsUtils::readFileLines(statusPath);
    if (lines.isEmpty()) {
        return false;
    }

    for (const QString &line : lines) {
        if (line.startsWith("Uid:")) {
            QStringList parts = ProcFsUtils::splitFields(line);
            if (parts.size() > 1) {
                processInfo.userId = parts.at(1).toInt();
                processInfo.userName = resolveUserName(processInfo.userId);
            }
        }
    }
    return true;
}

bool ProcessReader::parseCmdlineFile(int pid, ProcessInfo &processInfo)
{
    QString cmdlinePath = ProcFsUtils::procPath(pid, "cmdline");
    QFile file(cmdlinePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray rawData = file.readAll();
    rawData.replace('\0', ' ');
    processInfo.commandLine = QString::fromUtf8(rawData).trimmed();

    return true;
}

bool ProcessReader::parseExeLink(int pid, ProcessInfo &processInfo)
{
    QString exePath = ProcFsUtils::procPath(pid, "exe");
    QFileInfo linkInfo(exePath);
    processInfo.executablePath = linkInfo.symLinkTarget();
    return !processInfo.executablePath.isEmpty();
}

bool ProcessReader::parseIoFile(int pid, ProcessInfo &processInfo)
{
    QString ioPath = ProcFsUtils::procPath(pid, "io");
    QStringList lines = ProcFsUtils::readFileLines(ioPath);
    if (lines.isEmpty()) {
        return false;
    }

    for (const QString &line : lines) {
        if (line.startsWith("read_bytes:")) {
            QStringList parts = ProcFsUtils::splitFields(line);
            if (parts.size() > 1) {
                processInfo.readBytesTotal = parts.at(1).toLongLong();
            }
        } else if (line.startsWith("write_bytes:")) {
            QStringList parts = ProcFsUtils::splitFields(line);
            if (parts.size() > 1) {
                processInfo.writeBytesTotal = parts.at(1).toLongLong();
            }
        }
    }

    return true;
}

void ProcessReader::computeIoDeltaPerSecond(ProcessInfo &processInfo)
{
    qint64 elapsedMilliseconds = ioDeltaTimer.isValid() ? ioDeltaTimer.elapsed() : 0;

    if (elapsedMilliseconds <= 0) {
        previousIoSnapshots.insert(processInfo.pid, {processInfo.readBytesTotal, processInfo.writeBytesTotal});
        return;
    }

    double elapsedSeconds = static_cast<double>(elapsedMilliseconds) / 1000.0;

    if (previousIoSnapshots.contains(processInfo.pid)) {
        const ProcessIoSnapshot &previousSnapshot = previousIoSnapshots.value(processInfo.pid);

        qint64 readDelta = processInfo.readBytesTotal - previousSnapshot.readBytes;
        qint64 writeDelta = processInfo.writeBytesTotal - previousSnapshot.writeBytes;

        if (readDelta > 0) {
            processInfo.diskReadBytesPerSec = static_cast<double>(readDelta) / elapsedSeconds;
        }
        if (writeDelta > 0) {
            processInfo.diskWriteBytesPerSec = static_cast<double>(writeDelta) / elapsedSeconds;
        }
    }

    previousIoSnapshots.insert(processInfo.pid, {processInfo.readBytesTotal, processInfo.writeBytesTotal});
}

QString ProcessReader::resolveUserName(int userId)
{
    struct passwd *passwordEntry = getpwuid(static_cast<uid_t>(userId));
    if (passwordEntry != nullptr) {
        return QString::fromLocal8Bit(passwordEntry->pw_name);
    }
    return QString::number(userId);
}