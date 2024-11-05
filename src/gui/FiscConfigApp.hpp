#ifndef FISC_CONFIG_APP_HPP
#define FISC_CONFIG_APP_HPP

#include <QMainWindow>
#include <QPushButton>
#include "../config/FiscConfigParser.hpp"

class FiscConfigApp : public QMainWindow {
    Q_OBJECT

public:
    FiscConfigApp(QWidget *parent = nullptr);

private slots:
    void loadConfig();
    void editConfig();

private:
    QPushButton *loadButton;
    QPushButton *editButton;
    FiscConfigParser configParser;
};

#endif // FISC_CONFIG_APP_HPP 