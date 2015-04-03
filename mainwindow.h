#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QTextStream>
#include <QThread>

#include "adddialog.h"
#include "updatedialog.h"
#include "movedialog.h"
#include "database.h"
#include "utility.h"

#include "logindialog.h"
#include "structures.h"
#include "admindialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlDatabase &db, sLogin& mLogin, QWidget *parent = 0);
    ~MainWindow();

public slots:

    void closeEvent(QCloseEvent *event);
private slots:

    // TABLE VIEW //
    void on_tableView_warehouse_pressed(const QModelIndex &index);
    void on_tableView_warehouse_doubleClicked(const QModelIndex &index);

    // SEARCH //
    // Warehouse
    void on_lineEdit_search_textChanged(const QString &arg1);
    void on_comboBox_search_mode_currentTextChanged(const QString &arg1);
    // Inventory
    void on_lineEdit_textEdited(const QString &arg1);

    // ACTIONS - MENU ITEMS & RIGHT CLICK & BUTTONS //
    // File
    void on_actionOpen_Database_triggered();
    // Edit
    void on_actionAdd_Item_triggered();
    void on_actionRemove_Item_triggered(); // Remove Item
    void on_actionChange_Item_triggered();
    void on_actionMove_Item_triggered();
    // Item/Kit


    // User
    void on_actionLog_In_triggered();
    void on_actionLog_Out_triggered();
    // About
    void on_actionAbout_triggered();
    // Admin
    void on_actionAdmin_triggered();

    void on_lineEdit_search_returnPressed();

    void on_actionCompletion_Popup_toggled(bool arg1);

private:
    Ui::MainWindow *ui;

    bool completer;
    void setCompleter(bool active);

    sLogin login;

    void updateSecurity(sLogin &cred);
    void freeDeadChildren();

    /* FILE IO BITS */
    bool loadPreferences();
    bool loadSearchMode();
    bool loadDatabasePath();

    bool savePreferences();
    bool saveSearchMode();
    bool saveDatabasePath();

    bool openNewDatabase();

    /* SQL OBJECTS */
    QSqlDatabase db;
    QSqlQueryModel *imodel;
    QSqlQueryModel *wmodel;
    QSqlQueryModel *wSearchModel;

    QString databasePath;
    QString searchMode;

    /* SQL METHODS */
    void updateWHSEView(QString searchString = "");
    void updateINVView(QString searchString = "");

    AdminDialog *adminDialog;

    /* NORMAL OBJECTS */
    Product product;
};

#endif // MAINWINDOW_H
