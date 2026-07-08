#ifndef NETWORKADDRESSPARSER_H
#define NETWORKADDRESSPARSER_H

#include <QString>
#include <QPair>

class NetworkAddressParser
{
public:
    static QPair<QString, int> parseIPv4AddressPort(const QString &hexAddressPort);
    static QPair<QString, int> parseIPv6AddressPort(const QString &hexAddressPort);

private:
    static QString formatIPv4FromHex(const QString &hexAddress);
    static QString formatIPv6FromHex(const QString &hexAddress);
};

#endif
