#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MemoryDumpDialog.h"

#include <QKeyEvent>
#include <QShortcut>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , processReader(new ProcessReader(this))
    , processTableModel(new ProcessTableModel(this))
    , processFilterProxyModel(new QSortFilterProxyModel(this))
    , systemStatReader(new SystemStatReader(this))
    , memInfoReader(new MemInfoReader(this))
    , networkConnectionReader(new NetworkConnectionReader(this))
    , networkTableModel(new NetworkTableModel(this))
    , processSignalSender(new ProcessSignalSender(this))
    , autostartManager(new AutostartManager(this))
    , autostartTableModel(new AutostartTableModel(this))
    , refreshTimer(new QTimer(this))
    , rootkitScanWorker(nullptr)
{
    ui->setupUi(this);

    setupModel();
    setupTimer();
    setupSearchBar();
    setupRootkitWorkerThread();

    previousCpuSnapshot = systemStatReader->readOverallCpuSnapshot();

    refreshProcessList();
    refreshSystemInfo();
    refreshNetworkConnections();
    refreshAutostartEntries();
}

MainWindow::~MainWindow()
{
    rootkitWorkerThread.quit();
    rootkitWorkerThread.wait();

    delete ui;
}

void MainWindow::setupModel()
{
    processFilterProxyModel->setSourceModel(processTableModel);
    processFilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    processFilterProxyModel->setFilterKeyColumn(-1);

    ui->processTableView->setModel(processFilterProxyModel);
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

    connect(ui->scanRootkitButton, &QPushButton::clicked,
            this, &MainWindow::onScanRootkitButtonClicked);

    ui->autostartTableView->setModel(autostartTableModel);
    ui->autostartTableView->setAlternatingRowColors(true);
    ui->autostartTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(autostartTableModel, &AutostartTableModel::entryToggleRequested,
            this, &MainWindow::onAutostartEntryToggleRequested);
    connect(ui->refreshAutostartButton, &QPushButton::clicked,
            this, &MainWindow::onRefreshAutostartButtonClicked);
    connect(ui->removeAutostartButton, &QPushButton::clicked,
            this, &MainWindow::onRemoveAutostartButtonClicked);
}

void MainWindow::setupSearchBar()
{
    ui->searchBarWidget->setVisible(false);

    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);
    connect(ui->closeSearchButton, &QPushButton::clicked,
            this, &MainWindow::onSearchBarCloseRequested);

    QShortcut *searchShortcut = new QShortcut(QKeySequence::Find, this);
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::toggleSearchBarVisibility);
}

void MainWindow::setupRootkitWorkerThread()
{
    rootkitScanWorker = new RootkitScanWorker();
    rootkitScanWorker->moveToThread(&rootkitWorkerThread);

    connect(&rootkitWorkerThread, &QThread::finished, rootkitScanWorker, &QObject::deleteLater);
    connect(rootkitScanWorker, &RootkitScanWorker::scanFinished, this, &MainWindow::onRootkitScanFinished);

    rootkitWorkerThread.start();
}

void MainWindow::setupTimer()
{
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::onRefreshTimerTimeout);
    refreshTimer->start(2000);
}

void MainWindow::toggleSearchBarVisibility()
{
    bool willBeVisible = !ui->searchBarWidget->isVisible();
    ui->searchBarWidget->setVisible(willBeVisible);

    if (willBeVisible) {
        ui->searchLineEdit->setFocus();
        ui->searchLineEdit->selectAll();
    } else {
        ui->searchLineEdit->clear();
    }
}

void MainWindow::onSearchTextChanged(const QString &searchText)
{
    processFilterProxyModel->setFilterFixedString(searchText);
}

void MainWindow::onSearchBarCloseRequested()
{
    ui->searchLineEdit->clear();
    ui->searchBarWidget->setVisible(false);
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

    lastKnownVisiblePids.clear();
    for (const ProcessInfo &processInfo : processList) {
        lastKnownVisiblePids.insert(processInfo.pid);
    }
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

void MainWindow::onScanRootkitButtonClicked()
{
    ui->scanRootkitButton->setEnabled(false);
    ui->scanRootkitButton->setText(QStringLiteral("Scanning..."));

    QMetaObject::invokeMethod(rootkitScanWorker, "runScan", Qt::QueuedConnection,
                               Q_ARG(QSet<int>, lastKnownVisiblePids));
}

void MainWindow::onRootkitScanFinished(const QVector<int> &hiddenProcessIds)
{
    ui->scanRootkitButton->setEnabled(true);
    ui->scanRootkitButton->setText(QStringLiteral("Scan for Hidden Processes"));

    QSet<int> hiddenSet;
    for (int hiddenPid : hiddenProcessIds) {
        hiddenSet.insert(hiddenPid);
    }

    processTableModel->markHiddenProcessIds(hiddenSet);

    if (!hiddenProcessIds.isEmpty()) {
        QString pidListText;
        for (int hiddenPid : hiddenProcessIds) {
            pidListText += QString::number(hiddenPid) + " ";
        }
        ui->statusbar->showMessage(QString("WARNING: %1 hidden process(es) detected: %2")
                                        .arg(hiddenProcessIds.size())
                                        .arg(pidListText));
    } else {
        ui->statusbar->showMessage(QStringLiteral("No hidden processes detected"), 5000);
    }
}

void MainWindow::onProcessTableContextMenuRequested(const QPoint &position)
{
    QModelIndex proxyIndex = ui->processTableView->indexAt(position);
    if (!proxyIndex.isValid()) {
        return;
    }

    QModelIndex sourceIndex = processFilterProxyModel->mapToSource(proxyIndex);
    const ProcessInfo &processInfo = processTableModel->processAt(sourceIndex.row());
    QPoint globalPosition = ui->processTableView->viewport()->mapToGlobal(position);

    showProcessSignalMenu(processInfo.pid, globalPosition, true);
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

    showProcessSignalMenu(connectionInfo.ownerPid, globalPosition, false);
}

void MainWindow::showProcessSignalMenu(int targetPid, const QPoint &globalPosition, bool showMemoryDumpOption)
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

    QAction *memoryDumpAction = nullptr;
    if (showMemoryDumpOption) {
        contextMenu.addSeparator();
        memoryDumpAction = contextMenu.addAction(QStringLiteral("Memory Dump..."));
    }

    QAction *selectedAction = contextMenu.exec(globalPosition);

    if (selectedAction == terminateAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Terminate);
    } else if (selectedAction == killAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Kill);
    } else if (selectedAction == suspendAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Suspend);
    } else if (selectedAction == resumeAction) {
        handleSignalRequest(targetPid, ProcessSignalType::Resume);
    } else if (memoryDumpAction != nullptr && selectedAction == memoryDumpAction) {
        std::optional<ProcessInfo> processInfo = processReader->readSingleProcess(targetPid);
        QString processName = processInfo.has_value() ? processInfo.value().name : QString::number(targetPid);
        openMemoryDumpDialog(targetPid, processName);
    }
}

void MainWindow::openMemoryDumpDialog(int targetPid, const QString &targetProcessName)
{
    MemoryDumpDialog dumpDialog(targetPid, targetProcessName, this);
    dumpDialog.exec();
}

void MainWindow::handleSignalRequest(int targetPid, ProcessSignalType signalType)
{
    processSignalSender->sendSignal(targetPid, signalType);

    refreshProcessList();
    refreshNetworkConnections();
}

void MainWindow::refreshAutostartEntries()
{
    QVector<AutostartEntry> entryList = autostartManager->loadAllEntries();
    autostartTableModel->updateEntryList(entryList);
}

void MainWindow::onRefreshAutostartButtonClicked()
{
    refreshAutostartEntries();
}

void MainWindow::onAutostartEntryToggleRequested(int row, bool newEnabledState)
{
    const AutostartEntry &entry = autostartTableModel->entryAt(row);

    QString errorMessage;
    bool operationSuccess = autostartManager->setEntryEnabled(entry, newEnabledState, errorMessage);

    if (!operationSuccess) {
        QMessageBox::warning(this, QStringLiteral("Autostart"), errorMessage);
    }

    refreshAutostartEntries();
}

void MainWindow::onRemoveAutostartButtonClicked()
{
    QModelIndex currentIndex = ui->autostartTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::information(this, QStringLiteral("Autostart"), QStringLiteral("Select an entry first"));
        return;
    }

    const AutostartEntry &entry = autostartTableModel->entryAt(currentIndex.row());

    QMessageBox::StandardButton confirmationResult = QMessageBox::question(
        this, QStringLiteral("Remove Autostart Entry"),
        QString("Are you sure you want to remove \"%1\" from autostart?").arg(entry.displayName));

    if (confirmationResult != QMessageBox::Yes) {
        return;
    }

    QString errorMessage;
    bool operationSuccess = autostartManager->deleteEntry(entry, errorMessage);

    if (!operationSuccess) {
        QMessageBox::warning(this, QStringLiteral("Autostart"), errorMessage);
    }

    refreshAutostartEntries();
}
