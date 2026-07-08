#ifndef NETWORKCONNECTIONREADER_H
#define NETWORKCONNECTIONREADER_H

#include <QObject>
#include <QVector>
#include "NetworkConnectionInfo.h"
#include "SocketInodeResolver.h"

class NetworkConnectionReader : public QObject
{
    Q_OBJECT

public:
    explicit NetworkConnectionReader(QObject *parent = nullptr);

    QVector<NetworkConnectionInfo> readAllConnections();

private:
    QVector<NetworkConnectionInfo> parseProtocolFile(const QString &filePath, NetworkProtocol protocol);
    void applyOwnerInfo(NetworkConnectionInfo &connectionInfo);

    SocketInodeResolver *socketInodeResolver;
};

#endif
