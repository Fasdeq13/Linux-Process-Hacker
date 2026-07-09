#include "AutostartManager.h"
#include "DesktopFileParser.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QHash>

AutostartManager::AutostartManager(QObject *parent)
    : QObject(parent)
{
}

QString AutostartManager::userAutostartDirectoryPath() const
{
    QString configHome = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (configHome.isEmpty()) {
        configHome = QDir::homePath() + "/.config";
    }
    return configHome + "/autostart";
}

QString AutostartManager::systemAutostartDirectoryPath() const
{
    return QStringLiteral("/etc/xdg/autostart");
}

QVector<AutostartEntry> AutostartManager::scanDirectoryForEntries(const QString &directoryPath, bool isSystemWide)
{
    QVector<AutostartEntry> entries;

    QDir directory(directoryPath);
    if (!directory.exists()) {
        return entries;
    }

    QStringList nameFilters;
    nameFilters << "*.desktop";

    const QStringList fileNames = directory.entryList(nameFilters, QDir::Files);

    for (const QString &fileName : fileNames) {
        QString fullFilePath = directory.filePath(fileName);
        AutostartEntry entry = DesktopFileParser::buildEntryFromFile(fullFilePath, isSystemWide);
        entries.append(entry);
    }

    return entries;
}

QVector<AutostartEntry> AutostartManager::loadAllEntries()
{
    QHash<QString, AutostartEntry> mergedEntries;

    const QVector<AutostartEntry> systemEntries = scanDirectoryForEntries(systemAutostartDirectoryPath(), true);
    for (const AutostartEntry &systemEntry : systemEntries) {
        mergedEntries.insert(systemEntry.entryName, systemEntry);
    }

    const QVector<AutostartEntry> userEntries = scanDirectoryForEntries(userAutostartDirectoryPath(), false);
    for (const AutostartEntry &userEntry : userEntries) {
        mergedEntries.insert(userEntry.entryName, userEntry);
    }

    return mergedEntries.values().toVector();
}

QString AutostartManager::copySystemEntryToUserOverride(const AutostartEntry &systemEntry, QString &errorMessage)
{
    QString userDirectoryPath = userAutostartDirectoryPath();
    QDir userDirectory(userDirectoryPath);

    if (!userDirectory.exists()) {
        if (!userDirectory.mkpath(userDirectoryPath)) {
            errorMessage = QStringLiteral("Failed to create user autostart directory");
            return QString();
        }
    }

    QFileInfo systemFileInfo(systemEntry.filePath);
    QString overrideFilePath = userDirectory.filePath(systemFileInfo.fileName());

    if (QFile::exists(overrideFilePath)) {
        return overrideFilePath;
    }

    if (!QFile::copy(systemEntry.filePath, overrideFilePath)) {
        errorMessage = QStringLiteral("Failed to copy system entry to user autostart directory");
        return QString();
    }

    QFile overrideFile(overrideFilePath);
    overrideFile.setPermissions(overrideFile.permissions() | QFileDevice::WriteOwner);

    return overrideFilePath;
}

bool AutostartManager::setEntryEnabled(const AutostartEntry &entry, bool enabledValue, QString &errorMessage)
{
    QString targetFilePath = entry.filePath;

    if (entry.isSystemWide) {
        targetFilePath = copySystemEntryToUserOverride(entry, errorMessage);
        if (targetFilePath.isEmpty()) {
            return false;
        }
    }

    bool writeSuccess = DesktopFileParser::writeHiddenKey(targetFilePath, !enabledValue);
    if (!writeSuccess) {
        errorMessage = QStringLiteral("Failed to write autostart entry file, check permissions");
        return false;
    }

    return true;
}

bool AutostartManager::deleteEntry(const AutostartEntry &entry, QString &errorMessage)
{
    if (entry.isSystemWide) {
        errorMessage = QStringLiteral("Cannot delete a system-wide autostart entry, disable it instead");
        return false;
    }

    QFile entryFile(entry.filePath);
    if (!entryFile.remove()) {
        errorMessage = QStringLiteral("Failed to delete autostart entry file, check permissions");
        return false;
    }

    return true;
}
