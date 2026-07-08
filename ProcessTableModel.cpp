#include "ProcessTableModel.h"

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

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    const ProcessInfo &processInfo = processes.at(index.row());

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
        default: return QVariant();
    }
}

void ProcessTableModel::updateProcessList(const QVector<ProcessInfo> &processList)
{
    beginResetModel();
    processes = processList;
    endResetModel();
}

const ProcessInfo &ProcessTableModel::processAt(int row) const
{
    return processes.at(row);
}
