#include <QApplication>
#include "FiscConfigApp.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FiscConfigApp window;
    window.show();
    return app.exec();
} 