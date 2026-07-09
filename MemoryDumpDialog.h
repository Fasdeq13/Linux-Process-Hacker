#ifndef MEMORYDUMPDIALOG_H
#define MEMORYDUMPDIALOG_H

#include <QDialog>
#include <QVector>
#include "MemoryRegion.h"
#include "MemoryMapsParser.h"
#include "ProcessMemoryDumper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MemoryDumpDialog; }
QT_END_NAMESPACE

class MemoryDumpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryDumpDialog(int targetPid, const QString &targetProcessName, QWidget *parent = nullptr);
    ~MemoryDumpDialog();

private slots:
    void onRegionSelectionChanged(int currentRow);
    void onDumpRawButtonClicked();
    void onExtractStringsButtonClicked();
    void onRefreshRegionsButtonClicked();

private:
    void populateRegionList();
    QString buildDefaultOutputPath(const QString &suffix);
    MemoryRegion currentlySelectedRegion() const;

    Ui::MemoryDumpDialog *ui;
    MemoryMapsParser *memoryMapsParser;
    ProcessMemoryDumper *processMemoryDumper;
    QVector<MemoryRegion> loadedRegions;
    int processId;
    QString processName;
};

#endif
