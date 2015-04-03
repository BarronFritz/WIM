#ifndef ADDKITITEMDIALOG_H
#define ADDKITITEMDIALOG_H

#include <QDialog>
#include <QtSql>
#include "structures.h"

namespace Ui {
class AddKitItemDialog;
}

class AddKitItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddKitItemDialog(QWidget *parent = 0);
    ~AddKitItemDialog();

    void setDatabase(QSqlDatabase &dbConnection);

    void setID(int ID) {kitID = ID;}

private slots:
    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_OK_clicked();

    void on_lineEdit_amount_textChanged(const QString &arg1);

private:
    Ui::AddKitItemDialog *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;

    int kitID;

    Product item;
};

#endif // ADDKITITEMDIALOG_H
