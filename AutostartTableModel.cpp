#include "AutostartTableModel.h"

AutostartTableModel::AutostartTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int AutostartTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return entries.size();
}

int AutostartTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnCount;
}

QVariant AutostartTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= entries.size()) {
        return QVariant();
    }

    const AutostartEntry &entry = entries.at(index.row());

    if (role == Qt::CheckStateRole && index.column() == ColumnEnabled) {
        return entry.isEnabled ? Qt::Checked : Qt::Unchecked;
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (index.column()) {
        case ColumnEnabled:
            return QVariant();
        case ColumnName:
            return entry.displayName;
        case ColumnComment:
            return entry.comment;
        case ColumnExecCommand:
            return entry.execCommand;
        case ColumnScope:
            return entry.isSystemWide ? QStringLiteral("System") : QStringLiteral("User");
        default:
            return QVariant();
    }
}

QVariant AutostartTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case ColumnEnabled: return QStringLiteral("Enabled");
        case ColumnName: return QStringLiteral("Name");
        case ColumnComment: return QStringLiteral("Comment");
        case ColumnExecCommand: return QStringLiteral("Command");
        case ColumnScope: return QStringLiteral("Scope");
        default: return QVariant();
    }
}

Qt::ItemFlags AutostartTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags baseFlags = QAbstractTableModel::flags(index);

    if (index.column() == ColumnEnabled) {
        return baseFlags | Qt::ItemIsUserCheckable;
    }

    return baseFlags;
}

bool AutostartTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= entries.size()) {
        return false;
    }

    if (role == Qt::CheckStateRole && index.column() == ColumnEnabled) {
        bool newEnabledState = (value.toInt() == Qt::Checked);
        emit entryToggleRequested(index.row(), newEnabledState);
        return true;
    }

    return false;
}

void AutostartTableModel::updateEntryList(const QVector<AutostartEntry> &entryList)
{
    beginResetModel();
    entries = entryList;
    endResetModel();
}

const AutostartEntry &AutostartTableModel::entryAt(int row) const
{
    return entries.at(row);
}
