#include "ProcFsUtils.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRegularExpression>

QString ProcFsUtils::readFileContent(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    QTextStream stream(&file);
    return stream.readAll();
}

QStringList ProcFsUtils::readFileLines(const QString &filePath)
{
    QString content = readFileContent(filePath);
    if (content.isEmpty()) {
        return QStringList();
    }
    return content.split('\n', Qt::SkipEmptyParts);
}

QStringList ProcFsUtils::splitFields(const QString &line)
{
    return line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

bool ProcFsUtils::isNumericDirectoryName(const QString &name)
{
    if (name.isEmpty()) {
        return false;
    }
    for (const QChar &character : name) {
        if (!character.isDigit()) {
            return false;
        }
    }
    return true;
}

QVector<int> ProcFsUtils::listProcessIds()
{
    QVector<int> processIds;
    QDir procDirectory("/proc");
    const QStringList entries = procDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        if (isNumericDirectoryName(entry)) {
            processIds.append(entry.toInt());
        }
    }
    return processIds;
}

std::optional<qint64> ProcFsUtils::readLongLongValue(const QString &filePath)
{
    QString content = readFileContent(filePath).trimmed();
    if (content.isEmpty()) {
        return std::nullopt;
    }
    bool conversionOk = false;
    qint64 value = content.toLongLong(&conversionOk);
    if (!conversionOk) {
        return std::nullopt;
    }
    return value;
}

QString ProcFsUtils::procPath(int pid, const QString &suffix)
{
    return QString("/proc/%1/%2").arg(pid).arg(suffix);
}

QString ProcFsUtils::procPath(int pid, int tid, const QString &suffix)
{
    return QString("/proc/%1/task/%2/%3").arg(pid).arg(tid).arg(suffix);
}
