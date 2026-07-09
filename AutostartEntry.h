#ifndef AUTOSTARTENTRY_H
#define AUTOSTARTENTRY_H

#include <QString>
#include <QMetaType>

class AutostartEntry
{
public:
    AutostartEntry();

    QString filePath;
    QString entryName;
    QString displayName;
    QString comment;
    QString execCommand;
    QString iconName;
    bool isEnabled = true;
    bool isSystemWide = false;
};

Q_DECLARE_METATYPE(AutostartEntry)

#endif
