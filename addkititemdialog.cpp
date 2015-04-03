#include "addkititemdialog.h"
#include "ui_addkititemdialog.h"

AddKitItemDialog::AddKitItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddKitItemDialog)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);

    kitID = -1;

    QRegExp rxAmount("\\d+");
    ui->lineEdit_amount->setValidator(new QRegExpValidator(rxAmount, this));
    ui->lineEdit_amount->setText("1");
    item.quantity = 1;
}

AddKitItemDialog::~AddKitItemDialog()
{
    delete ui;
}

void AddKitItemDialog::setDatabase(QSqlDatabase &dbConnection)
{
    db = dbConnection;

    if(!db.open())
        qDebug() << db.lastError().text();
    else
    {
        QSqlQuery query;
        if(!query.exec("SELECT sku,description,id FROM item ORDER BY sku"))
            qDebug() << query.lastError().text();
        else
        {
            model->setQuery(query);

            ui->tableView->setModel(model);
            ui->tableView->show();
            ui->tableView->setColumnHidden(2,true);
            ui->tableView->resizeColumnsToContents();
        }

        db.close();
    }
}

void AddKitItemDialog::on_tableView_clicked(const QModelIndex &index)
{
    item.kit.sku            = index.sibling(index.row(), 0).data().toString();
    item.kit.description    = index.sibling(index.row(), 1).data().toString();
    item.warehouse.id       = index.sibling(index.row(), 2).data().toInt();

    ui->lineEdit_sku->setText(item.kit.sku);
    ui->lineEdit_description->setText(item.kit.description);

    ui->lineEdit_amount->setFocus();
    ui->lineEdit_amount->selectAll();
}

void AddKitItemDialog::on_pushButton_OK_clicked()
{
    if(kitID == -1)
        reject();

    if(item.kit.sku.isEmpty() || item.kit.description.isEmpty())
        qDebug() << "item empty";

    if(!db.open())
        qDebug() << db.lastError().text();
    else
    {
        QSqlQuery query;
        qDebug() << "INSERT INTO kit(sku,description,item_id,amount) SELECT kit.sku, kit.description, (SELECT id FROM item WHERE sku=\"" +
                    item.kit.sku + "\")," +
                    QString::number(item.quantity) + " FROM kit WHERE id=" + QString::number(kitID) + " LIMIT 1";
        if(!query.exec("INSERT INTO kit(sku,description,item_id,amount) SELECT kit.sku, kit.description, (SELECT id FROM item WHERE sku=\"" +
                       item.kit.sku + "\")," +
                       QString::number(item.quantity) + " FROM kit WHERE id=" + QString::number(kitID) + " LIMIT 1"))
            qDebug() << query.lastError().text();
        else
        {
            db.close();
            accept();
        }
    }
}

void AddKitItemDialog::on_lineEdit_amount_textChanged(const QString &arg1)
{
    item.quantity = arg1.toInt();
}
