#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , processReader(new ProcessReader(this))
    , processTableModel(new ProcessTableModel(this))
    , systemStatReader(new SystemStatReader(this))
    , memInfoReader(new MemInfoReader(this))
    , networkConnectionReader(new NetworkConnectionReader(this))
    , networkTableModel(new NetworkTableModel(this))
    , processSignalSender(new ProcessSignalSender(this))
    , refreshTimer(new QTimer(this))
{
    ui->setupUi(this);

    setupModel();
    setupTimer();

    previousCpuSnapshot = systemStatReader->readOverallCpuSnapshot();

    refreshProcessList();
    refreshSystemInfo();
    refreshNetworkConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel()
{
    ui->processTableView->setModel(processTableModel);
    ui->processTableView->setSortingEnabled(true);
    ui->processTableView->setAlternatingRowColors(true);
    ui->processTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->networkTableView->setModel(networkTableModel);
    ui->networkTableView->setSortingEnabled(true);
    ui->networkTableView->setAlternatingRowColors(true);
    ui->networkTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->processTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->processTableView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onProcessTableContextMenuRequested);

    ui->networkTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->networkTableView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onNetworkTableContextMenuRequested);
}

void MainWindow::setupTimer()
{
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::onRefreshTimerTimeout);
    refreshTimer->start(2000);
}

void MainWindow::onRefreshTimerTimeout()
{
    refreshProcessList();
    refreshSystemInfo();
    refreshNetworkConnections();
}

void MainWindow::refreshProcessList()
{
    QVector<ProcessInfo> processList = processReader->readAllProcesses();
    processTableModel->updateProcessList(processList);
}

void MainWindow::refreshSystemInfo()
{
    CpuTimeSnapshot currentCpuSnapshot = systemStatReader->readOverallCpuSnapshot();
    double cpuUsage = systemStatReader->computeCpuUsagePercent(previousCpuSnapshot, currentCpuSnapshot);
    previousCpuSnapshot = currentCpuSnapshot;

    SystemMemorySnapshot memorySnapshot = memInfoReader->readMemorySnapshot();

    ui->cpuUsageLabel->setText(QString("CPU: %1%").arg(cpuUsage, 0, 'f', 1));
    ui->memoryUsageLabel->setText(QString("Memory: %1%").arg(memorySnapshot.usedMemoryPercent(), 0, 'f', 1));
}

void MainWindow::refreshNetworkConnections()
{
    QVector<NetworkConnectionInfo> connectionList = networkConnectionReader->readAllConnections();
    networkTableModel->updateConnectionList(connectionList);
}

void MainWindow::onProcessTableContextMenuRequested(const QPoint &position)
{
    QModelIndex clickedIndex = ui->processTableView->indexAt(position);
    if (!clickedIndex.isValid()) {
        return;
    }

    const ProcessInfo &processInfo = processTableModel->processAt(clickedIndex.row());
    QPoint globalPosition = ui->processTableView->viewport()->mapToGlobal(position);

    showProcessSignalMenu(processInfo.pid, globalPosition);
}

void MainWindow::onNetworkTableContextMenuRequested(const QPoint &position)
{
    QModelIndex clickedIndex = ui->networkTableView->indexAt(position);
    if (!clickedIndex.isValid()) {
        return;
    }

    const NetworkConnectionInfo &connectionInfo = networkTableModel->connectionAt(clickedIndex.row());
    if (connectionInfo.ownerPid == 0) {
        return;
    }

    QPoint globalPosition = ui->networkTableView->viewport()->mapToGlobal(position);

    showProcessSignalMenu(connectionInfo.ownerPid, globalPosition);
}

void MainWindow::showProcessSignalMenu(int targetPid, const QPoint &globalPosition)
{
    QMenu contextMenu(this);

    QAction *terminateAction = contextMenu.addAction(
        ProcessSignalSender::signalTypeToLabel(ProcessSignalType::Terminate));
    QAction *killAction = contextMenu.addAction(
        ProcessSignalSender::signalTypeToLabel(ProcessSignalType::Kill));
    QAction *suspendAction = contextMenu.addAction(
        ProcessSignalSender::signalTypeToLabel(ProcessSignalType::Suspend));
    QAction *resumeAction = contextMenu.addAction(
        ProcessSignalSender::signalTypeToLabel(ProcessSignalType::Resume));

    QAction *selectedAction = contextMenu.exec(globalPosition);

    if (selectedAction == terminateAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Terminate);
    } else if (selectedAction == killAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Kill);
    } else if (selectedAction == suspendAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Suspend);
    } else if (selectedAction == resumeAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Resume);
    }
}

void MainWindow::handleSignalRequest(int targetPid, ProcessSignalType signalType)
{
    processSignalSender->sendSignal(targetPid, signalType);

    refreshProcessList();
    refreshNetworkConnections();
}
