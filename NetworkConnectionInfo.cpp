#include "NetworkConnectionInfo.h"

NetworkConnectionInfo::NetworkConnectionInfo()
{
}

TcpConnectionState NetworkConnectionInfo::stateFromHex(const QString &hexValue)
{
    bool conversionOk = false;
    int stateCode = hexValue.toInt(&conversionOk, 16);
    if (!conversionOk) {
        return TcpConnectionState::Unknown;
    }

    switch (stateCode) {
        case 0x01: return TcpConnectionState::Established;
        case 0x02: return TcpConnectionState::SynSent;
        case 0x03: return TcpConnectionState::SynRecv;
        case 0x04: return TcpConnectionState::FinWait1;
        case 0x05: return TcpConnectionState::FinWait2;
        case 0x06: return TcpConnectionState::TimeWait;
        case 0x07: return TcpConnectionState::Close;
        case 0x08: return TcpConnectionState::CloseWait;
        case 0x09: return TcpConnectionState::LastAck;
        case 0x0A: return TcpConnectionState::Listen;
        case 0x0B: return TcpConnectionState::Closing;
        default: return TcpConnectionState::Unknown;
    }
}

QString NetworkConnectionInfo::stateToString(TcpConnectionState state)
{
    switch (state) {
        case TcpConnectionState::Established: return QStringLiteral("ESTABLISHED");
        case TcpConnectionState::SynSent: return QStringLiteral("SYN_SENT");
        case TcpConnectionState::SynRecv: return QStringLiteral("SYN_RECV");
        case TcpConnectionState::FinWait1: return QStringLiteral("FIN_WAIT1");
        case TcpConnectionState::FinWait2: return QStringLiteral("FIN_WAIT2");
        case TcpConnectionState::TimeWait: return QStringLiteral("TIME_WAIT");
        case TcpConnectionState::Close: return QStringLiteral("CLOSE");
        case TcpConnectionState::CloseWait: return QStringLiteral("CLOSE_WAIT");
        case TcpConnectionState::LastAck: return QStringLiteral("LAST_ACK");
        case TcpConnectionState::Listen: return QStringLiteral("LISTEN");
        case TcpConnectionState::Closing: return QStringLiteral("CLOSING");
        default: return QStringLiteral("UNKNOWN");
    }
}

QString NetworkConnectionInfo::protocolToString(NetworkProtocol protocol)
{
    switch (protocol) {
        case NetworkProtocol::Tcp: return QStringLiteral("TCP");
        case NetworkProtocol::Udp: return QStringLiteral("UDP");
        case NetworkProtocol::Tcp6: return QStringLiteral("TCP6");
        case NetworkProtocol::Udp6: return QStringLiteral("UDP6");
        default: return QStringLiteral("UNKNOWN");
    }
}
