#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QCompleter>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QInputDialog>
#include "structures.h"
#include "database.h"
#include "utility.h"
#include "addwarehousedialog.h"

namespace Ui {
class AdminDialog;
}

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(QSqlDatabase &db, sLogin login, QWidget *parent = 0);
    ~AdminDialog();

private slots:
    void on_tableView_log_clicked(const QModelIndex &index);
    void on_pushButton_revert_clicked();

    void on_lineEdit_slot_slot_textEdited(const QString &arg1);
    void on_tableView_slot_clicked(const QModelIndex &index);
    void on_pushButton_slot_add_clicked();
    void on_pushButton_slot_remove_clicked();

    void on_lineEdit_product_kit_search_textEdited(const QString &arg1);
    void on_pushButton_product_kit_add_clicked();
    void on_pushButton_product_kit_remove_clicked();
    void on_tableView_product_kits_clicked(const QModelIndex &index);
    void on_pushButton_product_kit_edit_clicked();

    void on_tableView_product_items_in_selected_kit_clicked(const QModelIndex &index);

    void on_lineEdit_product_item_search_textEdited(const QString &arg1);
    void on_pushButton_product_item_add_clicked();
    void on_pushButton_product_item_remove_clicked();
    void on_tableView_product_items_clicked(const QModelIndex &index);
    void on_pushButton_product_item_edit_clicked();

    void on_pushButton_product_add_to_kit_clicked();
    void on_pushButton_product_remove_from_kit_clicked();

    void on_pushButton_query_exec_clicked();

    void on_tableView_users_clicked(const QModelIndex &index);
    void on_pushButton_login_reset_clicked();

    void on_comboBox_slot_warehouse_currentIndexChanged(int index);
    void on_pushButton_warehouse_add_clicked();
    void on_pushButton_warehouse_remove_clicked();

    void on_comboBox_slot_warehouse_activated(int index);

    void on_tableView_slot_doubleClicked(const QModelIndex &index);

private:
    Ui::AdminDialog *ui;
    QSqlDatabase db;
    sLogin login;
    QSqlTableModel *loginModel;
    QSqlQueryModel *logModel, *slotModel, *kitModel;
    QSqlQueryModel *iikModel, *itemModel, *warehouseModel;

    struct ProductLog{
        int id;
        QString type;
        QDateTime changed;
        Product product;
    } selectedLog;

    Bin selectedBin;
    Kit selectedKit;
    Kit selectedItemInKit;
    Item selectedItem;
    Warehouse selectedWarehouse;

    void tryRemoveSlot(QString slot, Warehouse warehouse);
    void refreshItemsInKit();
    void refreshSlots();
    void deleteLogEntry(ProductLog entry);
};

#endif // ADMINDIALOG_H
