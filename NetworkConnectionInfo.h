#ifndef NETWORKCONNECTIONINFO_H
#define NETWORKCONNECTIONINFO_H

#include <QString>
#include <QMetaType>

enum class NetworkProtocol
{
    Tcp,
    Udp,
    Tcp6,
    Udp6,
    Unknown
};

enum class TcpConnectionState
{
    Established,
    SynSent,
    SynRecv,
    FinWait1,
    FinWait2,
    TimeWait,
    Close,
    CloseWait,
    LastAck,
    Listen,
    Closing,
    Unknown
};

class NetworkConnectionInfo
{
public:
    NetworkConnectionInfo();

    NetworkProtocol protocol = NetworkProtocol::Unknown;
    TcpConnectionState state = TcpConnectionState::Unknown;

    QString localAddress;
    int localPort = 0;

    QString remoteAddress;
    int remotePort = 0;

    quint64 inodeNumber = 0;
    int ownerPid = 0;
    QString ownerProcessName;

    qint64 receiveQueueBytes = 0;
    qint64 transmitQueueBytes = 0;

    static TcpConnectionState stateFromHex(const QString &hexValue);
    static QString stateToString(TcpConnectionState state);
    static QString protocolToString(NetworkProtocol protocol);
};

Q_DECLARE_METATYPE(NetworkConnectionInfo)

#endif
