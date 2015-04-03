#include "movedialog.h"
#include "ui_movedialog.h"

MoveDialog::MoveDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveDialog),
    db(dbConnection),
    login(login)
{
    ui->setupUi(this);

    /// SLOT
    // ui->lineEdit_new_slot->setValidator(new QRegExpValidator(regex::slot(), this));
    ui->lineEdit_new_slot->setText("");

    slotModel = new QSqlQueryModel(this);

    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT slot FROM bin ORDER BY slot")) {
            if (query.exec()) {
                slotModel->setQuery(query);
                QCompleter *slotCompleter = new QCompleter(slotModel, this);
                slotCompleter->setCompletionMode(QCompleter::PopupCompletion);
                ui->lineEdit_new_slot->setCompleter(slotCompleter);
            } else {
                // Query didn't execute
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            // Query didn't prepapre
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
    } else {
        // DB didn't open
        qDebug() << db.lastError().text();
    }

    setFromItem(product);
}

MoveDialog::~MoveDialog()
{
    delete ui;
}

void MoveDialog::setFromItem(Product &item)
{
    fromProduct = item;
    toProduct = fromProduct;
    toProduct.bin = Bin();

    ui->label_warehouse_id->setText(QString::number(item.warehouse.id));
    ui->label_slot_id->setText(item.bin.slot);
    ui->label_sku_id->setText(item.kit.sku);
    ui->label_description_id->setText(item.kit.description);
}

void MoveDialog::on_lineEdit_new_slot_editingFinished()
{
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT id FROM bin WHERE slot LIKE :slot "
                          "AND warehouse_id = :whseID")) {
            query.bindValue(":slot", toProduct.bin.slot);
            query.bindValue(":whseID", fromProduct.warehouse.id);
            if (query.exec()) {
                if (query.first()) {
                    if (query.value(0).toInt() > 0) {
                        toProduct.bin.slot = ui->lineEdit_new_slot->text();
                        toProduct.bin.id   = query.value(0).toInt();
                    } else {
                        // Returned value has an invalid ID#
                        // This should have never happened
                        // arbitrary ERROR_CODE: 4000
                        qDebug() << "Returned value has an invalid ID. ";
                        QMessageBox::warning(this, "Invalid ID!",
                                             "The value returned by this query "
                                             "was invalid.\nERROR_CODE: 4000\n"
                                             "Please Tell your system Administrator",
                                             QMessageBox::Ok);
                    }
                } else {
                    // Cannot fetch first result
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                    QMessageBox::warning(this, "Bin Doesn't Exist!",
                                         "Bin doesn't exists in the "
                                         "selected warehouse.",
                                         QMessageBox::Ok);
                }
            } else {
                // Query cannot execute
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            // Query coudln't prepare
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        // DB couldn't open.
        qDebug() << db.lastError().text();
    }
}

void MoveDialog::on_lineEdit_new_slot_textChanged(const QString &arg1)
{
    ui->lineEdit_new_slot->setText(arg1.toUpper());
    toProduct.bin.slot = arg1.toUpper();
}

void MoveDialog::on_pushButton_lookup_clicked()
{
    qDebug() << "TODO: Implement Location Lookup;";
    QMessageBox::warning(this, "Not Implemented", "Location Lookup is not yet Implemented.");

}

void MoveDialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

void MoveDialog::on_pushButton_ok_clicked()
{
    if(fromProduct.bin.id == toProduct.bin.id)
    {
        QMessageBox::warning(this,"Invalid Move", "Cannot move to same Location!", QMessageBox::Ok);
        return;
    }

    if(fromProduct.removable() && toProduct.addable())
    {
        if(moveProduct(db, fromProduct, toProduct, login))
            this->accept();
        else
        {
            QMessageBox::warning(this,"Failure", "Error moving product. Please contact System Administrator.", QMessageBox::Ok);
            this->reject();
        }
    }
    else
    {
        QMessageBox::warning(this, "Incomplete Form", "Fill form completely, or cancel", QMessageBox::Ok);
    }
}
