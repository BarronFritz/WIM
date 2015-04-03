#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QCompleter>
#include "database.h"
#include "structures.h"
#include "utility.h"

namespace Ui {
class MoveDialog;
}

class MoveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent = 0);
    ~MoveDialog();

private slots:
    void on_lineEdit_new_slot_textChanged(const QString &arg1);

    void on_pushButton_lookup_clicked();

    void on_pushButton_cancel_clicked();
    void on_pushButton_ok_clicked();

    void on_lineEdit_new_slot_editingFinished();

private:
    Ui::MoveDialog *ui;
    QSqlDatabase db;
    sLogin login;
    QSqlQueryModel *slotModel;

    void setFromItem(Product &item);

    Product fromProduct;
    Product toProduct;
};

#endif // MOVEDIALOG_H
