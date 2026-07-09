#include "MemoryDumpDialog.h"
#include "ui_MemoryDumpDialog.h"

#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QListWidgetItem>

MemoryDumpDialog::MemoryDumpDialog(int targetPid, const QString &targetProcessName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MemoryDumpDialog)
    , memoryMapsParser(new MemoryMapsParser(this))
    , processMemoryDumper(new ProcessMemoryDumper(this))
    , processId(targetPid)
    , processName(targetProcessName)
{
    ui->setupUi(this);

    ui->targetProcessLabel->setText(QString("Target process: %1 (PID %2)").arg(processName).arg(processId));

    connect(ui->regionsListWidget, &QListWidget::currentRowChanged,
            this, &MemoryDumpDialog::onRegionSelectionChanged);
    connect(ui->dumpRawButton, &QPushButton::clicked,
            this, &MemoryDumpDialog::onDumpRawButtonClicked);
    connect(ui->extractStringsButton, &QPushButton::clicked,
            this, &MemoryDumpDialog::onExtractStringsButtonClicked);
    connect(ui->refreshRegionsButton, &QPushButton::clicked,
            this, &MemoryDumpDialog::onRefreshRegionsButtonClicked);

    populateRegionList();
}

MemoryDumpDialog::~MemoryDumpDialog()
{
    delete ui;
}

void MemoryDumpDialog::populateRegionList()
{
    ui->regionsListWidget->clear();
    loadedRegions = memoryMapsParser->parseMapsForProcess(processId);

    for (const MemoryRegion &region : loadedRegions) {
        QString displayLabel = QString("0x%1 - 0x%2  [%3]  %4")
            .arg(region.startAddress, 0, 16)
            .arg(region.endAddress, 0, 16)
            .arg(region.permissions)
            .arg(region.pathName.isEmpty() ? QStringLiteral("[anonymous]") : region.pathName);

        ui->regionsListWidget->addItem(displayLabel);
    }

    if (loadedRegions.isEmpty()) {
        ui->regionDetailsLabel->setText(QStringLiteral("No memory regions found, process may have exited"));
    }
}

void MemoryDumpDialog::onRefreshRegionsButtonClicked()
{
    populateRegionList();
    ui->extractedStringsListWidget->clear();
}

MemoryRegion MemoryDumpDialog::currentlySelectedRegion() const
{
    int currentRow = ui->regionsListWidget->currentRow();
    if (currentRow < 0 || currentRow >= loadedRegions.size()) {
        return MemoryRegion();
    }
    return loadedRegions.at(currentRow);
}

void MemoryDumpDialog::onRegionSelectionChanged(int currentRow)
{
    if (currentRow < 0 || currentRow >= loadedRegions.size()) {
        ui->regionDetailsLabel->setText(QStringLiteral("Select a memory region above"));
        return;
    }

    const MemoryRegion &region = loadedRegions.at(currentRow);
    ui->regionDetailsLabel->setText(
        QString("Size: %1 KB   Readable: %2")
            .arg(region.sizeInBytes() / 1024)
            .arg(region.isReadable() ? QStringLiteral("Yes") : QStringLiteral("No")));
}

QString MemoryDumpDialog::buildDefaultOutputPath(const QString &suffix)
{
    QString desktopDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (desktopDirectoryPath.isEmpty()) {
        desktopDirectoryPath = QDir::homePath();
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString fileName = QString("%1_pid%2_%3.%4").arg(processName).arg(processId).arg(timestamp).arg(suffix);

    return QDir(desktopDirectoryPath).filePath(fileName);
}

void MemoryDumpDialog::onDumpRawButtonClicked()
{
    MemoryRegion selectedRegion = currentlySelectedRegion();

    if (selectedRegion.sizeInBytes() == 0) {
        QMessageBox::warning(this, QStringLiteral("Memory Dumper"), QStringLiteral("Please select a valid memory region first"));
        return;
    }

    QString outputFilePath = buildDefaultOutputPath(QStringLiteral("bin"));
    QString errorMessage;

    bool dumpSuccess = processMemoryDumper->dumpRegionToFile(processId, selectedRegion, outputFilePath, errorMessage);

    if (dumpSuccess) {
        QMessageBox::information(this, QStringLiteral("Memory Dumper"),
                                  QString("Memory region dumped to:\n%1").arg(outputFilePath));
    } else {
        QMessageBox::critical(this, QStringLiteral("Memory Dumper"), errorMessage);
    }
}

void MemoryDumpDialog::onExtractStringsButtonClicked()
{
    MemoryRegion selectedRegion = currentlySelectedRegion();

    if (selectedRegion.sizeInBytes() == 0) {
        QMessageBox::warning(this, QStringLiteral("Memory Dumper"), QStringLiteral("Please select a valid memory region first"));
        return;
    }

    ui->extractedStringsListWidget->clear();

    QVector<ExtractedStringEntry> extractedStrings = processMemoryDumper->extractStringsFromRegion(processId, selectedRegion);

    if (extractedStrings.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Memory Dumper"),
                              QStringLiteral("No strings extracted, root privileges may be required to read this process memory"));
        return;
    }

    for (const ExtractedStringEntry &entry : extractedStrings) {
        QString displayLine = QString("0x%1  %2").arg(entry.address, 0, 16).arg(entry.text);
        ui->extractedStringsListWidget->addItem(displayLine);
    }
}
