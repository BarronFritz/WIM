#include "updatedialog.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog),
    db(dbConnection),
    login(login)
{
    ui->setupUi(this);

    /// Allocate new QueryModel
    warehouseModel = new QSqlQueryModel(this);
    skuModel = new QSqlQueryModel(this);
    if(!db.open())
    {
        qDebug() << db.lastError().text();
    }
    else
    {
        QSqlQuery query;
        if(!query.exec("SELECT id,code FROM warehouse"))
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            warehouseModel->setQuery(query);
            ui->comboBox_warehouse->setModel(warehouseModel);
            ui->comboBox_warehouse->setModelColumn(1);
        }

        if(!query.exec("SELECT sku FROM kit GROUP BY sku"))
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            skuModel->setQuery(query);
            QCompleter *skuCompleter = new QCompleter(skuModel, this);
            skuCompleter->setCompletionMode(QCompleter::PopupCompletion);
            ui->lineEdit_product_sku->setCompleter(skuCompleter);
        }
        db.close();
    }

    ui->lineEdit_slot_slot->setText("");

    ui->lineEdit_quantity->setValidator(new QRegExpValidator(regex::quantity(), this));
    ui->lineEdit_quantity->setText("");

    ui->lineEdit_date_received->setValidator(new QRegExpValidator(regex::date(), this));
    ui->lineEdit_date_received->setText("");

    ui->lineEdit_date_packed->setValidator(new QRegExpValidator(regex::date(), this));
    ui->lineEdit_date_packed->setText("");

    ui->lineEdit_date_expiration->setValidator(new QRegExpValidator(regex::date(), this));
    ui->lineEdit_date_expiration->setText("");

    ui->lineEdit_product_description->setText("");
    ui->lineEdit_product_sku->setText("");

    setItem(product);
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::setItem(Product &Item) //private
{
    oldProduct = Item;
    newProduct = oldProduct;

    ui->lineEdit_slot_slot->setText(oldProduct.bin.slot);

    // If sku is not empty or null, load the description from the database.
    if(!oldProduct.kit.sku.isEmpty()) {
        ui->lineEdit_product_sku->setText(oldProduct.kit.sku);
        if(!db.open())
        {
            qDebug() << db.lastError();
        }
        else
        {
            QSqlQuery query;
            if(!query.exec("SELECT description FROM kit WHERE sku=\"" + oldProduct.kit.sku + "\""))
            {
                qDebug() << query.lastError();
            }
            else
            {
                query.first();
                ui->lineEdit_product_description->setText(query.value(0).toString());
            }
            db.close();
        }
    }

    if(oldProduct.received.isValid())
        ui->lineEdit_date_received->setText(oldProduct.received.toString(Qt::ISODate));
    if(oldProduct.packed.isValid())
        ui->lineEdit_date_packed->setText(oldProduct.packed.toString(Qt::ISODate));
    if(oldProduct.expiration.isValid())
        ui->lineEdit_date_expiration->setText(oldProduct.expiration.toString(Qt::ISODate));
    ui->lineEdit_quantity->setText(QString::number(oldProduct.quantity));
    ui->lineEdit_details_notes->setText(oldProduct.notes);
}

bool UpdateDialog::formFilled()
{
    bool ret = false;

    if(newProduct.warehouse.id == 0)
        newProduct.warehouse.id = oldProduct.warehouse.id;
    else
        ret = true;

    if(newProduct.kit.sku.isEmpty())
        newProduct.kit.sku = oldProduct.kit.sku;
    else
        ret = true;

    if(newProduct.kit.description.isEmpty())
        newProduct.kit.description = oldProduct.kit.description;
    else
        ret = true;

    if(!newProduct.packed.isValid())
        newProduct.packed = oldProduct.packed;
    else
    {
        ret = true;
    }

    if(ui->lineEdit_quantity->text().isEmpty())
        newProduct.quantity = oldProduct.quantity;
    else
        ret = true;

    return ret;
}

void UpdateDialog::on_comboBox_warehouse_currentIndexChanged(int index)
{
    newProduct.warehouse.id = ui->comboBox_warehouse->model()->index(index, 0).data().toInt();
}

void UpdateDialog::on_lineEdit_product_sku_textEdited(const QString &arg1)
{
    if(db.open())
    {
        QSqlQuery query;
        if(query.exec("SELECT sku, description FROM kit WHERE sku LIKE '%"
                      + arg1 + "%' ORDER BY sku"))
        {
            if(query.first())
            {
                ui->lineEdit_product_description->setText(
                            query.value("description").toString());
            }
        }
        else
        {
            qDebug() << query.lastError().text();
        }
        db.close();
    }
    else
    {
        qDebug() << db.lastError().text();
    }
}

void UpdateDialog::on_lineEdit_product_sku_returnPressed()
{
    if(db.open())
    {
        QSqlQuery query;
        if(query.exec("SELECT id, sku, description FROM kit WHERE sku LIKE '%"
                      + ui->lineEdit_product_sku->text() + "%' ORDER BY sku"))
        {
            if(query.first())
            {
                ui->lineEdit_product_description->setText(
                            query.value("description").toString());
                ui->lineEdit_product_sku->setText(
                            query.value("sku").toString());

                newProduct.kit.id = query.value("id").toInt();
                newProduct.kit.sku = query.value("sku").toString();
                newProduct.kit.description = query.value("description").toString();

                ui->lineEdit_date_received->setFocus();
                ui->lineEdit_date_received->selectAll();
            }
        }
        else
        {
            qDebug() << query.lastError().text();
        }
        db.close();
    }
    else
    {
        qDebug() << db.lastError().text();
    }
}

void UpdateDialog::on_pushButton_product_lookup_clicked()
{
    Kit item;
    ItemSelectDialog dialog(db, &item, this);

    if(dialog.exec() == QDialog::Accepted)
    {
        ui->lineEdit_product_sku->setText(item.sku);
        ui->lineEdit_product_description->setText(item.description);
    }

    newProduct.kit.sku = item.sku;
}

void UpdateDialog::on_lineEdit_date_received_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_date_received->text());
    ui->lineEdit_date_received->setText(d.toString(Qt::ISODate));
    newProduct.received = d;
}

void UpdateDialog::on_lineEdit_date_packed_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_date_packed->text());
    ui->lineEdit_date_packed->setText(d.toString(Qt::ISODate));
    newProduct.packed = d;
}

void UpdateDialog::on_lineEdit_date_expiration_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_date_expiration->text());
    ui->lineEdit_date_expiration->setText(d.toString(Qt::ISODate));
    newProduct.expiration = d;
}

void UpdateDialog::on_lineEdit_quantity_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        newProduct.quantity = oldProduct.quantity;
    else
        newProduct.quantity = arg1.toInt();
}

void UpdateDialog::on_lineEdit_details_notes_textEdited(const QString &arg1)
{
    newProduct.notes = arg1;
}

void UpdateDialog::on_pushButton_clear_clicked()
{
    setItem(oldProduct);
}

void UpdateDialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

void UpdateDialog::on_pushButton_ok_clicked()
{
    if(formFilled())
    {
        QString warehouse,slot,line_break,sku,desc,dateReceived,datePacked,dateExpires,quantity,notes;

        warehouse = oldProduct.warehouse.code;

        slot = oldProduct.bin.slot;

        line_break = "\n------------------";

        if(oldProduct.kit.sku != newProduct.kit.sku)
            sku = oldProduct.kit.sku + " --> " + newProduct.kit.sku;

        if(oldProduct.kit.description != newProduct.kit.description)
            desc = oldProduct.kit.description + " --> " + newProduct.kit.description;

        if(oldProduct.received != newProduct.received)
            dateReceived = oldProduct.received.toString(Qt::ISODate) + " --> " + newProduct.received.toString(Qt::ISODate);

        if(oldProduct.packed != newProduct.packed)
            datePacked = oldProduct.packed.toString(Qt::ISODate) + " --> " + newProduct.packed.toString(Qt::ISODate);

        if(oldProduct.expiration != newProduct.expiration)
            dateExpires = oldProduct.expiration.toString(Qt::ISODate) + " --> " + newProduct.expiration.toString(Qt::ISODate);

        if(oldProduct.quantity != newProduct.quantity)
            quantity = QString::number(oldProduct.quantity) + " --> " + QString::number(newProduct.quantity);

        if(oldProduct.notes != newProduct.notes)
            notes = oldProduct.notes + " --> " + newProduct.notes;

        QMessageBox mbox(this);
        mbox.setWindowTitle("Update Database?");
        mbox.setText("Warehouse: " + warehouse
                     + "\nSlot:" + slot
                     + line_break
                     + "\nSKU:" + sku
                     + "\nDescription:" + desc
                     + "\nDate Received:" + dateReceived
                     + "\nDate Packed:" + datePacked
                     + "\nDate Expires:" + dateExpires
                     + "\nAmount:" + quantity
                     + "\nNotes:" + notes
                     + line_break
                     + "\n\nOnly edited values are displayed!");
        mbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        mbox.setDefaultButton(QMessageBox::Ok);

        if(mbox.exec() == QMessageBox::Ok)
        {
            if(editProduct(db, oldProduct, newProduct, login))
                this->accept();
            else
                QMessageBox::warning(this, "Failed To Remove", "Product failed to remove from slot.\nPlease notify the system administrator.", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "Incomplete Form", "Fill form completely, or cancel", QMessageBox::Ok);
    }
}

void UpdateDialog::on_lineEdit_quantity_editingFinished()
{
    QString expText = ui->lineEdit_quantity->text();
    double expression = Lepton::Parser::parse(expText.toStdString()).evaluate();
    ui->lineEdit_quantity->setText(QString::number(expression));
}
