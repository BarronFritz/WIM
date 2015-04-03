#include "itemselectdialog.h"
#include "ui_itemselectdialog.h"

ItemSelectDialog::ItemSelectDialog(QSqlDatabase &db, Kit *resultKit, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ItemSelectDialog),
      db(db),
      item(resultKit)
{
    ui->setupUi(this);

    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    model = new QSqlQueryModel(this);

    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT sku,description FROM kit GROUP BY sku ORDER BY sku")) {
            if (query.exec()) {
                model->setQuery(query);
                ui->tableView->setModel(model);
                ui->tableView->show();
                ui->tableView->resizeColumnsToContents();
            } else {
                qDebug() << query.lastError().text();
            }
        } else {
            qDebug() << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
    }
}

ItemSelectDialog::~ItemSelectDialog()
{
    delete ui;
}

void ItemSelectDialog::update_tableView(QString arg1)
{
    arg1.prepend('%');
    arg1.append('%');

    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT sku,description FROM kit WHERE sku LIKE "
                          "? OR description LIKE ? "
                          "GROUP BY sku ORDER BY sku")) {
            query.bindValue(0,arg1);
            query.bindValue(1,arg1);
            if (query.exec()) {
                model->setQuery(query);
                ui->tableView->setModel(model);
                ui->tableView->show();
                ui->tableView->resizeColumnsToContents();
            } else {
                qDebug() << query.lastError().text();
            }
        } else {
            qDebug() << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
    }
}

void ItemSelectDialog::on_lineEdit_search_textEdited(const QString &arg1)
{
    update_tableView(arg1);
}

void ItemSelectDialog::on_tableView_clicked(const QModelIndex &index)
{
    selectedItem.sku = index.sibling(index.row(), 0).data().toString();
    selectedItem.description = index.sibling(index.row(), 1).data().toString();
    selectedItem.id = index.sibling(index.row(), 2).data().toInt();
}

void ItemSelectDialog::on_tableView_doubleClicked(const QModelIndex &)
{
    fillResult();
    accept();
}

void ItemSelectDialog::on_pushButton_ok_clicked()
{
    fillResult();
    accept();
}

void ItemSelectDialog::fillResult()
{
    item->sku = selectedItem.sku;
    item->description = selectedItem.description;
    item->id = selectedItem.id;
}
