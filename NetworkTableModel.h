#ifndef NETWORKTABLEMODEL_H
#define NETWORKTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "NetworkConnectionInfo.h"

class NetworkTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnIndex
    {
        ColumnProtocol = 0,
        ColumnLocalAddress,
        ColumnLocalPort,
        ColumnRemoteAddress,
        ColumnRemotePort,
        ColumnState,
        ColumnPid,
        ColumnProcessName,
        ColumnCount
    };

    explicit NetworkTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void updateConnectionList(const QVector<NetworkConnectionInfo> &connectionList);
    const NetworkConnectionInfo &connectionAt(int row) const;

private:
    QVector<NetworkConnectionInfo> connections;
};

#endif
