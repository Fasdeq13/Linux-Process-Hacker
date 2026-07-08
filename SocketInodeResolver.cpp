#include "SocketInodeResolver.h"
#include "ProcFsUtils.h"

#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

SocketInodeResolver::SocketInodeResolver(QObject *parent)
    : QObject(parent)
{
}

void SocketInodeResolver::rebuildInodeToPidMap()
{
    inodeToPidMap.clear();

    const QVector<int> processIds = ProcFsUtils::listProcessIds();
    for (int pid : processIds) {
        scanProcessFileDescriptors(pid);
    }
}

void SocketInodeResolver::scanProcessFileDescriptors(int pid)
{
    QString fdDirectoryPath = ProcFsUtils::procPath(pid, "fd");
    QDir fdDirectory(fdDirectoryPath);

    if (!fdDirectory.exists()) {
        return;
    }

    QString processNamePath = ProcFsUtils::procPath(pid, "comm");
    QString processName = ProcFsUtils::readFileContent(processNamePath).trimmed();

    const QStringList fdEntries = fdDirectory.entryList(QDir::Files | QDir::System | QDir::NoDotAndDotDot);

    for (const QString &fdEntry : fdEntries) {
        QString fullFdPath = fdDirectoryPath + "/" + fdEntry;
        QFileInfo fdFileInfo(fullFdPath);
        QString linkTarget = fdFileInfo.symLinkTarget();

        quint64 inodeNumber = extractInodeFromSocketLink(linkTarget);
        if (inodeNumber != 0) {
            inodeToPidMap.insert(inodeNumber, qMakePair(pid, processName));
        }
    }
}

quint64 SocketInodeResolver::extractInodeFromSocketLink(const QString &linkTarget) const
{
    QRegularExpression socketPattern("socket:\\[(\\d+)\\]");
    QRegularExpressionMatch match = socketPattern.match(linkTarget);

    if (!match.hasMatch()) {
        return 0;
    }

    return match.captured(1).toULongLong();
}

QPair<int, QString> SocketInodeResolver::resolvePidForInode(quint64 inodeNumber) const
{
    return inodeToPidMap.value(inodeNumber, qMakePair(0, QString()));
}
