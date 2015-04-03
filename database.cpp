#include "database.h"
#include <QMessageBox>

QString database::getLastExecutedQuery(const QSqlQuery& query)
{
     QString str = query.lastQuery();
     QMapIterator<QString, QVariant> it(query.boundValues());
     while (it.hasNext())
     {
          it.next();
          str.replace(it.key(),it.value().toString());
     }
     return str;
}
bool local::query(QSqlDatabase &db, const QString &statement)
{
    if(!db.open())
    {
        qDebug() << db.lastError().text();
        return false;
    }
    else
    {
        QSqlQuery query;
        if(!query.exec(statement))
        {
            qDebug() << query.lastError().text();
            return false;
        }
        else
        {
            db.close();
            return true;
        }
    }
}

bool local::preparedQuery(QSqlDatabase &db, QSqlQuery &query)
{
    if(!db.open())
    {
        qDebug() << db.lastError().text();
        return false;
    }
    else
    {
        if(!query.exec())
        {
            qDebug() << query.lastError().text();
            return false;
        }
        else
        {
            db.close();
            return true;
        }
    }
}

bool initDatabase(QSqlDatabase &db) {
    // All this function does is set Foreign Key restraints to active.

    qDebug() << "TODO(barron): Add 'Create table if not exists' for initDatabase()";

    if (db.open()) {
        QSqlQuery query;
        if (query.exec("PRAGMA foreign_keys (true)"))
            return true;
    }
    return false;
}

bool addProduct(QSqlDatabase &db, Product &product, sLogin login)
{
    if(!product.addable()) {
        qDebug() << "Debug: Product Not Addable";
        return false;
    }

    bool result{false};

    if(db.open()) {
        QSqlQuery query;
        // If no product exists, insert into the database
        if(!query.prepare("INSERT INTO product (warehouse_id, bin_id, kit_sku, "
                          "quantity, received, packed, expiration, "
                          "modified, modified_by, notes) VALUES ( "
                          " :warehouse_id , :bin_id , :kit_sku , :quantity , "
                          " :received , :packed , :expiration , :modified , "
                          " :username , :notes )" )) {
            qDebug() << "Prepare Failed";
            qDebug() << query.lastError();
        } else {
            query.bindValue(":warehouse_id", product.warehouse.id);
            query.bindValue(":bin_id", product.bin.id);
            query.bindValue(":kit_sku", product.kit.sku);
            query.bindValue(":quantity", product.quantity);
            query.bindValue(":received", product.received);
            query.bindValue(":packed", product.packed);
            query.bindValue(":expiration", product.expiration);
            query.bindValue(":modified", product.modified);
            query.bindValue(":username", login.usernameFull());
            query.bindValue(":notes", product.notes);

            if(query.exec()) {
                result = true;
                // Logging is now done thru triggers in the database
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        }
       db.close();
    } else {
        qDebug() << "There's nothing the urks me more than cheap, low profile "
                    "dome actuated, wireless keyboards, whose right shift key "
                    "rarely-if-ever works on the first press...";
        qDebug() << "Also...db didn't open...";
    }
    return result;
}

bool removeFromSlot(QSqlDatabase &db, Product &product)
{
    bool result{false};

    if (product.warehouse.id > 0 &&
            product.bin.id > 0) {
        if (db.open()) {
            QSqlQuery query;
            if (query.prepare("DELETE FROM product WHERE bin_id = :binID "
                              "AND warehouse_ID = :whseID")) {
                query.bindValue(":binID", product.bin.id);
                query.bindValue(":whseID", product.warehouse.id);
                if (query.exec()) {
                    result = true;
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
            db.close();
        } else {
            qDebug() << db.lastError().text();
            qDebug() << "Failed to open database.";
        }
    }
    return result;
}

bool removeProduct(QSqlDatabase &db, Product &product)
{
    if(!product.removable()) {
        return false;
    }

    bool result{false};

    if(db.open()) {
        QSqlQuery query;
        if(!query.prepare("DELETE FROM product WHERE id = :product_id ")) {
            qDebug() << "Prepare Failed";
        } else {
            query.bindValue(":product_id", product.id);
            // We only need to check the unique product ID to remove
       }
        if(query.exec()) {
            result = true;
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    }
    return result;
}

bool editProduct(QSqlDatabase &db, Product &before, Product &after, sLogin login)
{
    bool result{false};

    if(!before.removable())
    {
        qDebug() << "oldProduct not removeable";
        qDebug() << before.toString();
        return false;
    } else {
        if(!after.addable()) {
            qDebug() << "newProduct not addable";
            qDebug() << after.toString();
            return false;
        } else {
            if(db.open()) {
                QSqlQuery query;
                if(query.prepare("UPDATE product SET kit_sku = :newSku, "
                                 "quantity = :newQuantity, "
                                 "received = :newReceived, "
                                 "packed = :newPacked, "
                                 "expiration = :newExpiration, "
                                 "modified = datetime('now','localtime'), "
                                 "modified_by = :newUsername, "
                                 "notes = :newNotes "
                                 "WHERE id = :productID ")) {
                    query.bindValue(":newSku", after.kit.sku);
                    query.bindValue(":newQuantity", after.quantity);
                    query.bindValue(":newReceived", after.received);
                    query.bindValue(":newPacked", after.packed);
                    query.bindValue(":newExpiration", after.expiration);
                    query.bindValue(":newUsername", login.usernameFull());
                    query.bindValue(":newNotes", after.notes);
                    query.bindValue(":productID", before.id);

                    if(query.exec()) {
                        result = true;
                        // Logging should be automatically triggered
                        // in the database after executing the update query.
                    } else {
                        qDebug() << query.lastError().text();
                        qDebug() << database::getLastExecutedQuery(query);
                    }
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << "Failed to prepare query.";
                }
                db.close();
            } else {
                qDebug() << db.lastError().text();
                qDebug() << "Failed to open database.";
            }
        }
    }
    return result;
}

bool moveProduct(QSqlDatabase &db, Product &source, Product &destination, sLogin login)
{
    bool result{false};
    if(!source.removable()) {
        qDebug() << "oldProduct not removeable";
        return false;
    } else {
        if(!destination.addable()) {
           qDebug() << "newProduct not addable";
           return false;
        } else {
            if(db.open()) {
                QSqlQuery query;
                if(query.prepare("UPDATE product SET bin_id = :binID, "
                                 "modified = datetime('now','localtime'), "
                                 "modified_by = :newUsername "
                                 "WHERE id = :productID")) {
                    query.bindValue(":binID", destination.bin.id);
                    query.bindValue(":newUsername", login.usernameFull());
                    query.bindValue(":productID", source.id);

                    if(query.exec()) {
                    result = true;
                    // Logging should be automatically triggered
                    // in the database after executing the update query.
                    } else {
                        qDebug() << query.lastError().text();
                        qDebug() << database::getLastExecutedQuery(query);
                    }
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << "Failed to prepare query.";
                }
                db.close();
            } else {
                qDebug() << db.lastError().text();
                qDebug() << "Failed to open database.";
            }
        }
    }
    return result;
}

bool updateKit(QSqlDatabase &db, Kit oldKit, Kit newKit)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("UPDATE kit SET sku = :newSKU , "
                          "description = :newDesc WHERE sku = :oldSKU")) {
            query.bindValue(":newSKU", newKit.sku);
            query.bindValue(":newDesc", newKit.description);
            query.bindValue(":oldSKU", oldKit.sku);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool addKit(QSqlDatabase &db, Kit kit)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO kit (sku, description) "
                          "VALUES ( :sku , :desc )")) {
            query.bindValue(":sku", kit.sku);
            query.bindValue(":desc", kit.description);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool removeKit(QSqlDatabase &db, const QString &kit_sku)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM kit "
                          "WHERE sku LIKE '%" + kit_sku + "%'")) {
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }

    return result;
}

bool updateItem(QSqlDatabase &db, Item oldItem, Item newItem)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("UPDATE item SET sku = :newSKU , "
                          "description = :newDesc WHERE id = :oldID")) {
            query.bindValue(":newSKU", newItem.sku);
            query.bindValue(":newDesc", newItem.description);
            query.bindValue(":oldID", oldItem.id);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool addItem(QSqlDatabase &db, Item item)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO item (sku, description) "
                          "VALUES ( :sku , :desc )")) {
            query.bindValue(":sku", item.sku);
            query.bindValue(":desc", item.description);
            if (query.exec()) {
                if (query.prepare("INSERT INTO unit (code, item_id, quantity) "
                                  "VALUES ( :code , (SELECT id FROM item WHERE "
                                  "sku = :itemSKU ) , :qty ) ")) {
                    query.bindValue(":code", QString("EACH"));
                    query.bindValue(":itemSKU", item.sku);
                    query.bindValue(":qty", 1);
                    if (query.exec()) {
                        result = true;
                    } else {
                        qDebug() << query.lastError().text();
                        qDebug() << database::getLastExecutedQuery(query);
                    }
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool removeItem(QSqlDatabase &db, int item_id)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM item WHERE id = :itemID")) {
            query.bindValue(":itemID", item_id);
            if (query.exec()) {
                if (query.prepare("DELETE FROM unit WHERE item_id = :itemID")) {
                    query.bindValue(":itemID", item_id);
                    if (query.exec()) {
                        result = true;
                    } else {
                        qDebug() << query.lastError().text();
                        qDebug() << database::getLastExecutedQuery(query);
                    }
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }

    return result;
}

bool addItemToKit(QSqlDatabase &db, QString kit_sku, QString kit_description,
                  int item_id, int quantity)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO kit (sku, description, item_id, "
                          "unit_id, quantity) VALUES ( "
                          ":kitSku , :kitDescription , :itemID , (SELECT id "
                          "FROM unit WHERE item_id = :unitItemID ) , "
                          ":qty )")) {
            query.bindValue(":kitSku", kit_sku);
            query.bindValue(":kitDescription", kit_description);
            query.bindValue(":itemID", item_id);
            query.bindValue(":unitItemID", item_id);
            query.bindValue(":qty", quantity);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
 }

bool removeItemFromKit(QSqlDatabase &db, int kitIdToRemove)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM kit WHERE "
                          "id = :kitID")) {
            query.bindValue(":kitID", kitIdToRemove);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool addBin(QSqlDatabase &db, Warehouse warehouse, Bin bin)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO bin(slot, warehouse_id) "
                          "VALUES( :slot , :whseID )")) {
            query.bindValue(":slot", bin.slot);
            query.bindValue(":whseID", warehouse.id);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool removeBin(QSqlDatabase &db, int binID)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM bin WHERE id = :binID")) {
            query.bindValue(":binID", binID);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool addWarehouse(QSqlDatabase &db, Warehouse warehouse)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO warehouse(code, name, address1, "
                          "address2, city, state, zip, country) VALUES( "
                          ":co , :na , :a1 , :a2 , :ci , :st , :zi , :cy )")) {
            query.bindValue(":co", warehouse.code);
            query.bindValue(":na", warehouse.name);
            query.bindValue(":a1", warehouse.address1);
            query.bindValue(":a2", warehouse.address2);
            query.bindValue(":ci", warehouse.city);
            query.bindValue(":st", warehouse.state);
            query.bindValue(":zi", warehouse.zip);
            query.bindValue(":cy", warehouse.country);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool removeWarehouse(QSqlDatabase &db, int whseID)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM warehouse WHERE id = :whseID")) {
            query.bindValue(":whseID", whseID);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool slotIsEmpty(QSqlDatabase &db, QString slot, int warehouseID)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT * FROM product WHERE bin_id IN (SELECT id "
                          "FROM bin WHERE slot = :slot AND warehouse_id = "
                          ":binWhseID ) AND warehouse_id = :prodWhseID")) {
            query.bindValue(":slot", slot);
            query.bindValue(":binWhseID", warehouseID);
            query.bindValue(":prodWhseID", warehouseID);
            if (query.exec()) {
                int count = 0;
                while (query.next()) {
                    count++;
                }
                if (count > 0) {
                    // Product exists in this slot. Cannot remove.
                    result = false;
                } else {
                    // Slot is empty
                    result = true;
                }
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool slotExists(QSqlDatabase &db, QString slot, Warehouse warehouse)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("SELECT * FROM bin WHERE slot = :slot AND "
                          "warehouse_id = :whseID")) {
            query.bindValue(":slot", slot);
            query.bindValue(":whseID", warehouse.id);
            if (query.exec()) {
                if (query.first()) {
                    if (!query.value(0).toString().isEmpty()) {
                        // Slot exists.
                        result = true;
                    } else {
                        // Slot does not exist
                        result = false;
                    }
                } else {
                    qDebug() << query.lastError().text();
                    qDebug() << database::getLastExecutedQuery(query);
                }
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool addSlot(QSqlDatabase &db, QString slot, Warehouse warehouse)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("INSERT INTO bin(slot, warehouse_id) "
                          "VALUES ( :slot , :whseID )")) {
            query.bindValue(":slot", slot);
            query.bindValue(":whseID", warehouse.id);
            if (query.exec()) {
                result = true;
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}

bool removeSlot(QSqlDatabase &db, QString slot, Warehouse warehouse)
{
    bool result{false};
    if (db.open()) {
        QSqlQuery query;
        if (query.prepare("DELETE FROM bin WHERE slot = :slot AND warehouse_id "
                          "= :whseID")) {
            query.bindValue(":slot", slot);
            query.bindValue(":whseID", warehouse.id);
            if (query.exec()) {
                result = true;
            } else {
                qDebug() << query.lastError().text();
                qDebug() << database::getLastExecutedQuery(query);
            }
        } else {
            qDebug() << query.lastError().text();
            qDebug() << database::getLastExecutedQuery(query);
        }
        db.close();
    } else {
        qDebug() << db.lastError().text();
        qDebug() << "Failed to open database.";
    }
    return result;
}
