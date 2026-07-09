#ifndef PROCESSTABLEMODEL_H
#define PROCESSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QSet>
#include "ProcessInfo.h"

class ProcessTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnIndex
    {
        ColumnPid = 0,
        ColumnName,
        ColumnUser,
        ColumnState,
        ColumnCpuPercent,
        ColumnMemoryPercent,
        ColumnResidentMemory,
        ColumnThreads,
        ColumnDiskRead,
        ColumnDiskWrite,
        ColumnCount
    };

    explicit ProcessTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void updateProcessList(const QVector<ProcessInfo> &processList);
    void markHiddenProcessIds(const QSet<int> &hiddenProcessIds);
    const ProcessInfo &processAt(int row) const;

private:
    QVector<ProcessInfo> processes;
    QSet<int> hiddenProcessIdSet;
};

#endif
