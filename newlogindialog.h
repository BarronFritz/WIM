#ifndef NEWLOGINDIALOG_H
#define NEWLOGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QByteArray>

#include "structures.h"
#include "utility.h"

namespace Ui {
class NewLoginDialog;
}

class NewLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewLoginDialog(QSqlDatabase &db, bool reset, QWidget *parent = 0);
    ~NewLoginDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    bool reset{false};
    sLogin login;
    QSqlDatabase db;
    Ui::NewLoginDialog *ui;

    bool passwordsMatch(const QString &p1, const QString &p2);
};

#endif // NEWLOGINDIALOG_H
