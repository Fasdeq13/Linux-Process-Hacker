#include "NetworkTableModel.h"

NetworkTableModel::NetworkTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int NetworkTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return connections.size();
}

int NetworkTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnCount;
}

QVariant NetworkTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= connections.size()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    const NetworkConnectionInfo &connectionInfo = connections.at(index.row());

    switch (index.column()) {
        case ColumnProtocol:
            return NetworkConnectionInfo::protocolToString(connectionInfo.protocol);
        case ColumnLocalAddress:
            return connectionInfo.localAddress;
        case ColumnLocalPort:
            return connectionInfo.localPort;
        case ColumnRemoteAddress:
            return connectionInfo.remoteAddress;
        case ColumnRemotePort:
            return connectionInfo.remotePort;
        case ColumnState:
            return NetworkConnectionInfo::stateToString(connectionInfo.state);
        case ColumnPid:
            return connectionInfo.ownerPid == 0 ? QString("-") : QString::number(connectionInfo.ownerPid);
        case ColumnProcessName:
            return connectionInfo.ownerProcessName.isEmpty() ? QString("-") : connectionInfo.ownerProcessName;
        default:
            return QVariant();
    }
}

QVariant NetworkTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
        case ColumnProtocol: return QStringLiteral("Protocol");
        case ColumnLocalAddress: return QStringLiteral("Local Address");
        case ColumnLocalPort: return QStringLiteral("Local Port");
        case ColumnRemoteAddress: return QStringLiteral("Remote Address");
        case ColumnRemotePort: return QStringLiteral("Remote Port");
        case ColumnState: return QStringLiteral("State");
        case ColumnPid: return QStringLiteral("PID");
        case ColumnProcessName: return QStringLiteral("Process");
        default: return QVariant();
    }
}

void NetworkTableModel::updateConnectionList(const QVector<NetworkConnectionInfo> &connectionList)
{
    beginResetModel();
    connections = connectionList;
    endResetModel();
}

const NetworkConnectionInfo &NetworkTableModel::connectionAt(int row) const
{
    return connections.at(row);
}
