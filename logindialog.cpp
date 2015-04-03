#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QSqlDatabase &db, sLogin &mLogin, QWidget *parent) :
    QDialog(parent),
    login(mLogin),
    ui(new Ui::LoginDialog)
{
    this->db = db;

    login.security = 0;
    login.username = "";

    ui->setupUi(this);

    // Check previous login settings.
    QSettings settings;
#ifdef DEBUG_MODE
    QString userName = "debug/userName";
#else
    QString userName = "userName";
#endif
    if(settings.contains(userName) &&
            !settings.value(userName).toString().isEmpty())
    {
        QString uname = settings.value(userName).toString();
        if (uname.isEmpty())
            ui->lineEdit_username->setText("Default");
        else
            ui->lineEdit_username->setText(uname);

        ui->lineEdit_password->setFocus();
        ui->lineEdit_password->selectAll();
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getPasswordFromTable(const QString &username) {
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT password FROM loginNEW WHERE username LIKE :name ")) {
            query.bindValue(":name", username);
            if (query.exec()) {
                if (query.first()) {
                    login.username = username;
                    return query.value(0).toString();
                } else {
                    qDebug() << "Record did not return any results";
                }
            } else {
                qDebug() << "Query could not execute?!";
                qDebug() << query.lastError().text();
            }
        } else {
            qDebug() << "Query could not be prepared";
            qDebug() << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "Couldn't Open Database";
        qDebug() << db.lastError().text();
    }
    return QString();
}

QString LoginDialog::getSaltFromTable(const QString &username) {
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT salt FROM loginNEW WHERE username LIKE :name ")) {
            query.bindValue(":name", username);
            if (query.exec()) {
                if (query.first()) {
                    login.username = username;
                    return query.value(0).toString();
                } else {
                    qDebug() << "Record did not return any results";
                }
            } else {
                qDebug() << "Query could not execute?!";
                qDebug() << query.lastError().text();
            }
        } else {
            qDebug() << "Query could not be prepared";
            qDebug() << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "Couldn't Open Database";
        qDebug() << db.lastError().text();
    }
    return QString();
}

int LoginDialog::getSecurityLevelFromTable(const QString &username) {
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT security FROM loginNEW WHERE username LIKE :name ")) {
            query.bindValue(":name", username);
            if (query.exec()) {
                if (query.first()) {
                    login.username = username;
                    return query.value(0).toInt();
                } else {
                    qDebug() << "Record did not return any results";
                }
            } else {
                qDebug() << "Query could not execute?!";
                qDebug() << query.lastError().text();
            }
        } else {
            qDebug() << "Query could not be prepared";
            qDebug() << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "Couldn't Open Database";
        qDebug() << db.lastError().text();
    }
    return 0;
}

bool LoginDialog::verifyLogin() {
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    if (username.isEmpty()) {
        qDebug() << "Enter username!";
        return false;
    }
    if (password.isEmpty()) {
        qDebug() << "Enter password!";
        return false;
    }

    auto loginInfo = security::getLoginInformation(db, username, password);

    if (!loginInfo.isValid()) {
        qDebug() << "Info is not valid";
        security::printLoginDetails(loginInfo);
        return false;
    }

    login.security = loginInfo.security;
    login.username = loginInfo.username;

    QSettings settings;
#ifdef DEBUG_MODE
    settings.setValue("debug/userName", ui->lineEdit_username->text());
#else
    settings.setValue("userName", ui->lineEdit_username->text());
#endif
    return true;
}

void LoginDialog::on_pushButton_ok_clicked()
{
    if(verifyLogin()) {
        this->accept();
    }
    else {
        QMessageBox::warning(this, "Login", "Could not verify username or "
                                            "password.\n\nNote: Password "
                                            "is CASE SENSITIVE.");
        ui->lineEdit_password->setFocus();
        ui->lineEdit_password->selectAll();
    }
}

void LoginDialog::on_lineEdit_password_returnPressed()
{
    if(!ui->lineEdit_password->text().isEmpty() &&
            !ui->lineEdit_username->text().isEmpty()) {
        ui->pushButton_ok->click();
    }
}

void LoginDialog::on_pushButton_reset_clicked()
{
    NewLoginDialog resetDialog(db, true, this);
    resetDialog.exec();
}

void LoginDialog::on_pushButton_new_clicked()
{
    NewLoginDialog newDialog(db, false, this);
    newDialog.exec();
}
