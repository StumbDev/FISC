#include "FiscConfigApp.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

FiscConfigApp::FiscConfigApp(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("FISC-V Configuration Manager");
    setFixedSize(300, 150);

    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto layout = new QVBoxLayout(centralWidget);

    loadButton = new QPushButton("Load Config", this);
    loadButton->setToolTip("Load a FISC-V configuration file");
    layout->addWidget(loadButton);

    editButton = new QPushButton("Edit Config", this);
    editButton->setToolTip("Edit the current configuration");
    layout->addWidget(editButton);

    connect(loadButton, &QPushButton::clicked, this, &FiscConfigApp::loadConfig);
    connect(editButton, &QPushButton::clicked, this, &FiscConfigApp::editConfig);
}

void FiscConfigApp::loadConfig() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Load Configuration", "",
        "FXml Files (*.fxml);;All Files (*)");

    if (!filename.isEmpty()) {
        if (configParser.loadConfig(filename.toStdString())) {
            QMessageBox::information(this, "Success", "Configuration loaded successfully");
        } else {
            QMessageBox::critical(this, "Error", "Failed to load configuration");
        }
    }
}

void FiscConfigApp::editConfig() {
    QMessageBox::information(this, "Edit Config",
        "Edit configuration feature will be implemented here");
} 