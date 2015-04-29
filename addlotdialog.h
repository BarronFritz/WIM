#ifndef ADDLOTDIALOG_H
#define ADDLOTDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include "structures.h"
#include "database.h"
#include "utility.h"


namespace Ui {
class AddLotDialog;
}

class AddLotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLotDialog(QWidget *parent, QSqlDatabase &db);
    ~AddLotDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::AddLotDialog *ui;
    QSqlDatabase db;
    Lot lot;
};

#endif // ADDLOTDIALOG_H
