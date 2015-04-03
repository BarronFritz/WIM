#include "addkitdialog.h"
#include "ui_addkitdialog.h"

AddKitDialog::AddKitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddKitDialog)
{
    ui->setupUi(this);
}

AddKitDialog::~AddKitDialog()
{
    delete ui;
}

void AddKitDialog::on_pushButton_ok_clicked()
{
    QString sku = ui->lineEdit_sku->text();
    QString description = ui->lineEdit_description->text();

    if(sku.isEmpty() || description.isEmpty())
    {

        QMessageBox::about(this, "Form Not Filled", "Fill Form or Cancel.");
        return;
    }

    if(!db.open())
        qDebug() << db.lastError().text();
    else
    {
        QSqlQuery query;
        if(!query.exec("INSERT INTO kit(sku,description) VALUES(\"" +
                       sku + "\", \"" + description + "\")"))
            qDebug() << query.lastError().text();
        db.close();
    }
    this->accept();
}

void AddKitDialog::on_lineEdit_sku_returnPressed()
{
    if(ui->lineEdit_sku->text().isEmpty())
        return;
    ui->lineEdit_description->setFocus();
    ui->lineEdit_description->selectAll();
}

void AddKitDialog::on_lineEdit_description_returnPressed()
{
    if(ui->lineEdit_description->text().isEmpty())
        return;
    if(!ui->lineEdit_sku->text().isEmpty())
        ui->pushButton_ok->click();
}
