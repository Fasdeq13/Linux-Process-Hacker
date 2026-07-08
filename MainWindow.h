#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMenu>
#include <QPoint>
#include "ProcessReader.h"
#include "ProcessTableModel.h"
#include "SystemStatReader.h"
#include "MemInfoReader.h"
#include "NetworkConnectionReader.h"
#include "NetworkTableModel.h"
#include "ProcessSignalSender.h"

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

private:
    void setupModel();
    void setupTimer();
    void refreshProcessList();
    void refreshSystemInfo();
    void refreshNetworkConnections();
    void showProcessSignalMenu(int targetPid, const QPoint &globalPosition);
    void handleSignalRequest(int targetPid, ProcessSignalType signalType);

    Ui::MainWindow *ui;
    ProcessReader *processReader;
    ProcessTableModel *processTableModel;
    SystemStatReader *systemStatReader;
    MemInfoReader *memInfoReader;
    NetworkConnectionReader *networkConnectionReader;
    NetworkTableModel *networkTableModel;
    ProcessSignalSender *processSignalSender;
    QTimer *refreshTimer;
    CpuTimeSnapshot previousCpuSnapshot;
};

#endif
