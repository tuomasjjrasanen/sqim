#ifndef DATABASE_HH
#define DATABASE_HH

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QtSql>

#include "database.hh"

static void openDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::homePath() + "/.qpicman/db.sqlite3");
    if (!db.open()) {
        qCritical() << "failed to open sqlite3 database file" << db.databaseName();
        exit(1);
    }

    QSqlQuery query;
    if (!query.exec("PRAGMA foreign_keys = ON;")) {
        qCritical() << "failed to enable foreign key support";
        exit(1);
    }
}

static void createTables()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query;

    if (db.tables().contains("image")
        && db.tables().contains("tag")) {
        return;
    }

    db.transaction();

    query.prepare("CREATE TABLE image("
                  "  filepath TEXT PRIMARY KEY,"
                  "  thumbnail BLOB NOT NULL,"
                  "  mtime INTEGER NOT NULL);");
    if (query.lastError().isValid())
        goto err;

    query.exec();
    if (query.lastError().isValid())
        goto err;

    query.prepare("CREATE TABLE tag("
                  "  image_filepath TEXT NOT NULL,"
                  "  name TEXT NOT NULL,"
                  "  PRIMARY KEY(image_filepath, name),"
                  "  FOREIGN KEY(image_filepath) REFERENCES image(filepath));");
    if (query.lastError().isValid())
        goto err;

    query.exec();
    if (query.lastError().isValid())
        goto err;

    db.commit();
    return;

err:
    qCritical() << query.lastError();
    db.rollback();
    exit(1);
}

void SQIM::init()
{
    openDatabase();
    createTables();
}

#endif // DATABASE_HH
