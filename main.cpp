#include "mainwindow.h"
#include "logindialog.h"
#include <QDebug>
#include <QApplication>
#include "database.h"
#include "itemselectdialog.h"

void openDB(QSqlDatabase &db);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("New Rome");
    QCoreApplication::setApplicationName("Inventory Manager");
    QCoreApplication::setApplicationVersion("0.9.6");

    QSqlDatabase db;
    /// SET DB DRIVER
    db = QSqlDatabase::addDatabase("QSQLITE");
    /// OPEN THE DB
    openDB(db);
    /// INITIALIZE THE NEWLY OPENED DATABASE
    if(!initDatabase(db))
    {
        db.close();
        return 1;
    }

    /// OPEN LOGIN WINDOW
    sLogin login;
    LoginDialog l(db, login);
    if(l.exec() == l.Rejected)
        return 7;

    /// OPEN MAIN WINDOW
    MainWindow w(db, login);
    w.show();

    return a.exec();
}

void openDB(QSqlDatabase &db)
{
    QSettings settings;
#ifdef DEBUG_MODE
    QString databasePath = "debug/databasePath";
#else
    QString databasePath = "databasePath";
#endif
    if(settings.contains(databasePath) &&
            !settings.value(databasePath).toString().isEmpty()) {
        QString fileName = settings.value(databasePath).toString();
        if (QFile(fileName).exists()) {
            db.setDatabaseName(fileName);
            return;
        }
    }
    QString path("P:/SHIPPING & RECEIVING/Inventory/Inventory Manager/Database/INVENTORY.sqlite");
    QString fileName = QFileDialog::getOpenFileName(0, "Open Database",
                                            path,
                                            "Database Files (*.db *.sqlite)");
    db.setDatabaseName(fileName);

    //Save new DB in settings
    settings.setValue(databasePath, fileName);
}
