#include "NetworkConnectionReader.h"
#include "NetworkAddressParser.h"
#include "ProcFsUtils.h"

NetworkConnectionReader::NetworkConnectionReader(QObject *parent)
    : QObject(parent)
    , socketInodeResolver(new SocketInodeResolver(this))
{
}

QVector<NetworkConnectionInfo> NetworkConnectionReader::readAllConnections()
{
    socketInodeResolver->rebuildInodeToPidMap();

    QVector<NetworkConnectionInfo> allConnections;

    allConnections += parseProtocolFile("/proc/net/tcp", NetworkProtocol::Tcp);
    allConnections += parseProtocolFile("/proc/net/tcp6", NetworkProtocol::Tcp6);
    allConnections += parseProtocolFile("/proc/net/udp", NetworkProtocol::Udp);
    allConnections += parseProtocolFile("/proc/net/udp6", NetworkProtocol::Udp6);

    return allConnections;
}

QVector<NetworkConnectionInfo> NetworkConnectionReader::parseProtocolFile(const QString &filePath, NetworkProtocol protocol)
{
    QVector<NetworkConnectionInfo> connections;
    QStringList lines = ProcFsUtils::readFileLines(filePath);

    if (lines.isEmpty()) {
        return connections;
    }

    bool isIpv6 = (protocol == NetworkProtocol::Tcp6 || protocol == NetworkProtocol::Udp6);

    for (int lineIndex = 1; lineIndex < lines.size(); lineIndex++) {
        QStringList fields = ProcFsUtils::splitFields(lines.at(lineIndex));

        if (fields.size() < 10) {
            continue;
        }

        NetworkConnectionInfo connectionInfo;
        connectionInfo.protocol = protocol;

        QPair<QString, int> localEndpoint = isIpv6
            ? NetworkAddressParser::parseIPv6AddressPort(fields.at(1))
            : NetworkAddressParser::parseIPv4AddressPort(fields.at(1));

        QPair<QString, int> remoteEndpoint = isIpv6
            ? NetworkAddressParser::parseIPv6AddressPort(fields.at(2))
            : NetworkAddressParser::parseIPv4AddressPort(fields.at(2));

        connectionInfo.localAddress = localEndpoint.first;
        connectionInfo.localPort = localEndpoint.second;
        connectionInfo.remoteAddress = remoteEndpoint.first;
        connectionInfo.remotePort = remoteEndpoint.second;

        connectionInfo.state = NetworkConnectionInfo::stateFromHex(fields.at(3));

        QStringList queueParts = fields.at(4).split(':');
        if (queueParts.size() == 2) {
            connectionInfo.transmitQueueBytes = queueParts.at(0).toLongLong(nullptr, 16);
            connectionInfo.receiveQueueBytes = queueParts.at(1).toLongLong(nullptr, 16);
        }

        connectionInfo.inodeNumber = fields.at(9).toULongLong();

        applyOwnerInfo(connectionInfo);

        connections.append(connectionInfo);
    }

    return connections;
}

void NetworkConnectionReader::applyOwnerInfo(NetworkConnectionInfo &connectionInfo)
{
    if (connectionInfo.inodeNumber == 0) {
        return;
    }

    QPair<int, QString> ownerInfo = socketInodeResolver->resolvePidForInode(connectionInfo.inodeNumber);
    connectionInfo.ownerPid = ownerInfo.first;
    connectionInfo.ownerProcessName = ownerInfo.second;
}
