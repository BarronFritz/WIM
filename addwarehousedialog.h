#ifndef ADDWAREHOUSEDIALOG_H
#define ADDWAREHOUSEDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include "structures.h"
#include "database.h"

namespace Ui {
class AddWarehouseDialog;
}

class AddWarehouseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddWarehouseDialog(QWidget *parent, QSqlDatabase &db, Warehouse warehouse = Warehouse());
    ~AddWarehouseDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::AddWarehouseDialog *ui;
    QSqlDatabase db;
    Warehouse warehouse;
};

#endif // ADDWAREHOUSEDIALOG_H
