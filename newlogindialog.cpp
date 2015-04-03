#include "newlogindialog.h"
#include "ui_newlogindialog.h"

constexpr int PASSWORD_MINIMUM_LENGTH = 3;

NewLoginDialog::NewLoginDialog(QSqlDatabase &db, bool reset, QWidget *parent)
    : QDialog(parent),
    reset(reset),
    ui(new Ui::NewLoginDialog)
{
    this->db = db;
    ui->setupUi(this);
}

NewLoginDialog::~NewLoginDialog()
{
    delete ui;
}
bool NewLoginDialog::passwordsMatch(const QString &p1, const QString &p2) {
    return (
                ( p1.length() > PASSWORD_MINIMUM_LENGTH )
                && (p1 == p2)
           );
}

void NewLoginDialog::on_buttonBox_accepted()
{
    QString username = ui->lineEdit_username->text();
    QString password1 = ui->lineEdit_password_1->text();
    QString password2 = ui->lineEdit_password_2->text();
    if (!passwordsMatch(password1, password2)) {
        QMessageBox::warning(this, "Password Mismatch",
                             "The passwords do not match. Please re-enter.");
        ui->lineEdit_password_1->clear();
        ui->lineEdit_password_2->clear();
        ui->lineEdit_password_1->setFocus();
        return;
    }

    security::SecLogin secLogin =
            security::getLoginInformation(db, username, password1);
    if (reset && secLogin.saltHash.isEmpty()) {
        // Changing password
        security::resetPasswordMarkedForReset(db, username, password1);
        qDebug() << "Password Reset";
        this->accept();

    } else if (!reset && secLogin == security::SecLogin()) {
        // Add user to db
        security::insertUserIntoDatabase(db, username, password1, 1);
        qDebug() << "New User Inserted";
        this->accept();
    }
}
void NewLoginDialog::on_buttonBox_rejected()
{
    this->reject();
}
