#include "admindialog.h"
#include "ui_admindialog.h"

AdminDialog::AdminDialog(QSqlDatabase &db, sLogin login, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminDialog),
    db(db),
    login(login)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("adminDialogGeometry").toByteArray());
//    restoreState(settings.value("adminDialogState").toByteArray());

    loginModel = new QSqlTableModel(this, db);
    logModel = new QSqlQueryModel(this);
    slotModel = new QSqlQueryModel(this);
    kitModel = new QSqlQueryModel(this);
    iikModel = new QSqlQueryModel(this);
    itemModel = new QSqlQueryModel(this);
    warehouseModel = new QSqlQueryModel(this);
    lotModel = new QSqlQueryModel(this);

    // ui->lineEdit_slot_slot->setValidator(new QRegExpValidator(regex::slot(),this));

    if(!db.open())
    {
        qDebug() << "Problem Opening Database. " << db.lastError().text();
    }
    else
    {
        // LOGIN
        loginModel->setTable("login");
        loginModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        loginModel->select();
        loginModel->setHeaderData(0, Qt::Horizontal, tr("Username"));
        loginModel->setHeaderData(1, Qt::Horizontal, tr("Password"));
        loginModel->setHeaderData(2, Qt::Horizontal, tr("Salt"));
        loginModel->setHeaderData(3, Qt::Horizontal, tr("Security"));
        ui->tableView_users->setModel(loginModel);
        ui->tableView_users->show();
        ui->tableView_users->horizontalHeader()->setSectionResizeMode(
                    QHeaderView::Stretch);

        // LOG
        logModel->setQuery("SELECT * FROM product_log_view");
        logModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
        logModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
        logModel->setHeaderData(2, Qt::Horizontal, tr("Changed"));
        logModel->setHeaderData(3, Qt::Horizontal, tr("Product ID"));
        logModel->setHeaderData(4, Qt::Horizontal, tr("Warehouse ID"));
        logModel->setHeaderData(5, Qt::Horizontal, tr("Warehouse Code"));
        logModel->setHeaderData(6, Qt::Horizontal, tr("Bin ID"));
        logModel->setHeaderData(7, Qt::Horizontal, tr("Slot"));
        logModel->setHeaderData(8, Qt::Horizontal, tr("SKU"));
        logModel->setHeaderData(9, Qt::Horizontal, tr("Description"));
        logModel->setHeaderData(10, Qt::Horizontal, tr("Quantity"));
        logModel->setHeaderData(11, Qt::Horizontal, tr("Received"));
        logModel->setHeaderData(12, Qt::Horizontal, tr("Packed"));
        logModel->setHeaderData(13, Qt::Horizontal, tr("Expiration"));
        logModel->setHeaderData(14, Qt::Horizontal, tr("Notes"));
        logModel->setHeaderData(15, Qt::Horizontal, tr("Modified By"));
        ui->tableView_log->setModel(logModel);
        ui->tableView_log->hideColumn(0); // Hide 'ID'
        ui->tableView_log->hideColumn(3); // Hide 'Product ID'
        ui->tableView_log->hideColumn(4); // Hide 'Warehouse ID'
        ui->tableView_log->hideColumn(6); // Hide 'Bin ID'
        ui->tableView_log->hideColumn(15); // Hide 'Modified By'
        ui->tableView_log->show();
        ui->tableView_log->horizontalHeader()->setSectionResizeMode(
                    QHeaderView::Stretch);

        // WAREHOUSE
        ui->comboBox_slot_warehouse->setInsertPolicy(QComboBox::NoInsert);
        warehouseModel->setQuery("SELECT id, code FROM warehouse");
        ui->comboBox_slot_warehouse->setModel(warehouseModel);
        ui->comboBox_slot_warehouse->setModelColumn(1);
        // This will close the database, we must open it again
        if (!db.isOpen())
            db.open();

        // SLOT
        slotModel->setQuery("SELECT slot, id FROM bin "
                            "WHERE warehouse_id = " +
                            QString::number(selectedWarehouse.id) +
                            " ORDER BY slot");
        slotModel->setHeaderData(0, Qt::Horizontal, tr("Slot"));
        ui->tableView_slot->setModel(slotModel);
        ui->tableView_slot->hideColumn(1); // Hide 'id'
        ui->tableView_slot->show();
        ui->tableView_slot->horizontalHeader()->setSectionResizeMode(
                    QHeaderView::Stretch);

        // KIT
        kitModel->setQuery("SELECT id, sku, description FROM kit GROUP BY sku");
        while (kitModel->canFetchMore()) {
            kitModel->fetchMore();
        }
        kitModel->setHeaderData(1, Qt::Horizontal, tr("SKU"));
        kitModel->setHeaderData(2, Qt::Horizontal, tr("Description"));
        ui->tableView_product_kits->setModel(kitModel);
        ui->tableView_product_kits->hideColumn(0); // Hide 'id'
        ui->tableView_product_kits->show();
        ui->tableView_product_kits->resizeColumnsToContents();

        // ITEM IN KIT
        iikModel->setQuery("SELECT 'ID', 'Select Kit', 'And Items Will', 'Show Here', ''");
        while (iikModel->canFetchMore()) {
            iikModel->fetchMore();
        }
        iikModel->setHeaderData(1, Qt::Horizontal, tr("SKU"));
        iikModel->setHeaderData(2, Qt::Horizontal, tr("Description"));
        iikModel->setHeaderData(3, Qt::Horizontal, tr("Quantity"));
        iikModel->setHeaderData(4, Qt::Horizontal, tr("UOM"));
        ui->tableView_product_items_in_selected_kit->setModel(iikModel);
        ui->tableView_product_items_in_selected_kit->hideColumn(0); // Hide 'id'
        ui->tableView_product_items_in_selected_kit->show();
        ui->tableView_product_items_in_selected_kit->resizeColumnsToContents();

        // ITEM
        itemModel->setQuery("SELECT id, sku, description FROM item ORDER BY sku");
        while (itemModel->canFetchMore()) {
            itemModel->fetchMore();
        }
        itemModel->setHeaderData(1, Qt::Horizontal, tr("SKU"));
        itemModel->setHeaderData(2, Qt::Horizontal, tr("Description"));
        ui->tableView_product_items->setModel(itemModel);
        ui->tableView_product_items->hideColumn(0); // Hide 'id'
        ui->tableView_product_items->show();
        ui->tableView_product_items->resizeColumnsToContents();

        // LOTS
        lotModel->setQuery("SELECT * FROM lot");
        while (lotModel->canFetchMore()) {
            lotModel->fetchMore();
        }
        lotModel->setHeaderData(1, Qt::Horizontal, tr("Lot Code"));
        lotModel->setHeaderData(2, Qt::Horizontal, tr("Production Date"));
        lotModel->setHeaderData(3, Qt::Horizontal, tr("Expiration Date"));
        lotModel->setHeaderData(4, Qt::Horizontal, tr("Description"));
        ui->tableView_lots->setModel(lotModel);
        ui->tableView_lots->hideColumn(0); // Hide 'id'
        ui->tableView_lots->show();
        ui->tableView_lots->resizeColumnsToContents();
    }

    ui->label_log_queryText->setText("");
}

AdminDialog::~AdminDialog()
{
    QSettings settings;
    settings.setValue("adminDialogGeometry", saveGeometry());
    delete ui;
}

void AdminDialog::on_lineEdit_slot_slot_textEdited(const QString &arg1)
{
    ui->lineEdit_slot_slot->setText(arg1.toUpper());
    // Filter tableView_slot
    if(db.open()) {
        slotModel->setQuery("SELECT slot, id FROM bin WHERE slot LIKE '"
                            + arg1.toUpper() + "%' AND warehouse_id = "
                            + QString::number(selectedWarehouse.id) +
                            " ORDER BY slot");
        ui->tableView_slot->hideColumn(1); // Hide 'id'
    }
}

void AdminDialog::on_tableView_log_clicked(const QModelIndex &index)
{
    selectedLog.id =                        index.sibling(index.row(), 0).data().toInt();
    selectedLog.type =                      index.sibling(index.row(), 1).data().toString();
    selectedLog.changed =                   index.sibling(index.row(), 2).data().toDateTime();
    selectedLog.product.id =                index.sibling(index.row(), 3).data().toInt();
    selectedLog.product.warehouse.id =      index.sibling(index.row(), 4).data().toInt();
    selectedLog.product.warehouse.code =    index.sibling(index.row(), 5).data().toString();
    selectedLog.product.bin.id =            index.sibling(index.row(), 6).data().toInt();
    selectedLog.product.bin.slot =          index.sibling(index.row(), 7).data().toString();
    selectedLog.product.kit.sku =           index.sibling(index.row(), 8).data().toString();
    selectedLog.product.kit.description =   index.sibling(index.row(), 9).data().toString();
    selectedLog.product.quantity =          index.sibling(index.row(), 10).data().toInt();
    selectedLog.product.received =          index.sibling(index.row(), 11).data().toDate();
    selectedLog.product.packed =            index.sibling(index.row(), 12).data().toDate();
    selectedLog.product.expiration =        index.sibling(index.row(), 13).data().toDate();
    selectedLog.product.notes =             index.sibling(index.row(), 14).data().toString();
}

void AdminDialog::deleteLogEntry(ProductLog entry)
{
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM product_log WHERE warehouse_id = :whseID "
                          "AND bin_id = :binID AND changed_on <= :changedDate")) {
            query.bindValue(":whseID", entry.product.warehouse.id);
            query.bindValue(":binID", entry.product.bin.id);
            query.bindValue(":changedDate", entry.changed);
            if (query.exec()) {
                if (query.prepare("DELETE FROM product_log WHERE id IN "
                                  "(SELECT id FROM product_log "
                                  "ORDER BY changed_on DESC) AND warehouse_id = "
                                  ":whseID AND bin_id = :binID")) {
                    query.bindValue(":whseID", entry.product.warehouse.id);
                    query.bindValue(":binID", entry.product.bin.id);
                    if (query.exec()) {

                    } else {
                        //didn't delete last entry
                    }
                } else {
                    // didn't preapre 2nd
                }
            } else {
                // didn't exec first
            }
        } else {
            //didn't prepare 1st
        }
        db.close();
    } else {
        // didn't open db
    }
}

void AdminDialog::on_pushButton_revert_clicked()
{
    bool changed{false};
    if (selectedLog.id > 0) {
        if (selectedLog.type == "insert") {
            if (QMessageBox::warning(this, "Remove Product", "Are you sure you "
                                     "want to remove the\n"
                                     "selected product from the product table?"
                                     "\nThis operation cannot be undone!",
                                     QMessageBox::No,
                                     QMessageBox::Yes) == QMessageBox::Yes) {
                if (removeProduct(db, selectedLog.product)) {
                    changed = true;
                    // Now we need to delete the log entry
                    deleteLogEntry(selectedLog);
                }
            }
        } else if (selectedLog.type == "delete") {
            if (QMessageBox::warning(this, "Insert Product",
                                     "Are you sure you want to reinsert the "
                                     "previously deleted product?\n"
                                     "This operation cannot be undone!",
                                     QMessageBox::No,
                                     QMessageBox::Yes) == QMessageBox::Yes) {
                if (addProduct(db, selectedLog.product, login)) {
                    changed = true;
                    // Now we need to delete the log entry
                    deleteLogEntry(selectedLog);
                }
            }
            // We have to insert the product back into the table
        } else if (selectedLog.type == "update_after" ||
                   selectedLog.type == "update_before") {
            if (QMessageBox::warning(this, "Update Product",
                                     "Are you sure you want to revert the "
                                     "changes to this product?\n"
                                     "This operation cannot be undone!",
                                     QMessageBox::No,
                                     QMessageBox::Yes) == QMessageBox::Yes) {
                qDebug() << "TODO: Get old and new product";
                qDebug() << "TODO: editProduct(db, old, new);";
                if (false /* product is updated */) {
                    changed = true;
                    qDebug() << "TODO: Remove Log Entries";
                }
            }
        }
    }
    if (changed) {
        qDebug() << "Changed!";
        if (!db.isOpen()) {
            db.open();
            logModel->setQuery("SELECT * FROM product_log_view");
            db.close();
        }
    }
}

void AdminDialog::refreshSlots()
{
    if (db.open()) {
        slotModel->setQuery("SELECT slot, id FROM bin "
                            "WHERE warehouse_id = " +
                            QString::number(selectedWarehouse.id) +
                            " ORDER BY slot");
    }
}

void AdminDialog::on_tableView_slot_clicked(const QModelIndex &index)
{
    // Set selected bin
    selectedBin.slot = index.sibling(index.row(), 0).data().toString();
    selectedBin.id =    index.sibling(index.row(), 1).data().toInt();
}

void AdminDialog::on_pushButton_slot_add_clicked()
{
    QString enteredSlots = ui->lineEdit_slot_slot->text();
    QStringList slotList = enteredSlots.trimmed().split(",");
    bool yesToAll{false};
    for (QString slot : slotList) {
        if (yesToAll) {
            if (!slotExists(db, slot, selectedWarehouse)) {
                if (addSlot(db, slot, selectedWarehouse)) {
                    qDebug() << "Entering all Slots";
                }
            }
            continue;
        }
        QMessageBox question(this);
        question.setWindowTitle("Add Slot?");
        question.setText("Do you want to add this slot?\n SLOT: " + slot);
        question.setStandardButtons(QMessageBox::Yes | QMessageBox::YesToAll |
                                    QMessageBox::No | QMessageBox::NoToAll);

        auto answer = question.exec();
        switch (answer) {
        case QMessageBox::Yes:
            if (!slotExists(db, slot, selectedWarehouse)) {
                if (addSlot(db, slot, selectedWarehouse)) {
                    qDebug() << "Entering new Slot";
                }
            }
            break;
        case QMessageBox::YesToAll:
            if (!slotExists(db, slot, selectedWarehouse)) {
                if (addSlot(db, slot, selectedWarehouse)) {
                    qDebug() << "Entering new slot and selecting yet for all";
                }
            }
            yesToAll = true;
            break;
        case QMessageBox::No:
            continue;
            break;
        case QMessageBox::NoToAll:
            return;
            break;
        default:
            break;
        }
    }
    refreshSlots();
}

void AdminDialog::tryRemoveSlot(QString slot, Warehouse warehouse)
{
    if (slotExists(db, slot, warehouse)) {
        if (slotIsEmpty(db, slot, warehouse.id)) {
            if (removeSlot(db, slot, warehouse)) {
                qDebug() << "Removing Slot";
            } else {
                QMessageBox::warning(this, "Could Not Remove",
                     "There was a problem removing the slot: " + slot
                     + ".\n Please make sure the slot exists in the "
                     "selected warehouse and that product does not "
                     "reference it.\n\nIf this problem persists, please "
                     "contact your database administrator.");
            }
        }
    }
}

void AdminDialog::on_pushButton_slot_remove_clicked()
{
    // Check that the entered text is a slot that exists in the current warehouse
    QString enteredSlots = ui->lineEdit_slot_slot->text();
    QStringList slotList = enteredSlots.trimmed().split(",");
    bool yesToAll{false};
    for (QString slot : slotList) {
        if (yesToAll) {
            tryRemoveSlot(slot, selectedWarehouse);
            continue;
        }
        QMessageBox question(this);
        question.setWindowTitle("Remove Slot?");
        question.setText("Are you sure you wish to remove?\nWarehouse: "
                         + selectedWarehouse.code + "\n Slot: "  + slot);
        question.setStandardButtons(QMessageBox::Yes | QMessageBox::YesToAll |
                                    QMessageBox::No | QMessageBox::NoToAll);
        auto answer = question.exec();
        switch (answer) {
        case QMessageBox::Yes:
            tryRemoveSlot(slot, selectedWarehouse);
            break;
        case QMessageBox::YesToAll:
            tryRemoveSlot(slot, selectedWarehouse);
            yesToAll = true;
            break;
        case QMessageBox::No:
            continue;
        case QMessageBox::NoToAll:
            return;
        default:
            break;
        }
    }
    refreshSlots();
}

void AdminDialog::on_lineEdit_product_kit_search_textEdited(const QString &arg1)
{
    if(db.open()) {
        kitModel->setQuery("SELECT id, sku, description FROM kit "
                           "WHERE sku LIKE '%" + arg1 + "%' "
                           "OR description LIKE '%" + arg1 + "%' GROUP BY sku");
        db.close();
    }
}

void AdminDialog::on_pushButton_product_kit_edit_clicked()
{
    if (selectedKit.id <= 0) { return; }

    Kit newKit;
    bool skuOK{false}, descOK{false};

    newKit.sku =
            QInputDialog::getText(this, "Edit Kit Number",
                                  "Enter the new kit number",
                                  QLineEdit::Normal, selectedKit.sku, &skuOK);
    if (skuOK) {
        newKit.description =
                QInputDialog::getText(this, "Edit Kit Description",
                                      "Enter the new kit description",
                                      QLineEdit::Normal,
                                      selectedKit.description, &descOK);
        if (descOK) {
            if (updateKit(db, selectedKit, newKit)) {
                if (db.open()) {
                    QString arg = ui->lineEdit_product_kit_search->text();
                    arg.prepend('%');
                    arg.append('%');
                    kitModel->setQuery("SELECT id, sku, description FROM kit "
                                       "WHERE sku LIKE '%" + arg + "%' OR "
                                       "description LIKE '%" + arg + "%' "
                                       "GROUP BY sku");
                    db.close();
                } else { /* DB didn't open */ }
            } else { /* Couldn't Add Item */ }
        } else { /* Canceled Description */ }
    } else { /* Canceled SKU */ }

}

void AdminDialog::on_pushButton_product_kit_add_clicked()
{
    Kit kit;
    bool skuOK{false}, descOK{false};

    kit.sku =
            QInputDialog::getText(this, "New Kit Number",
                                  "Enter the new kit number",
                                  QLineEdit::Normal, QString(), &skuOK);
    if (skuOK) {
        kit.description =
                QInputDialog::getText(this, "New Kit Description",
                                      "Enter the new kit description",
                                      QLineEdit::Normal, QString(), &descOK);
        if (descOK) {
            if (addKit(db, kit)) {
                if (db.open()) {
                    QString arg = ui->lineEdit_product_kit_search->text();
                    arg.prepend('%');
                    arg.append('%');
                    kitModel->setQuery("SELECT id, sku, description FROM kit "
                                       "WHERE sku LIKE '%" + arg + "%' OR "
                                       "description LIKE '%" + arg + "%' "
                                       "GROUP BY sku");
                    db.close();
                } else { /* DB didn't open */ }
            } else { /* Couldn't Add Item */ }
        } else { /* Canceled Description */ }
    } else { /* Canceled SKU */ }
}

void AdminDialog::on_pushButton_product_kit_remove_clicked()
{
    if (QMessageBox::warning(this, "Remove Kit", "Are you sure you want to "
                             "remove this kit? You can not undo this change.",
                             QMessageBox::No,
                             QMessageBox::Yes) == QMessageBox::Yes) {
        if (removeKit(db, selectedKit.sku)) {
            if (db.open()) {
                QString arg = ui->lineEdit_product_kit_search->text();
                arg.prepend('%');
                arg.append('%');
                kitModel->setQuery("SELECT id, sku, description FROM kit "
                                   "WHERE sku LIKE '%" + arg + "%' OR "
                                   "description LIKE '%" + arg + "%' "
                                   "GROUP BY sku");
                db.close();
            } else { /* DB didn't open */ }
        } else { /* Failed to remove item */ }
    }
}

void AdminDialog::on_tableView_product_kits_clicked(const QModelIndex &index)
{
    // Filter tableView_product_items_in_selected_kit
    if(db.open()) {
        iikModel->setQuery("SELECT kit.id, item.sku, item.description, "
                           "kit.quantity, unit.code FROM kit "
                           "JOIN item ON kit.item_id=item.id "
                           "JOIN unit ON unit_id=unit.id WHERE kit.sku='"
                           + index.sibling(index.row(), 1).data().toString() + "'");
        db.close();
    }
    ui->tableView_product_items_in_selected_kit->hideColumn(0); // Hide ID
    ui->tableView_product_items_in_selected_kit->resizeColumnsToContents();

    // Set selected kit
    selectedKit.id = index.sibling(index.row(), 0).data().toInt();
    selectedKit.sku = index.sibling(index.row(), 1).data().toString();
    selectedKit.description = index.sibling(index.row(), 2).data().toString();
}

void AdminDialog::on_tableView_product_items_in_selected_kit_clicked(const QModelIndex &index)
{
    selectedItemInKit.id = index.sibling(index.row(), 0).data().toInt();
    selectedItemInKit.sku = index.sibling(index.row(), 1).data().toString();
    selectedItemInKit.description = index.sibling(index.row(), 2).data().toString();
}

void AdminDialog::on_lineEdit_product_item_search_textEdited(const QString &arg1)
{
    // Filter tableView_product_item
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT id, sku, description FROM item WHERE sku LIKE "
                          ":sku OR description LIKE :desc ORDER BY sku")) {
            QString arg = arg1;
            arg.prepend('%');
            arg.append('%');
            query.bindValue(":sku", arg);
            query.bindValue(":desc", arg);
            if (query.exec()) {
                itemModel->setQuery(query);
            }
        } else { /* Query didn't prepare */ }
        db.close();
    } else { /* DB didn't open */ }
}

void AdminDialog::on_pushButton_product_item_edit_clicked()
{
    if (selectedItem.id <= 0) { return; }

    Item newItem;
    bool skuOK{false}, descOK{false};
    newItem.sku =
            QInputDialog::getText(this, "Edit Item Number",
                                  "Enter the new item number",
                                  QLineEdit::Normal, selectedItem.sku, &skuOK);
    if (skuOK) {
        newItem.description =
                QInputDialog::getText(this, "Edit Item Description",
                                      "Enter the new item description",
                                      QLineEdit::Normal,
                                      selectedItem.description, &descOK);
        if (descOK) {
            if (updateItem(db, selectedItem, newItem)) {
                if (db.open()) {
                    QString arg = ui->lineEdit_product_item_search->text();
                    arg.prepend('%');
                    arg.append('%');
                    itemModel->setQuery("SELECT id, sku, description FROM item "
                                        "WHERE sku LIKE '%" + arg + "%' OR "
                                        "description LIKE '%" + arg + "%' "
                                        "ORDER BY sku");
                    db.close();
                } else { /* DB didn't open */ }
            } else { /* Couldn't Add Item */ }
        } else { /* Canceled Description */ }
    } else { /* Canceled SKU */ }
}

void AdminDialog::on_pushButton_product_item_add_clicked()
{
    Item item;
    bool skuOK{false}, descOK{false};
    item.sku =
            QInputDialog::getText(this, "New Item Number",
                                  "Enter the new item number",
                                  QLineEdit::Normal, QString(), &skuOK);
    if (skuOK) {
        item.description =
                QInputDialog::getText(this, "New Item Description",
                                      "Enter the new item description",
                                      QLineEdit::Normal, QString(), &descOK);
        if (descOK) {
            if (addItem(db, item)) {
                if (db.open()) {
                    QString arg = ui->lineEdit_product_item_search->text();
                    arg.prepend('%');
                    arg.append('%');
                    itemModel->setQuery("SELECT id, sku, description FROM item "
                                        "WHERE sku LIKE '%" + arg + "%' OR "
                                        "description LIKE '%" + arg + "%' "
                                        "ORDER BY sku");
                    db.close();
                } else { /* DB didn't open */ }
            } else { /* Couldn't Add Item */ }
        } else { /* Canceled Description */ }
    } else { /* Canceled SKU */ }
}

void AdminDialog::on_pushButton_product_item_remove_clicked()
{
    if (QMessageBox::warning(this, "Remove Item", "Are you sure you want to "
                             "remove this item? You can not undo this change.",
                             QMessageBox::No,
                             QMessageBox::Yes) == QMessageBox::Yes) {
        if (removeItem(db, selectedItem.id)) {
            if (db.open()) {
                QString arg = ui->lineEdit_product_item_search->text();
                arg.prepend('%');
                arg.append('%');
                itemModel->setQuery("SELECT id, sku, description FROM item "
                                    "WHERE sku LIKE '%" + arg + "%' OR "
                                    "description LIKE '%" + arg + "%' "
                                    "ORDER BY sku");
                db.close();
            } else { /* DB didn't open */ }
        } else { /* Failed to remove item */ }
    }
}

void AdminDialog::on_tableView_product_items_clicked(const QModelIndex &index)
{
    selectedItem.id = index.sibling(index.row(), 0).data().toInt();
    selectedItem.sku = index.sibling(index.row(), 1).data().toString();
    selectedItem.description = index.sibling(index.row(), 2).data().toString();
}

void AdminDialog::refreshItemsInKit()
{
    on_tableView_product_kits_clicked(
                ui->tableView_product_kits->selectionModel()
                ->selectedIndexes().first());
}

void AdminDialog::on_pushButton_product_add_to_kit_clicked()
{
    if (!selectedKit.sku.isEmpty()) {
        if (selectedItem.id > 0) {
            bool isResultOK;
            int amountToAdd = QInputDialog::getInt(this, "Add to Kit",
                                                   "Add how many?", 1, 1,
                                                   9999999, 1, &isResultOK);
            if (isResultOK) {
                if(addItemToKit(db, selectedKit.sku, selectedKit.description,
                                selectedItem.id, amountToAdd))
                {
                    refreshItemsInKit();
                } else { /* Failed to add Item */ }
            } else { /* Selected Cancel */ }
        } else { /*select Item*/ }
    } else { /*select Kit */ }
}

void AdminDialog::on_pushButton_product_remove_from_kit_clicked()
{

    if (!selectedKit.sku.isEmpty()) {
        if (selectedItemInKit.id > 0) {
            if (QMessageBox::question(this, tr("Remove From Kit?"),
                                      "Remove: " + selectedItemInKit.sku + "\n"
                                      + selectedItemInKit.description + "\n"
                                      + "From: " + selectedKit.sku + "\n"
                                      + selectedKit.description + "?",
                                      QMessageBox::Yes,
                                      QMessageBox::No) == QMessageBox::Yes) {
                if(removeItemFromKit(db, selectedItemInKit.id))
                {
                    refreshItemsInKit();
                }
                else { /* Failed to add Item */ }
            }
        }
        else { /*select Item In Kit */ }
    }
    else { /*select Kit */ }
}


void AdminDialog::on_pushButton_query_exec_clicked()
{
    QString statement = ui->textEdit_query->toPlainText();
    if(statement.isEmpty())
        return;

    if(!db.open())
    {
        ui->label_query_result->setText(db.lastError().text());
    }
    else
    {
        QSqlQuery query;
        if(!query.exec(statement))
        {
            ui->label_query_result->setText(query.lastError().text());
        }
        else
        {
            ui->label_query_result->setText("Executed Successfully");
            if(query.isSelect())
            {
                QSqlQueryModel *queryModel = new QSqlQueryModel(this);
                queryModel->setQuery(query);
                ui->tableView_query->setModel(queryModel);
                ui->tableView_query->show();
                ui->tableView_query->resizeColumnsToContents();
            }
        }
        db.close();
    }
}

void AdminDialog::on_pushButton_login_reset_clicked()
{
    if(QMessageBox::warning(this, "Reset Password",
                            "Are you sure you want to reset the password for "
                            + login.username + "?\n"
                            "You will not be able to undo this change."
                            "\nPress Ok to submit or Cancel.",
                            QMessageBox::No,
                            QMessageBox::Yes) == QMessageBox::Yes) {
        security::markUserForPasswordReset(db, login.username);
    }
}

void AdminDialog::on_tableView_users_clicked(const QModelIndex &index)
{
    login.username = index.sibling(index.row(), 0).data().toString();
}

void AdminDialog::on_comboBox_slot_warehouse_currentIndexChanged(int index)
{
    selectedWarehouse.id =
            ui->comboBox_slot_warehouse->model()->index(index,0).data().toInt();
    qDebug() << "WarehouseID: " << selectedWarehouse.id;
    selectedWarehouse.code =
            ui->comboBox_slot_warehouse->model()->index(index,1).data().toString();
    qDebug() << "WarehouseCODE: " << selectedWarehouse.code;
}

void AdminDialog::on_pushButton_warehouse_add_clicked()
{
    Warehouse result;
    // Get new warehouse for DB
    AddWarehouseDialog dialog(this, db, result);
    // if result acceptable
    if (dialog.exec() == AddWarehouseDialog::Accepted) {
        db.open();
        warehouseModel->setQuery("SELECT id, code FROM warehouse");
        db.close();
        ui->comboBox_slot_warehouse->setCurrentIndex(1);
        // and select the new one
    }
}

void AdminDialog::on_pushButton_warehouse_remove_clicked()
{
    if (QMessageBox::question(this, "Remove Selected Location?",
                              "Are you sure you want to remove this location?",
                              QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes) {
        if (removeWarehouse(db, selectedWarehouse.id)) {
            db.open();
            warehouseModel->setQuery("SELECT id, code FROM warehouse");
            db.close();
            ui->comboBox_slot_warehouse->setCurrentIndex(1);
        }
    }
}

void AdminDialog::on_comboBox_slot_warehouse_activated(int index)
{
    selectedWarehouse.id =
            ui->comboBox_slot_warehouse->model()->index(index,0).data().toInt();
    qDebug() << "WarehouseID: " << selectedWarehouse.id;
    selectedWarehouse.code =
            ui->comboBox_slot_warehouse->model()->index(index,1).data().toString();
    qDebug() << "WarehouseCODE: " << selectedWarehouse.code;
    // Update the slotModel
    if (db.open()) {
        slotModel->setQuery("SELECT slot, id FROM bin "
                            "WHERE warehouse_id = " +
                            QString::number(selectedWarehouse.id) +
                            " ORDER BY slot");
        slotModel->setHeaderData(0, Qt::Horizontal, tr("Slot"));
        ui->tableView_slot->hideColumn(1); // Hide 'id'
        db.close();
    }
}

void AdminDialog::on_tableView_slot_doubleClicked(const QModelIndex &index)
{
    ui->lineEdit_slot_slot->setText(index.sibling(index.row(), 0).data().toString());
}

void AdminDialog::on_lineEdit_lot_search_textEdited(const QString &arg1)
{
    // Filter tableView_logs
    if(db.open()) {
        lotModel->setQuery("SELECT id, code, production, expiration, description "
                           "FROM lot WHERE description LIKE "
                            + arg1.toUpper() + "%' or CODE LIKE "
                            + arg1.toUpper() + " ORDER BY production");
        ui->tableView_lots->hideColumn(0); // Hide 'id'
    }
}

void AdminDialog::on_pushButton_lot_add_clicked()
{
    // Get new warehouse for DB
    AddLotDialog dialog(this, db);
    // if result acceptable
    if (dialog.exec() == AddLotDialog::Accepted) {
        db.open();
        lotModel->setQuery("SELECT * FROM lot");
        db.close();
    }
}

void AdminDialog::on_pushButton_lot_remove_clicked()
{
    if (QMessageBox::question(this, "Remove Selected Lot Code?",
                              "Are you sure you want to remove this lot code "
                              "from the list?\nCode: " + selectedLot.code +
                              "\nDescription: " + selectedLot.description,
                              QMessageBox::No,
                              QMessageBox::Yes) == QMessageBox::Yes) {
        if (removeLot(db, selectedLot.id)) {
            db.open();
            lotModel->setQuery("SELECT * FROM lot");
            db.close();
        }
    }
}

void AdminDialog::on_tableView_lots_clicked(const QModelIndex &index)
{
    selectedLot.id = index.sibling(index.row(),0).data().toInt();
    selectedLot.code = index.sibling(index.row(), 1).data().toString();
    selectedLot.production = index.sibling(index.row(), 2).data().toDate();
    selectedLot.expiration = index.sibling(index.row(), 3).data().toDate();
    selectedLot.description = index.sibling(index.row(), 4).data().toString();
}
