#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QSql>
#include <QCompleter>
#include <QMessageBox>
#include "database.h"
#include "utility.h"
#include "itemselectdialog.h"
#include "Lepton.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent = 0);
    ~UpdateDialog();

private slots:

    void on_comboBox_warehouse_currentIndexChanged(int index);

    void on_lineEdit_product_sku_textEdited(const QString &arg1);
    void on_lineEdit_product_sku_returnPressed();
    void on_pushButton_product_lookup_clicked();

    void on_lineEdit_date_received_editingFinished();
    void on_lineEdit_date_packed_editingFinished();
    void on_lineEdit_date_expiration_editingFinished();
    void on_lineEdit_quantity_textChanged(const QString &arg1);
    void on_lineEdit_details_notes_textEdited(const QString &arg1);

    void on_pushButton_clear_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_ok_clicked();

    void on_lineEdit_quantity_editingFinished();

private:
    Ui::UpdateDialog *ui;
    QSqlDatabase db;
    sLogin login;
    QSqlQueryModel *warehouseModel;
    QSqlQueryModel *skuModel;

    void setItem(Product &Item);
    bool formFilled();

    Product oldProduct;
    Product newProduct;
};

#endif // UPDATEDIALOG_H
