#ifndef DESKTOPFILEPARSER_H
#define DESKTOPFILEPARSER_H

#include <QString>
#include <QHash>
#include "AutostartEntry.h"

class DesktopFileParser
{
public:
    static QHash<QString, QString> parseKeyValuePairs(const QString &filePath);
    static AutostartEntry buildEntryFromFile(const QString &filePath, bool isSystemWide);
    static bool writeHiddenKey(const QString &filePath, bool hiddenValue);

private:
    static QString stripInlineComment(const QString &line);
};

#endif
