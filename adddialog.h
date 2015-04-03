#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QCompleter>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include "database.h"
#include "utility.h"
#include "Lepton.h"
//#include "structures.h"

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent = 0);
    ~AddDialog();

private slots:

    // SLOT //
    void on_comboBox_slot_warehouse_currentIndexChanged(int index);
    void on_lineEdit_slot_textEdited(const QString &arg1);
    void on_lineEdit_slot_editingFinished();
    // PRODUCT //
    void on_lineEdit_product_sku_textEdited(const QString &arg1);
    void on_lineEdit_product_sku_returnPressed();
    void on_pushButton_product_lookup_clicked();
    // DETAILS //
    // Dates
    void on_lineEdit_details_date_receiving_editingFinished();
    void on_lineEdit_details_date_packed_editingFinished();
    void on_lineEdit_details_date_expiration_editingFinished();
    // Amount
    void on_lineEdit_details_quantity_returnPressed();
    void on_lineEdit_details_quantity_textChanged(const QString &arg1);
    // Notes
    void on_lineEdit_details_notes_textChanged(const QString &arg1);
    // ACCEPT //
    void on_pushButton_ok_clicked();

    void on_lineEdit_details_quantity_editingFinished();

private:
    Ui::AddDialog *ui;
    QSqlDatabase db;
    sLogin login;
    QSqlQueryModel *warehouseModel;
    QSqlQueryModel *slotModel;
    QSqlQueryModel *skuModel;

    Product product;
    QList<Bin> bins;

    bool warehouseValid();
    bool binValid();
    bool kitValid();
    bool isComplete();

    void debugOutput() {
        qDebug() << "Complete: " << (isComplete() ? "True" : "False");
        qDebug() << "Warehouse ID: " << product.warehouse.id;
        qDebug() << "Bin ID: " << product.bin.id;
        qDebug() << "Kit SKU: " << product.kit.sku;
        qDebug() << "Quantity: " << product.quantity;
        qDebug() << "Received: " << product.received.toString(Qt::ISODate);
        qDebug() << "Packed: " << product.packed.toString(Qt::ISODate);
        qDebug() << "Expiration: " << product.expiration.toString(Qt::ISODate);
        qDebug() << "Notes: " << product.notes;
    }

    void setItem(Product &product);
};

#endif // ADDDIALOG_H
