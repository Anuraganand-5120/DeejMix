#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initializeDatabase()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dataPath + "/deejmix.db");

    if (!m_db.open()) {
        qDebug() << "Error: connection with database fail";
        return false;
    }

    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;
    bool success = true;

    // Create Settings table
    success &= query.exec("CREATE TABLE IF NOT EXISTS settings ("
                          "key TEXT PRIMARY KEY,"
                          "value TEXT)");

    // Create Profiles table
    success &= query.exec("CREATE TABLE IF NOT EXISTS profiles ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "name TEXT,"
                          "is_active INTEGER)");

    // Create Channels table (Dynamic mapping)
    success &= query.exec("CREATE TABLE IF NOT EXISTS channels ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "profile_id INTEGER,"
                          "hardware_index INTEGER,"
                          "assigned_apps TEXT,"
                          "color TEXT,"
                          "invert INTEGER DEFAULT 0,"
                          "min_range INTEGER DEFAULT 0,"
                          "max_range INTEGER DEFAULT 1023,"
                          "sensitivity INTEGER DEFAULT 1,"
                          "FOREIGN KEY(profile_id) REFERENCES profiles(id))");

    if (!success) {
        qDebug() << "Failed to create tables:" << query.lastError().text();
    }
    return success;
}

QVariant DatabaseManager::getSetting(const QString& key, const QVariant& defaultValue)
{
    QSqlQuery query;
    query.prepare("SELECT value FROM settings WHERE key = :key");
    query.bindValue(":key", key);
    if (query.exec() && query.next()) {
        QVariant val = query.value(0);
        if (val.type() == QVariant::String) {
            QString str = val.toString().toLower();
            if (str == "true" || str == "1") return true;
            if (str == "false" || str == "0") return false;
        }
        return val;
    }
    return defaultValue;
}

void DatabaseManager::setSetting(const QString& key, const QVariant& value)
{
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO settings (key, value) VALUES (:key, :value)");
    query.bindValue(":key", key);
    // Let SQLite store it in its native type (INTEGER for bool) to avoid string casting issues
    query.bindValue(":value", value);
    query.exec();
}

QString DatabaseManager::getChannelApp(int hardwareIndex)
{
    QSqlQuery query;
    query.prepare("SELECT assigned_apps FROM channels WHERE hardware_index = :idx");
    query.bindValue(":idx", hardwareIndex);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

void DatabaseManager::setChannelApp(int hardwareIndex, const QString& appName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM channels WHERE hardware_index = :idx");
    query.bindValue(":idx", hardwareIndex);
    
    if (query.exec() && query.next()) {
        // Exists, update
        QSqlQuery update;
        update.prepare("UPDATE channels SET assigned_apps = :app WHERE hardware_index = :idx");
        update.bindValue(":app", appName);
        update.bindValue(":idx", hardwareIndex);
        update.exec();
    } else {
        // Does not exist, insert
        QSqlQuery insert;
        insert.prepare("INSERT INTO channels (hardware_index, assigned_apps) VALUES (:idx, :app)");
        insert.bindValue(":idx", hardwareIndex);
        insert.bindValue(":app", appName);
        insert.exec();
    }
}
