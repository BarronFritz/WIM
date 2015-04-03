#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "newlogindialog.h"
#include "structures.h"
#include "utility.h"

#include <QDialog>
#include <QtSql>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QSqlDatabase &db, sLogin& mLogin, QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_pushButton_ok_clicked();

    void on_lineEdit_password_returnPressed();

    void on_pushButton_reset_clicked();

    void on_pushButton_new_clicked();

private:
    QString getPasswordFromTable(const QString &username);
    QString getSaltFromTable(const QString &username);
    int getSecurityLevelFromTable(const QString &username);

    bool verifyLogin();

    sLogin &login;

    QSqlDatabase db;

    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
