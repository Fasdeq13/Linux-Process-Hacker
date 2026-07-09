#include "DesktopFileParser.h"
#include "ProcFsUtils.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

QString DesktopFileParser::stripInlineComment(const QString &line)
{
    QString trimmedLine = line.trimmed();
    if (trimmedLine.startsWith('#')) {
        return QString();
    }
    return trimmedLine;
}

QHash<QString, QString> DesktopFileParser::parseKeyValuePairs(const QString &filePath)
{
    QHash<QString, QString> keyValuePairs;
    QStringList lines = ProcFsUtils::readFileLines(filePath);

    bool insideDesktopEntrySection = false;

    for (const QString &rawLine : lines) {
        QString line = stripInlineComment(rawLine);
        if (line.isEmpty()) {
            continue;
        }

        if (line.startsWith('[')) {
            insideDesktopEntrySection = (line == QStringLiteral("[Desktop Entry]"));
            continue;
        }

        if (!insideDesktopEntrySection) {
            continue;
        }

        int equalsSignIndex = line.indexOf('=');
        if (equalsSignIndex <= 0) {
            continue;
        }

        QString key = line.left(equalsSignIndex).trimmed();
        QString value = line.mid(equalsSignIndex + 1).trimmed();

        keyValuePairs.insert(key, value);
    }

    return keyValuePairs;
}

AutostartEntry DesktopFileParser::buildEntryFromFile(const QString &filePath, bool isSystemWide)
{
    AutostartEntry entry;
    entry.filePath = filePath;
    entry.isSystemWide = isSystemWide;

    QFileInfo fileInfo(filePath);
    entry.entryName = fileInfo.completeBaseName();

    QHash<QString, QString> keyValuePairs = parseKeyValuePairs(filePath);

    entry.displayName = keyValuePairs.value(QStringLiteral("Name"), entry.entryName);
    entry.comment = keyValuePairs.value(QStringLiteral("Comment"));
    entry.execCommand = keyValuePairs.value(QStringLiteral("Exec"));
    entry.iconName = keyValuePairs.value(QStringLiteral("Icon"));

    bool hiddenFlag = (keyValuePairs.value(QStringLiteral("Hidden")).compare(
        QStringLiteral("true"), Qt::CaseInsensitive) == 0);

    bool gnomeDisabledFlag = (keyValuePairs.value(QStringLiteral("X-GNOME-Autostart-enabled")).compare(
        QStringLiteral("false"), Qt::CaseInsensitive) == 0);

    entry.isEnabled = !hiddenFlag && !gnomeDisabledFlag;

    return entry;
}

bool DesktopFileParser::writeHiddenKey(const QString &filePath, bool hiddenValue)
{
    QStringList originalLines = ProcFsUtils::readFileLines(filePath);
    if (originalLines.isEmpty()) {
        return false;
    }

    QStringList resultLines;
    bool insideDesktopEntrySection = false;
    bool keyWasReplaced = false;

    for (const QString &originalLine : originalLines) {
        QString trimmedLine = originalLine.trimmed();

        if (trimmedLine.startsWith('[')) {
            insideDesktopEntrySection = (trimmedLine == QStringLiteral("[Desktop Entry]"));
            resultLines.append(originalLine);
            continue;
        }

        if (insideDesktopEntrySection && trimmedLine.startsWith(QStringLiteral("X-GNOME-Autostart-enabled="))) {
            resultLines.append(QString("X-GNOME-Autostart-enabled=%1").arg(hiddenValue ? "false" : "true"));
            keyWasReplaced = true;
            continue;
        }

        resultLines.append(originalLine);
    }

    if (!keyWasReplaced) {
        QStringList finalLines;
        bool insertedKey = false;

        for (const QString &line : resultLines) {
            finalLines.append(line);
            QString trimmedLine = line.trimmed();
            if (!insertedKey && trimmedLine == QStringLiteral("[Desktop Entry]")) {
                finalLines.append(QString("X-GNOME-Autostart-enabled=%1").arg(hiddenValue ? "false" : "true"));
                insertedKey = true;
            }
        }

        resultLines = finalLines;
    }

    QFile outputFile(filePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream outputStream(&outputFile);
    for (const QString &line : resultLines) {
        outputStream << line << '\n';
    }

    outputFile.close();

    return true;
}
