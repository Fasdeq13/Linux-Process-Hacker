#ifndef PROCFSUTILS_H
#define PROCFSUTILS_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <optional>

class ProcFsUtils
{
public:
    static QString readFileContent(const QString &filePath);
    static QStringList readFileLines(const QString &filePath);
    static QStringList splitFields(const QString &line);
    static bool isNumericDirectoryName(const QString &name);
    static QVector<int> listProcessIds();
    static std::optional<qint64> readLongLongValue(const QString &filePath);
    static QString procPath(int pid, const QString &suffix);
    static QString procPath(int pid, int tid, const QString &suffix);
};

#endif
