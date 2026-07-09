#ifndef AUTOSTARTTABLEMODEL_H
#define AUTOSTARTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "AutostartEntry.h"

class AutostartTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnIndex
    {
        ColumnEnabled = 0,
        ColumnName,
        ColumnComment,
        ColumnExecCommand,
        ColumnScope,
        ColumnCount
    };

    explicit AutostartTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void updateEntryList(const QVector<AutostartEntry> &entryList);
    const AutostartEntry &entryAt(int row) const;

signals:
    void entryToggleRequested(int row, bool newEnabledState);

private:
    QVector<AutostartEntry> entries;
};

#endif
