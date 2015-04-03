#include "addwarehousedialog.h"
#include "ui_addwarehousedialog.h"

AddWarehouseDialog::AddWarehouseDialog(QWidget *parent, QSqlDatabase &dbConnection, Warehouse warehouse) :
    QDialog(parent),
    ui(new Ui::AddWarehouseDialog),
    db(dbConnection)
{
    ui->setupUi(this);
    ui->lineEdit_code->setText(warehouse.code);
    ui->lineEdit_name->setText(warehouse.name);
    ui->lineEdit_address_1->setText(warehouse.address1);
    ui->lineEdit_address_2->setText(warehouse.address2);
    ui->lineEdit_city->setText(warehouse.city);
    ui->lineEdit_state->setText(warehouse.state);
    ui->lineEdit_zip->setText(warehouse.zip);
    ui->lineEdit_country->setText(warehouse.country);
}

AddWarehouseDialog::~AddWarehouseDialog()
{
    delete ui;
}

void AddWarehouseDialog::on_buttonBox_accepted()
{
    warehouse.code = ui->lineEdit_code->text();
    warehouse.name = ui->lineEdit_name->text();
    warehouse.address1 = ui->lineEdit_address_1->text();
    warehouse.address2 = ui->lineEdit_address_2->text();
    warehouse.city = ui->lineEdit_city->text();
    warehouse.state = ui->lineEdit_state->text();
    warehouse.zip = ui->lineEdit_zip->text();
    warehouse.country = ui->lineEdit_country->text();

    if(addWarehouse(db, warehouse)) {
        this->accept();
    }
}

void AddWarehouseDialog::on_buttonBox_rejected()
{
    this->reject();
}
