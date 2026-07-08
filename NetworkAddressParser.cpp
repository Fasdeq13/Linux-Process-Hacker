#include "NetworkAddressParser.h"

#include <QStringList>
#include <arpa/inet.h>
#include <cstdint>

QString NetworkAddressParser::formatIPv4FromHex(const QString &hexAddress)
{
    bool conversionOk = false;
    quint32 rawValue = hexAddress.toUInt(&conversionOk, 16);
    if (!conversionOk) {
        return QStringLiteral("0.0.0.0");
    }

    quint8 firstOctet = static_cast<quint8>(rawValue & 0xFF);
    quint8 secondOctet = static_cast<quint8>((rawValue >> 8) & 0xFF);
    quint8 thirdOctet = static_cast<quint8>((rawValue >> 16) & 0xFF);
    quint8 fourthOctet = static_cast<quint8>((rawValue >> 24) & 0xFF);

    return QString("%1.%2.%3.%4")
        .arg(firstOctet)
        .arg(secondOctet)
        .arg(thirdOctet)
        .arg(fourthOctet);
}

QString NetworkAddressParser::formatIPv6FromHex(const QString &hexAddress)
{
    if (hexAddress.length() != 32) {
        return QStringLiteral("::");
    }

    uint32_t addressWords[4];
    for (int wordIndex = 0; wordIndex < 4; wordIndex++) {
        QString wordHex = hexAddress.mid(wordIndex * 8, 8);
        addressWords[wordIndex] = wordHex.toUInt(nullptr, 16);
    }

    struct in6_addr address6;
    memcpy(&address6, addressWords, sizeof(address6));

    char resultBuffer[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, &address6, resultBuffer, sizeof(resultBuffer)) == nullptr) {
        return QStringLiteral("::");
    }

    return QString::fromLatin1(resultBuffer);
}

QPair<QString, int> NetworkAddressParser::parseIPv4AddressPort(const QString &hexAddressPort)
{
    QStringList parts = hexAddressPort.split(':');
    if (parts.size() != 2) {
        return qMakePair(QString("0.0.0.0"), 0);
    }

    QString address = formatIPv4FromHex(parts.at(0));
    int port = parts.at(1).toInt(nullptr, 16);

    return qMakePair(address, port);
}

QPair<QString, int> NetworkAddressParser::parseIPv6AddressPort(const QString &hexAddressPort)
{
    QStringList parts = hexAddressPort.split(':');
    if (parts.size() != 2) {
        return qMakePair(QString("::"), 0);
    }

    QString address = formatIPv6FromHex(parts.at(0));
    int port = parts.at(1).toInt(nullptr, 16);

    return qMakePair(address, port);
}
