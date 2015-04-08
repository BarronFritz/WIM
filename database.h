#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include "structures.h"

namespace local {
struct columnValuePair {
    QString column;
    QString value;
};


bool query(QSqlDatabase &db, const QString &statement);
bool preparedQuery(QSqlDatabase &db, QSqlQuery &query);
}

namespace database {
QString getLastExecutedQuery(const QSqlQuery& query);
}

bool setupDatabaseTables(QSqlDatabase &db);
bool initDatabase(QSqlDatabase &db);

bool addProduct(QSqlDatabase &db, Product &product, sLogin login = sLogin());
bool removeFromSlot(QSqlDatabase &db, Product &product);
bool removeProduct(QSqlDatabase &db, Product &product);
bool editProduct(QSqlDatabase &db, Product &before, Product &after, sLogin login = sLogin());
bool moveProduct(QSqlDatabase &db, Product &source, Product &destination, sLogin login = sLogin());

bool updateKit(QSqlDatabase &db, Kit oldKit, Kit newKit);
bool addKit(QSqlDatabase &db, Kit kit);
bool removeKit(QSqlDatabase &db, const QString &kit_sku);

bool updateItem(QSqlDatabase &db, Item oldItem, Item newItem);
bool addItem(QSqlDatabase &db, Item item);
bool removeItem(QSqlDatabase &db, int item_id);

bool addItemToKit(QSqlDatabase &db, QString kit_sku, QString kit_description, int item_id, int quantity);
bool removeItemFromKit(QSqlDatabase &db, int kitIdToRemoveFrom);

bool addBin(QSqlDatabase &db, Warehouse warehouse, Bin bin);
bool removeBin(QSqlDatabase &db, int binID);

bool addWarehouse(QSqlDatabase &db, Warehouse warehouse);
bool removeWarehouse(QSqlDatabase &db, int whseID);

bool slotIsEmpty(QSqlDatabase &db, QString slot, int warehouseID);
bool slotExists(QSqlDatabase &db, QString slot, Warehouse warehouse);

bool addSlot(QSqlDatabase &db, QString slot, Warehouse warehouse);
bool removeSlot(QSqlDatabase &db, QString slot, Warehouse warehouse);
#endif // DATABASE_H
