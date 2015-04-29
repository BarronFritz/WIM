#include "addlotdialog.h"
#include "ui_addlotdialog.h"
#include <QMessageBox>

AddLotDialog::AddLotDialog(QWidget *parent, QSqlDatabase &db) :
    QDialog(parent),
    ui(new Ui::AddLotDialog),
    db(db)
{
    ui->setupUi(this);
}

AddLotDialog::~AddLotDialog()
{
    delete ui;
}

void AddLotDialog::on_buttonBox_accepted()
{
    lot.code = ui->lineEdit_code->text();
    lot.description = ui->lineEdit_description->text();
    lot.production = string_to_date(ui->lineEdit_production->text());
    lot.expiration = string_to_date(ui->lineEdit_expiration->text());

    if (addLot(db, lot)) {
        this->accept();
    } else {
        QMessageBox::warning(this, "Error",
                             "Could not add this lot to the database.\n"
                             "Make sure this lot code does not already exist.");
    }
}

void AddLotDialog::on_buttonBox_rejected()
{
    this->reject();
}
