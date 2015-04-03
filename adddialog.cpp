#include "adddialog.h"
#include "ui_adddialog.h"
#include "itemselectdialog.h"

AddDialog::AddDialog(QSqlDatabase &dbConnection, Product &product, sLogin login, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog),
    db(dbConnection),
    login(login)
{
    ui->setupUi(this);

    /// Allocate new QueryModel
    warehouseModel = new QSqlQueryModel(this);
    slotModel = new QSqlQueryModel(this);
    skuModel = new QSqlQueryModel(this);

    if (db.open()) {
        warehouseModel->setQuery("SELECT id,code FROM warehouse");
        ui->comboBox_slot_warehouse->setModel(warehouseModel);
        ui->comboBox_slot_warehouse->setModelColumn(1);
        ui->comboBox_slot_warehouse->setCurrentIndex(0);
        ui->comboBox_slot_warehouse->setInsertPolicy(QComboBox::NoInsert);

        slotModel->setQuery("SELECT slot FROM bin");
        QCompleter *slotCompleter = new QCompleter(slotModel, this);
        slotCompleter->setCompletionMode(QCompleter::PopupCompletion);
        ui->lineEdit_slot->setCompleter(slotCompleter);

        skuModel->setQuery("SELECT sku FROM kit GROUP BY sku");
        QCompleter *skuCompleter = new QCompleter(skuModel, this);
        skuCompleter->setCompletionMode(QCompleter::PopupCompletion);
        ui->lineEdit_product_sku->setCompleter(skuCompleter);

        db.close();
    } else { qDebug() << db.lastError().text(); }

    /// SLOT
    // ui->lineEdit_slot->setValidator(new QRegExpValidator(regex::slot(), this));
    ui->lineEdit_slot->setText("");

    /// PRODUCT
    ui->lineEdit_product_sku->setText("");
    ui->lineEdit_product_description->setText("");

    /// DETAILS
    ui->lineEdit_details_date_receiving->setValidator(
                new QRegExpValidator(regex::date(), this));
    ui->lineEdit_details_date_expiration->setValidator(
                new QRegExpValidator(regex::date(), this));

    ui->lineEdit_details_quantity->setValidator(
                new QRegExpValidator(regex::quantity(), this));
    ui->lineEdit_details_quantity->setText("0");

    ui->lineEdit_details_notes->setText("");

    /// OTHER
    ui->label_username->setText(login.usernameFull());

    setItem(product);
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::setItem(Product &product) {
    // Assign fields values.
    // Don't set the slot.
    if(product.warehouse.id <= 0)
        ui->comboBox_slot_warehouse->setCurrentIndex(0);
    else
        ui->comboBox_slot_warehouse->setCurrentIndex(product.warehouse.id - 1);

    // If sku is not empty or null, load the description from the database.
    if (!product.kit.sku.isEmpty()) {
        ui->lineEdit_product_sku->setText(product.kit.sku);
        this->product.kit.sku = product.kit.sku;
        if (!db.open()) {
            qDebug() << db.lastError();
        } else {
            QSqlQuery query;
            if (!query.prepare("SELECT description FROM kit WHERE sku = :sku")) {
                qDebug() << query.lastError();
            } else {
                query.bindValue(":sku", product.kit.sku);
                if (!query.exec()) {
                    qDebug() << query.lastError();
                    qDebug() << query.executedQuery();
                } else {
                    query.first();
                    this->product.kit.description = query.value(0).toString();
                    ui->lineEdit_product_description->setText(
                                this->product.kit.description);
                }
            }
            db.close();
        }
    }

    if (product.received.isValid()) {
        this->product.received = product.received;
        ui->lineEdit_details_date_receiving->setText(
                    product.received.toString(Qt::ISODate));
    }

    if (product.packed.isValid()) {
        this->product.packed = product.packed;
        ui->lineEdit_details_date_packed->setText(
                    product.packed.toString(Qt::ISODate));
    }

    if (product.expiration.isValid()) {
        this->product.expiration = product.expiration;
        ui->lineEdit_details_date_expiration->setText(
                    product.expiration.toString(Qt::ISODate));
    }

    if (product.modified.isValid()) {
        this->product.modified = product.modified;
    }

    ui->lineEdit_details_quantity->setText(QString::number(product.quantity));
    this->product.quantity = product.quantity;

    ui->lineEdit_details_notes->setText(product.notes);
    this->product.notes = product.notes;
}

void AddDialog::on_comboBox_slot_warehouse_currentIndexChanged(int index)
{
    product.warehouse.id =
            ui->comboBox_slot_warehouse->model()->index(index,0).data().toInt();
    product.warehouse.code =
            ui->comboBox_slot_warehouse->model()->index(index,1).data().toString();
}

void AddDialog::on_lineEdit_slot_textEdited(const QString &arg1)
{
    QString arg = arg1.toUpper();
    ui->lineEdit_slot->setText(arg);
    arg = arg.simplified();
    QStringList tmp = arg.split(',');
    bins.clear();
    for (int index = 0; index < tmp.size(); index++) {
        Bin b;
        b.slot = tmp[index];
        bins.push_back(b);
    }
}

void AddDialog::on_lineEdit_slot_editingFinished()
{
    for (auto itr = std::begin(bins); itr != std::end(bins); ++itr) {
        qDebug() << "Exists?: " << itr->slot;
        QString input = itr->slot;

        if (db.open()) {
            QSqlQuery query;
            if (query.prepare("SELECT id FROM bin WHERE slot = :slot "
                              "AND warehouse_id = :whseID")) {
                query.bindValue(":slot", input);
                query.bindValue(":whseID", product.warehouse.id);
                if (query.exec()) {
                    query.first();
                    if (query.value(0).toInt() > 0) {
                        product.bin.slot = input;
                        itr->id = query.value(0).toInt();
                        product.bin.id = itr->id;
                        qDebug() << "  Slot: " << input;
                    } else {
                        QMessageBox addSlotDialog(this);
                        addSlotDialog.setWindowTitle("Slot Doesn't Exist");
                        addSlotDialog.setText("The Slot " + input + " doesn't exist "
                                        "in the selected warehouse.\n"
                                        "Would you like to add it?");
                        addSlotDialog.setInformativeText("Warehouse: " + product.warehouse.code
                                                   + "\nSlot: " + input);
                        addSlotDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        addSlotDialog.setButtonText(QMessageBox::Yes, "Add Slot");
                        addSlotDialog.setButtonText(QMessageBox::No, "Cancel");
                        if (addSlotDialog.exec() == QMessageBox::Yes) {
                            if (addSlot(db, input, product.warehouse)) {
                                on_lineEdit_slot_editingFinished();
                            }
                        }
                    }
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
            db.close();
        } else {
            // DB didn't open
            qDebug() << db.lastError().text();
        }
    }
}

void AddDialog::on_lineEdit_product_sku_textEdited(const QString &arg1)
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

void AddDialog::on_lineEdit_product_sku_returnPressed()
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

                product.kit.id = query.value("id").toInt();
                product.kit.sku = query.value("sku").toString();
                product.kit.description = query.value("description").toString();

                ui->lineEdit_details_date_receiving->setFocus();
                ui->lineEdit_details_date_receiving->selectAll();
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

void AddDialog::on_pushButton_product_lookup_clicked()
{
    Kit item;
    ItemSelectDialog dialog(db, &item, this);

    if(dialog.exec() == QDialog::Accepted)
    {
        ui->lineEdit_product_sku->setText(item.sku);
        ui->lineEdit_product_description->setText(item.description);
    }

    product.kit.sku = item.sku;
}

void AddDialog::on_lineEdit_details_date_receiving_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_details_date_receiving->text());
    ui->lineEdit_details_date_receiving->setText(d.toString(Qt::ISODate));
    product.received = d;

    // Next Input
//    ui->lineEdit_details_date_packed->setFocus();
//    ui->lineEdit_details_date_packed->selectAll();
}

void AddDialog::on_lineEdit_details_date_packed_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_details_date_packed->text());
    ui->lineEdit_details_date_packed->setText(d.toString(Qt::ISODate));
    product.packed = d;

    // Next Input
//    ui->lineEdit_details_date_expiration->setFocus();
//    ui->lineEdit_details_date_expiration->selectAll();
}

void AddDialog::on_lineEdit_details_date_expiration_editingFinished()
{
    QDate d = string_to_date(ui->lineEdit_details_date_expiration->text());
    ui->lineEdit_details_date_expiration->setText(d.toString(Qt::ISODate));
    product.expiration = d;

    // Next Input
//    ui->lineEdit_details_lot->setFocus();
//    ui->lineEdit_details_lot->selectAll();
}

void AddDialog::on_lineEdit_details_quantity_returnPressed()
{
    QString expText = ui->lineEdit_details_quantity->text();
    double expression = Lepton::Parser::parse(expText.toStdString()).evaluate();
    ui->lineEdit_details_quantity->setText(QString::number(expression));
}

void AddDialog::on_pushButton_ok_clicked()
{
    if (!isComplete())
    {
        QString message;
        qDebug() << product.toString();
        debugOutput();
        QMessageBox::warning(this, "Incomplete Form", message
                             + "Fill form completely, or cancel",
                             QMessageBox::Ok);
        return;
    } else {
        if (product.kit.sku.isEmpty() && product.notes.isEmpty());
    }

    bool yesToAll{false};
    bool noToAll{false};
    for (Bin &bin : bins) {
        product.bin = bin;
        if (db.open()) {
            QSqlQuery query;
            // Check if the product already exists in this slot
            if(yesToAll) {
                if (addProduct(db, product, login)) {
                    qDebug() << "Adding new product regarless of current slot contents";
                }
            } else if (noToAll) {
                if (removeFromSlot(db, product)) {
                    if (addProduct(db, product, login)) {
                        qDebug() << "Deleting if existing product & Adding New Product";
                    }
                }
            } else if (query.prepare("SELECT kit_sku, kit.description FROM "
                                     "product JOIN kit ON kit_sku=kit.sku "
                                     "WHERE warehouse_id = :whse "
                                     "AND bin_id = :bin ")) {
                query.bindValue(":whse", product.warehouse.id);
                query.bindValue(":bin", product.bin.id);

                if(query.exec()) {
                    if(query.first()) {
                        QMessageBox question;
                        question.setWindowTitle("Slot Not Empty");
                        question.setText("Product already exists in slot: "
                                         + product.bin.slot);
                        question.setInformativeText("The Slot currently contains:\n"
                                                    + query.value(0).toString()
                                                    + " : "
                                                    + query.value(1).toString() +
                                                    "\n\nDo you want to keep "
                                                    "the existing product?");
                        question.setStandardButtons(QMessageBox::Cancel |
                                                    QMessageBox::Yes |
                                                    QMessageBox::YesToAll |
                                                    QMessageBox::No |
                                                    QMessageBox::NoToAll);
                        question.setDefaultButton(QMessageBox::Yes);
                        question.setButtonText(QMessageBox::Yes, "Keep");
                        question.setButtonText(QMessageBox::YesToAll, "Keep For All");
                        question.setButtonText(QMessageBox::No, "Remove");
                        question.setButtonText(QMessageBox::NoToAll, "Remove For All");
                        auto result = question.exec();
                        if (result == QMessageBox::Cancel) {
                            return;
                        }

                        if (result == QMessageBox::No) {
                            if (removeFromSlot(db, product)) {
                                if (addProduct(db, product, login)) {
                                    qDebug() << "Removing existing product";
                                    qDebug() << "Adding new product";
                                }
                            }
                        }
                        if (result == QMessageBox::NoToAll) {
                            noToAll = true;
                            if (removeFromSlot(db, product)) {
                                if (addProduct(db, product, login)) {
                                    qDebug() << "Removing existing product";
                                    qDebug() << "Adding new product";
                                }
                            }
                        }

                        if (result == QMessageBox::Yes) {
                            if (addProduct(db, product, login)) {
                                qDebug() << "Product Exists, Adding anyway";
                            }
                        }
                        if (result == QMessageBox::YesToAll) {
                            yesToAll = true;
                            if (addProduct(db, product, login)) {
                                qDebug() << "Adding all regarless of "
                                            "current slot contents...";
                            }
                        }
                    } else {
                        // Product does not exist in this slot, close db and accept.
                        if(addProduct(db, product, login)) {
                            qDebug() << "Adding Product";
                        }
                    }
                } else {
                    // Execution Failed
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            } else {
                qDebug() << "Prepare Failed";
                qDebug() << query.lastError();
            }
        db.close();
        } else {
            qDebug() << db.lastError();
        }
    }
    this->accept();
}

void AddDialog::on_lineEdit_details_quantity_textChanged(const QString &arg1)
{
    product.quantity = arg1.toInt();
}

void AddDialog::on_lineEdit_details_notes_textChanged(const QString &arg1)
{
    product.notes = arg1;
}


bool AddDialog::warehouseValid()
{
    if(product.warehouse.id < 1)
        return false;
    bool result{false};
    if(!db.open())
    {
        qDebug() << db.lastError().text();
    }
    else
    {
        QSqlQuery query;
        if(!query.exec("SELECT * FROM warehouse WHERE id="
                      + QString::number(product.warehouse.id)))
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            if(query.first())
                result = true;
        }
        db.close();
    }
    return result;
}

bool AddDialog::binValid()
{
    if(product.bin.id < 1)
        return false;
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT * FROM bin WHERE id = :binID")) {
            query.bindValue(":binID", product.bin.id);
            if (query.exec()) {
                if (query.first()) {
                    result = true;
                } else {
                    // Query yeileded zero results.
                    // No product in current bin
                }
            } else {
                // Query coudln't execute.
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            // Query couldn't prepare.
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        // DB coudln't open.
        qDebug() << db.lastError().text();
    }
    return result;
}

bool AddDialog::kitValid()
{
    if(product.kit.sku.isEmpty())
        return false;
    bool result{false};
    if(!db.open())
    {
        qDebug() << db.lastError().text();
    }
    else
    {
        QSqlQuery query;
        if(!query.exec("SELECT * FROM kit WHERE sku='"
                      + product.kit.sku + "'"))
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            if(query.first())
                result = true;
        }
        db.close();
    }
    return result;
}

bool AddDialog::isComplete() {
    if(warehouseValid()) {
        if(binValid()) {
            if(product.addable()) {
                if (product.kit.sku.isEmpty()
                    && !product.notes.isEmpty()) {
                    if (QMessageBox::question(this, "No Product!",
                            "There is not a valid product entered.\n "
                            "Would you like to add to the slot anyway?",
                            QMessageBox::No, QMessageBox::Yes)
                            == QMessageBox::Yes) {
                        return true;
                    } else {
                        qDebug() << "Rejected to add to slot";
                        return false;
                    }
                } else if (!product.kit.sku.isEmpty()) {
                    return true;
                } else {
                    qDebug() << "Sku is empty";
                    return false;
                }
            } else {
                qDebug() << "Product not addable";
                return false;
            }
        } else {
            qDebug() << "Bin Invalid";
            return false;
        }
    } else {
        qDebug() << "WarehouseInvalid";
        return false;
    }
}

void AddDialog::on_lineEdit_details_quantity_editingFinished()
{
    QString expText = ui->lineEdit_details_quantity->text();
    double expression = Lepton::Parser::parse(expText.toStdString()).evaluate();
    ui->lineEdit_details_quantity->setText(QString::number(expression));
}
