#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QSqlDatabase &mDB, sLogin &mLogin, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    login(mLogin),
    db(mDB)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());

    /// CREATE NEW MODEL FOR TABLE VIEW
    imodel = new QSqlQueryModel(this);
    wmodel = new QSqlQueryModel(this);
    wSearchModel = new QSqlQueryModel(this);

    //Load DB
    //else Try to open new DB
    if (loadDatabasePath() || openNewDatabase()) {
        // Set warehouse_view model 'wmodel'
        if (db.open()) {
            wmodel->setQuery("SELECT * FROM warehouse_view");
            ui->tableView_warehouse->setModel(wmodel);
            ui->tableView_warehouse->hideColumn(0); // Hide warehouse.id
            ui->tableView_warehouse->hideColumn(1); // Hide warehouse.code
            ui->tableView_warehouse->hideColumn(2); // Hide product.id
            ui->tableView_warehouse->hideColumn(3); // Hide product.bin.id
            ui->tableView_warehouse->show();
            ui->tableView_warehouse->resizeColumnsToContents();
            product = Product();
        }

        if (db.open()) {
            imodel->setQuery("SELECT * FROM inventory_view");
            ui->tableView_inventory->setModel(imodel);
            ui->tableView_inventory->show();
            ui->tableView_inventory->resizeColumnsToContents();
        }

            // Fill the comboBox with headers from warehouse_view table model
            // Push header data into the combobox;
            // THIS WILL TRIGGER comboBox_search_mode_current_text_changed(...)
            // which will close the current DB connection. It will need to be
            // re-opened after this loop
        for (int index = 0;
             index < wmodel->columnCount();
             index++) {
            ui->comboBox_search_mode->addItem(
                        wmodel->headerData(index, Qt::Horizontal).toString());
        }

        if (!loadSearchMode()) {
            ui->comboBox_search_mode->setCurrentIndex(0);
        } else {
            if (db.open()) {
                QSqlQuery query;
                ui->comboBox_search_mode->setCurrentText(searchMode);
                if (query.prepare("SELECT " + searchMode
                                  + " FROM warehouse_view ")) {

                    if (query.exec()) {
                        wSearchModel->setQuery(query);

                        QCompleter *searchCompleter = new QCompleter(wSearchModel, this);
                        searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
                        searchCompleter->setCompletionMode(
                                    QCompleter::UnfilteredPopupCompletion);
                        if (completer)
                            ui->lineEdit_search->setCompleter(searchCompleter);
                    } else {
                        // Query failed to execute
                        qDebug() << query.lastError().text();
                        qDebug() << database::getLastExecutedQuery(query);
                    }
                } else {
                    // Query Failed to prepare
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
                db.close();
            }
        }
    }
    ui->actionCompletion_Popup->setChecked(completer);
    setCompleter(completer);

    savePreferences();

    ui->tableView_warehouse->addAction(ui->actionAdd_Item);
    ui->tableView_warehouse->addAction(ui->actionRemove_Item);
    ui->tableView_warehouse->addAction(ui->actionChange_Item);
    ui->tableView_warehouse->addAction(ui->actionMove_Item);
    ui->tableView_warehouse->setContextMenuPolicy(Qt::ActionsContextMenu);

    updateSecurity(login);
}

void MainWindow::closeEvent(QCloseEvent *) {
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

MainWindow::~MainWindow()
{
    savePreferences();
    delete ui;
}

void MainWindow::setCompleter(bool active) {
    completer = active;
    if (completer) {
        if (db.open()) {
            QSqlQuery query;
            // Fill Product model for auto complete
            // REQUIRES: comboBox be filled with selection
            // This should be filled after the load preferences.
            if (query.prepare("SELECT " + searchMode + " FROM warehouse_view ")) {
                if (query.exec()) {
                    wSearchModel->setQuery(query);
                    QCompleter *searchCompleter =
                            new QCompleter(wSearchModel, this);
                    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
                    searchCompleter->setCompletionMode(
                                QCompleter::UnfilteredPopupCompletion);
                    ui->lineEdit_search->setCompleter(searchCompleter);
                } else {
                    // Query failed to execute
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            } else {
                // Query Failed to prepare
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
            db.close();
        } else {
            // DB failed to open
            qDebug() << db.lastError().text();
        }
    } else {
        ui->lineEdit_search->setCompleter(0);
        freeDeadChildren();
    }
}

bool MainWindow::loadDatabasePath()
{
    QSettings settings;
#ifdef DEBUG_MODE
    QString databasePath = "debug/databasePath";
#else
    QString databasePath = "databasePath";
#endif
    if (settings.contains(databasePath) &&
            !settings.value(databasePath).toString().isEmpty()) {

        this->databasePath = settings.value(databasePath).toString();
        db.setDatabaseName(this->databasePath);
        ui->statusBar->showMessage("Database Loaded: " + this->databasePath);
        return true;
    } else {
        ui->statusBar->showMessage("Database Failed To Load: "
                                   + this->databasePath);
        return false;
    }
}

bool MainWindow::loadSearchMode()
{
    QSettings settings;
#ifdef DEBUG_MODE
    QString searchMode     = "debug/searchMode";
#else
    QString searchMode     = "searchMode";
#endif

    if (settings.contains(searchMode) &&
            !settings.value(searchMode).toString().isEmpty()) {

        this->searchMode = settings.value(searchMode).toString();
        ui->comboBox_search_mode->setCurrentText(this->searchMode);
        return true;
    } else {
        ui->statusBar->showMessage("Settings Failed To Load: "
                                   + this->searchMode);
        return false;
    }
}

bool MainWindow::loadPreferences()
{
    if (loadDatabasePath())
        if (loadSearchMode())
            return true;
    // This should literally never happen, loads are nothrow returns.
    return false;
}

bool MainWindow::saveDatabasePath()
{
    QSettings settings;
#ifdef DEBUG_MODE
    QString databasePath = "debug/databasePath";
#else
    QString databasePath = "databasePath";
#endif
    settings.setValue(databasePath, this->databasePath);
    return true;
}

bool MainWindow::saveSearchMode()
{
    QSettings settings;
#ifdef DEBUG_MODE
    QString modePath     = "debug/searchMode";
#else
    QString modePath     = "searchMode";
#endif
    settings.setValue(modePath, searchMode);
    return true;
}

bool MainWindow::savePreferences()
{
    if (saveDatabasePath()) {
        if (saveSearchMode()) {
            return true;
        }
    }
    // Honestly, this should never return, both saves should return true.
    return false;
}

bool MainWindow::openNewDatabase()
{
    // TODO: This should have an options to create a new database as well.
    databasePath = QFileDialog::getOpenFileName(this, "Open Database",
                                            QDir::currentPath(),
                                            "Database Files (*.db *.sqlite)");
    if(!databasePath.isEmpty())
    {
        db.setDatabaseName(databasePath);
        if(db.open())
        {
            db.close();
            return true;
        }
    }
    return false;
}

void MainWindow::updateSecurity(sLogin &cred)
{
    /* SECURITY DETAILS
     * 0    No privlages, Cannot view any tables
     * 1    View and Search tables, Cannot edit
     * 2    View, Search and Edit tables.
     * 3    Admin
     */

    // Initialize all disabled
    ui->actionLog_In->setVisible(true);
    ui->actionLog_Out->setVisible(false);

    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->setTabEnabled(0, false);

    ui->actionAdd_Item->setVisible(false);
    ui->actionMove_Item->setVisible(false);
    ui->actionChange_Item->setVisible(false);
    ui->actionRemove_Item->setVisible(false);

    ui->actionAdmin->setVisible(false);

    // 0    No privlages, Cannot view any tables
    if(cred.security > 0) {

        // Enable Login/Logout
        ui->actionLog_In->setVisible(false);
        ui->actionLog_Out->setVisible(true);

        // 1    View and Search tables, Cannot edit
        if (cred.security >= 1) {

            // Enable Warehouse Tab
            ui->tabWidget->setCurrentIndex(0);
            ui->tabWidget->setTabEnabled(0, true);

            // 2    View, Search and Edit tables.
            if (cred.security >= 2) {

                // Enable Editing
                ui->actionAdd_Item->setVisible(true);
                ui->actionMove_Item->setVisible(true);
                ui->actionChange_Item->setVisible(true);
                ui->actionRemove_Item->setVisible(true);

                // 3    Admin
                if (cred.security >= 3) {
                    ui->actionAdmin->setVisible(true);
                }
            }
        }
    }
}

void MainWindow::freeDeadChildren()
{
    QObjectList children = this->children();
    for (QObject *child : children) {
        if (!child) {
            delete child;
        }
    }
}

void MainWindow::updateWHSEView(QString searchString)
{
    if (db.open()) {
        QSqlQuery query;

        if (query.prepare("SELECT * FROM warehouse_view "
                          "WHERE " + searchMode + " LIKE :value")) {

            searchString.prepend('%');
            searchString.append('%');
            query.bindValue(":value", searchString);

            if(query.exec()) {

                wmodel->setQuery(query);

//                ui->tableView_warehouse->setModel(wmodel);
                ui->tableView_warehouse->hideColumn(0); // Hide warehouse.id
                ui->tableView_warehouse->hideColumn(1); // Hide warehouse.code
                ui->tableView_warehouse->hideColumn(2); // Hide product.id
                ui->tableView_warehouse->hideColumn(3); // Hide product.bin.id
                ui->tableView_warehouse->show();
                ui->tableView_warehouse->resizeColumnsToContents();
                product = Product();
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
        // DB couldn't open
        ui->statusBar->showMessage("Cannot Open DB: " + db.lastError().text(), 5000);
    }
}

void MainWindow::updateINVView(QString searchString)
{
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT * FROM inventory_view "
                          "WHERE sku LIKE :sku "
                          "OR description LIKE :desc")) {
            searchString.prepend('%');
            searchString.append('%');

            query.bindValue(":sku", searchString);
            query.bindValue(":desc", searchString);

            if (query.exec()) {
                imodel->setQuery(query);

                ui->tableView_inventory->setModel(imodel);
                ui->tableView_inventory->show();
                ui->tableView_inventory->resizeColumnsToContents();
            } else {
                // Query didn't execute
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            // Query didn't prepare
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        // DB didn't open
        ui->statusBar->showMessage("Cannot Open DB: "
                                   + db.lastError().text(), 5000);
    }
}

void MainWindow::on_actionOpen_Database_triggered()
{
    openNewDatabase();
    saveDatabasePath();
    updateWHSEView();
    updateINVView();
}

void MainWindow::on_tableView_warehouse_pressed(const QModelIndex &index)
{
    ui->tableView_warehouse->selectRow(index.row());

    product.warehouse.id    = index.sibling(index.row(),0).data().toInt();
    product.warehouse.code  = index.sibling(index.row(),1).data().toString();
    product.id              = index.sibling(index.row(),2).data().toInt();
    product.bin.id          = index.sibling(index.row(),3).data().toInt();
    product.kit.sku         = index.sibling(index.row(),4).data().toString();
    product.kit.description = index.sibling(index.row(),5).data().toString();
    product.quantity        = index.sibling(index.row(),6).data().toInt();
    product.received        = index.sibling(index.row(),7).data().toDate();
    product.packed          = index.sibling(index.row(),8).data().toDate();
    product.expiration      = index.sibling(index.row(),9).data().toDate();
    product.modified        = index.sibling(index.row(),10).data().toDateTime();
    product.modified_by     = index.sibling(index.row(),11).data().toString();
    product.bin.slot        = index.sibling(index.row(),12).data().toString();
    product.notes           = index.sibling(index.row(),13).data().toString();

}

void MainWindow::on_lineEdit_search_textChanged(const QString &arg1)
{
    updateWHSEView(arg1);
}

void MainWindow::on_tableView_warehouse_doubleClicked(const QModelIndex &index)
{
    if(index.column() > 1)
        return;
    else
    {
        if(index.column() >= 10) {
            // Copy location to search bar
            QString location = index.sibling(index.row(),12).data().toString();
            ui->lineEdit_search->setText(location);
        }
        else
        {
            // Copy current selection to search bar
            ui->lineEdit_search->setText(index.data().toString());
        }
    }
}

void MainWindow::on_actionAdd_Item_triggered()
{
    AddDialog addDialog(db, product, login, this);

    ui->statusBar->showMessage("Adding Item");

    if (addDialog.exec() == QDialog::Accepted)  {
        updateWHSEView(ui->lineEdit_search->text());
        ui->statusBar->showMessage("Item Added Successfully.", 2000);
    } else {
        ui->statusBar->showMessage("Item not added", 2000);
    }
}

void MainWindow::on_actionRemove_Item_triggered()
{
    QItemSelectionModel *itemSelectionModel = ui->tableView_warehouse->selectionModel();
    QSqlQueryModel *queryModel = wmodel;

    if (itemSelectionModel && queryModel) {
        QModelIndexList indexList = itemSelectionModel->selectedRows();
        QList<Product> preList;
        for (QModelIndex row : indexList) {
            Product p;
            p.id = row.sibling(row.row(), 2).data().toInt();
            p.warehouse.id = row.sibling(row.row(), 0).data().toInt();
            p.warehouse.code = row.sibling(row.row(), 1).data().toString();
            p.bin.id = row.sibling(row.row(), 3).data().toInt();
            p.bin.slot = row.sibling(row.row(), 12).data().toString();
            p.kit.sku = row.sibling(row.row(), 4).data().toString();
            p.kit.description = row.sibling(row.row(), 5).data().toString();
            p.quantity = row.sibling(row.row(), 6).data().toInt();
            p.modified = row.sibling(row.row(), 10).data().toDateTime();
            preList.push_back(p);
        }

        QList<Product> postList; // List after culling non removable Product
        for (Product p : preList) {
            if (p.removable()) {
                postList.push_back(p);
            } else {
                QMessageBox::about(this, "Cannot Remove", "No item is selected, "
                                                          "or selected item is "
                                                          "not removeable.");
                return;
            }
        }

        QMessageBox removeDialog(this);
        bool yesToAll{false};

        // Remove based on selection from dialog.
        for (int index = 0; index < postList.size(); index++) {
            Product p = postList[index];
            int selection;
            if (!yesToAll) {
                removeDialog.setWindowTitle("Remove Product "
                                            + QString::number(index + 1)
                                            + " of "
                                            + QString::number(postList.size()));
                removeDialog.setStandardButtons(QMessageBox::No | QMessageBox::Yes |
                                                QMessageBox::YesToAll |
                                                QMessageBox::NoToAll);
                removeDialog.setDefaultButton(QMessageBox::No);
                removeDialog.setText("Warehouse:" + p.warehouse.code
                                     + "\nSlot:" + p.bin.slot
                                     + "\nSKU:" + p.kit.sku
                                     + "\nDescription:" + p.kit.description
                                     + "\nAmount:" + QString::number(p.quantity)
                                     + "\nLast Modified:"
                                     + p.modified.toString(Qt::ISODate));
                selection = removeDialog.exec();
            } else {
                selection = QMessageBox::YesToAll;
            }

            switch (selection) {
            case QMessageBox::No:
                ui->statusBar->showMessage("Item " + p.kit.sku +
                                           " was Not Removed.");
                break;
            case QMessageBox::Yes:
                if (removeProduct(db, p)) {
                    updateWHSEView(ui->lineEdit_search->text());

                    ui->statusBar->showMessage("Item " + p.kit.sku +
                                               " was Removed.");
                }
                break;
            case QMessageBox::NoToAll:
                // DO NOT PASS GO, DO NOT COLLECT $200
                index = postList.size();
                break;
            case QMessageBox::YesToAll:
                yesToAll = true;
                if (removeProduct(db, p)) {
                    updateWHSEView(ui->lineEdit_search->text());

                    ui->statusBar->showMessage("Item " + p.kit.sku +
                                               " was Removed.");
                }
                break;
            default: break;
            }
        }
    } else {
        qDebug() << "Nothing selected";
    }
}

void MainWindow::on_actionChange_Item_triggered()
{
    // IF ITEM NOT SELECTED
    if(!product.addable()) {
        QMessageBox::about(this, "Edit Fail", "Select an item to edit from the\ntable below to use this function.");
        return;
    }

    UpdateDialog updateDialog(db, product, login, this);
    ui->statusBar->showMessage("Editing Item");

    if (updateDialog.exec() == QDialog::Accepted) {
        updateWHSEView(ui->lineEdit_search->text());
        ui->statusBar->showMessage("Item Updated Successfully.", 2000);
    } else {
        ui->statusBar->showMessage("Item not updated", 2000);
    }
}

void MainWindow::on_actionMove_Item_triggered()
{
    // IF ITEM NOT SELECTED
    if (!product.removable()) {
        QMessageBox::about(this, "Move Fail", "Select an item to remove from the \ntable below to use this function");
        return;
    }

    MoveDialog moveDialog(db, product, login, this);
    ui->statusBar->showMessage("Moving Item");

    if (moveDialog.exec() == QDialog::Accepted) {
        updateWHSEView(ui->lineEdit_search->text());

        ui->statusBar->showMessage("Item Moved Successfully.", 2000);
    } else {
        ui->statusBar->showMessage("Item was not moved", 2000);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QString version = QCoreApplication::applicationVersion();
    QMessageBox::about(this,"About","Inventory Manager v" + version + "\n\nCreated By Barron V Fritz");
}

void MainWindow::on_comboBox_search_mode_currentTextChanged(const QString &arg1)
{
    qDebug() << "Current Text Changed: " << arg1;

    searchMode = arg1;

    if (searchMode.isEmpty())
        qDebug() << "Why is the searchMode Empty?";

    if (completer)
        setCompleter(true);
}

void MainWindow::on_lineEdit_textEdited(const QString &search)
{
    updateINVView(search);
}

void MainWindow::on_actionAdmin_triggered()
{
    adminDialog = new AdminDialog(db, login, 0);
    adminDialog->show();
}

void MainWindow::on_actionLog_In_triggered()
{
    LoginDialog l(db, login, this);
    l.exec();

    updateSecurity(login);
}

void MainWindow::on_actionLog_Out_triggered()
{
    login = sLogin{};

    updateSecurity(login);
}

void MainWindow::on_lineEdit_search_returnPressed()
{
    ui->lineEdit_search->completer()->popup()->hide();
}

void MainWindow::on_actionCompletion_Popup_toggled(bool arg1)
{
    completer = arg1;
    if (!completer) {
        ui->lineEdit_search->setCompleter(0);
    } else {
        setCompleter(true);
    }
}
