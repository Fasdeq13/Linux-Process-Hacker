#ifndef AUTOSTARTMANAGER_H
#define AUTOSTARTMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include "AutostartEntry.h"

class AutostartManager : public QObject
{
    Q_OBJECT

public:
    explicit AutostartManager(QObject *parent = nullptr);

    QVector<AutostartEntry> loadAllEntries();
    bool setEntryEnabled(const AutostartEntry &entry, bool enabledValue, QString &errorMessage);
    bool deleteEntry(const AutostartEntry &entry, QString &errorMessage);

private:
    QString userAutostartDirectoryPath() const;
    QString systemAutostartDirectoryPath() const;
    QVector<AutostartEntry> scanDirectoryForEntries(const QString &directoryPath, bool isSystemWide);
    QString copySystemEntryToUserOverride(const AutostartEntry &systemEntry, QString &errorMessage);
};

#endif
