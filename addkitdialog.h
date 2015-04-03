#ifndef ADDKITDIALOG_H
#define ADDKITDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddKitDialog;
}

class AddKitDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddKitDialog(QWidget *parent = 0);
    ~AddKitDialog();

    void setDatabase(QSqlDatabase &dbConnection) { db = dbConnection; }

private slots:
    void on_pushButton_ok_clicked();

    void on_lineEdit_sku_returnPressed();

    void on_lineEdit_description_returnPressed();

private:
    Ui::AddKitDialog *ui;
    QSqlDatabase db;
};

#endif // ADDKITDIALOG_H
