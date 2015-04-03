#ifndef ITEMSELECTDIALOG_H
#define ITEMSELECTDIALOG_H

#include <QtSql>
#include <QDialog>
#include "structures.h"

namespace Ui {
class ItemSelectDialog;
}

class ItemSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemSelectDialog(QSqlDatabase &db, Kit *resultKit, QWidget *parent = 0);
    ~ItemSelectDialog();

public slots:
    void update_tableView(QString arg1);

private slots:
    void on_lineEdit_search_textEdited(const QString &arg1);

    void on_tableView_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_ok_clicked();

private:
    Ui::ItemSelectDialog *ui;

    QSqlDatabase db;
    QSqlQueryModel *model;

    Kit *item;
    Kit selectedItem;

    void fillResult();
};

#endif // ITEMSELECTDIALOG_H
