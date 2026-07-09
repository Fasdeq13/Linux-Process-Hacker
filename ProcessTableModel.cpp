#include "ProcessTableModel.h"

#include <QBrush>
#include <QColor>

ProcessTableModel::ProcessTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ProcessTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return processes.size();
}

int ProcessTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnCount;
}

QVariant ProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= processes.size()) {
        return QVariant();
    }

    const ProcessInfo &processInfo = processes.at(index.row());

    if (role == Qt::BackgroundRole) {
        if (hiddenProcessIdSet.contains(processInfo.pid) || processInfo.isHiddenFromProcFs) {
            return QBrush(QColor(180, 30, 30));
        }
        return QVariant();
    }

    if (role == Qt::ForegroundRole) {
        if (hiddenProcessIdSet.contains(processInfo.pid) || processInfo.isHiddenFromProcFs) {
            return QBrush(QColor(255, 255, 255));
        }
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (index.column()) {
        case ColumnPid:
            return processInfo.pid;
        case ColumnName:
            return processInfo.name;
        case ColumnUser:
            return processInfo.userName;
        case ColumnState:
            return ProcessInfo::stateToString(processInfo.state);
        case ColumnCpuPercent:
            return QString::number(processInfo.cpuUsagePercent, 'f', 1);
        case ColumnMemoryPercent:
            return QString::number(processInfo.memoryUsagePercent, 'f', 1);
        case ColumnResidentMemory:
            return QString::number(processInfo.residentMemoryBytes / 1024);
        case ColumnThreads:
            return processInfo.threadCount;
        case ColumnDiskRead:
            return QString::number(processInfo.diskReadBytesPerSec / 1024.0, 'f', 1);
        case ColumnDiskWrite:
            return QString::number(processInfo.diskWriteBytesPerSec / 1024.0, 'f', 1);
        default:
            return QVariant();
    }
}

QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case ColumnPid: return QStringLiteral("PID");
        case ColumnName: return QStringLiteral("Name");
        case ColumnUser: return QStringLiteral("User");
        case ColumnState: return QStringLiteral("State");
        case ColumnCpuPercent: return QStringLiteral("CPU %");
        case ColumnMemoryPercent: return QStringLiteral("Mem %");
        case ColumnResidentMemory: return QStringLiteral("RSS (KB)");
        case ColumnThreads: return QStringLiteral("Threads");
        case ColumnDiskRead: return QStringLiteral("Disk Read (KB/s)");
        case ColumnDiskWrite: return QStringLiteral("Disk Write (KB/s)");
        default: return QVariant();
    }
}

void ProcessTableModel::updateProcessList(const QVector<ProcessInfo> &processList)
{
    beginResetModel();
    processes = processList;
    endResetModel();
}

void ProcessTableModel::markHiddenProcessIds(const QSet<int> &hiddenProcessIds)
{
    hiddenProcessIdSet = hiddenProcessIds;

    if (rowCount() > 0) {
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1),
                          {Qt::BackgroundRole, Qt::ForegroundRole});
    }
}

const ProcessInfo &ProcessTableModel::processAt(int row) const
{
    return processes.at(row);
}
