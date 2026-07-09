#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>
#include <QPoint>
#include <QThread>
#include <QSortFilterProxyModel>
#include <QSet>
#include <QKeyEvent>
#include "ProcessReader.h"
#include "ProcessTableModel.h"
#include "SystemStatReader.h"
#include "MemInfoReader.h"
#include "NetworkConnectionReader.h"
#include "NetworkTableModel.h"
#include "ProcessSignalSender.h"
#include "RootkitScanWorker.h"
#include "AutostartManager.h"
#include "AutostartTableModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRefreshTimerTimeout();
    void onProcessTableContextMenuRequested(const QPoint &position);
    void onNetworkTableContextMenuRequested(const QPoint &position);
    void onScanRootkitButtonClicked();
    void onRootkitScanFinished(const QVector<int> &hiddenProcessIds);
    void onSearchTextChanged(const QString &searchText);
    void onSearchBarCloseRequested();
    void onAutostartEntryToggleRequested(int row, bool newEnabledState);
    void onRefreshAutostartButtonClicked();
    void onRemoveAutostartButtonClicked();

private:
    void setupModel();
    void setupTimer();
    void setupSearchBar();
    void setupRootkitWorkerThread();
    void refreshProcessList();
    void refreshSystemInfo();
    void refreshNetworkConnections();
    void refreshAutostartEntries();
    void showProcessSignalMenu(int targetPid, const QPoint &globalPosition, bool showMemoryDumpOption);
    void handleSignalRequest(int targetPid, ProcessSignalType signalType);
    void openMemoryDumpDialog(int targetPid, const QString &targetProcessName);
    void toggleSearchBarVisibility();

    Ui::MainWindow *ui;
    ProcessReader *processReader;
    ProcessTableModel *processTableModel;
    QSortFilterProxyModel *processFilterProxyModel;
    SystemStatReader *systemStatReader;
    MemInfoReader *memInfoReader;
    NetworkConnectionReader *networkConnectionReader;
    NetworkTableModel *networkTableModel;
    ProcessSignalSender *processSignalSender;
    AutostartManager *autostartManager;
    AutostartTableModel *autostartTableModel;
    QTimer *refreshTimer;
    CpuTimeSnapshot previousCpuSnapshot;

    QThread rootkitWorkerThread;
    RootkitScanWorker *rootkitScanWorker;
    QSet<int> lastKnownVisiblePids;
};

#endif
