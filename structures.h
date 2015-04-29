#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QtSql>
#include <QDebug>
#include <QString>
#include <QList>
#include "utility.h"

///Program Operations

// Login Structure
// Used to pass security credentials to each part of the program
struct sLogin {
    QString username;
    int security;

    sLogin()
        : username{""}, security{0} {}
    sLogin(const sLogin& mLogin)
        : username(mLogin.username), security(mLogin.security) {}
    sLogin(QString mUsername, int mSecurity)
        : username(mUsername), security(mSecurity) {}

    QString usernameFull() {
        QString name;
        name = qgetenv("USER"); // get the user name in Linux
        if(name.isEmpty()) {
            name = qgetenv("USERNAME"); // get the name in Windows
        }
        if(name.isEmpty()) {
            name = QString("unknown"); // get the fallback name
        }
        return name + "_" + username;
    }
};

/// Inventory Structures

// Warehouse Struct representing the 'warehouse' database table
struct Warehouse {
    int id;
    QString code;
    QString name;
    QString address1;
    QString address2;
    QString city;
    QString state;
    QString zip;
    QString country;
};

// Structure representing 'item' database table.
struct Item {
    int id;
    QString sku;
    QString description;
};

// Structure representing 'unit' database table.
struct Unit {
    int id;
    QString code;
    Item item;
    int quantity;
};

// Structure representing 'kit' database table.
struct Kit {
    int id;
    QString sku;
    QString description;
    Item item;
    Unit unit;
    int quantity;
};

// Structure representing 'bin' database table.
struct Bin {
    int id;
    QString slot;
    Warehouse warehouse;
};

// Structure representing 'product' database table.
struct Product {
    int id;
    Warehouse warehouse;
    Bin bin;
    Kit kit;
    int quantity;
    QDate received;
    QDate packed;
    QDate expiration;
    QString notes;
    QDateTime modified{QDateTime::currentDateTime()};
    QString modified_by;
    bool addable(){
        if(warehouse.id > 0) {
            if (bin.id > 0) {
                if (quantity > -1) {
                    if (!kit.sku.isEmpty() ||
                            !notes.isEmpty()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    bool removable(){
        if (id > 0) {
            if (warehouse.id > 0) {
                if (!kit.sku.isEmpty() ||
                        !notes.isEmpty()) {
                    if (quantity > -1) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    QString toString(){
        return QString("ID: ") + QString::number(id) + QString("    WarehouseID: ")
                + QString::number(warehouse.id) + QString("    BinID: ") + QString::number(bin.id);
    }
};

struct Lot {
    int id;
    QString code;
    QDate production;
    QDate expiration;
    QString description;
};

#endif // STRUCTURES_H
