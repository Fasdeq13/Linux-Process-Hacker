#ifndef SOCKETINODERESOLVER_H
#define SOCKETINODERESOLVER_H

#include <QObject>
#include <QHash>
#include <QPair>

class SocketInodeResolver : public QObject
{
    Q_OBJECT

public:
    explicit SocketInodeResolver(QObject *parent = nullptr);

    void rebuildInodeToPidMap();
    QPair<int, QString> resolvePidForInode(quint64 inodeNumber) const;

private:
    void scanProcessFileDescriptors(int pid);
    quint64 extractInodeFromSocketLink(const QString &linkTarget) const;

    QHash<quint64, QPair<int, QString>> inodeToPidMap;
};

#endif
