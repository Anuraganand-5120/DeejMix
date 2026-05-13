#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initializeDatabase();
    
    // Settings
    QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant());
    void setSetting(const QString& key, const QVariant& value);

    // Dynamic Profiles and Channels
    QString getChannelApp(int hardwareIndex);
    void setChannelApp(int hardwareIndex, const QString& appName);

private:
    QSqlDatabase m_db;
    bool createTables();
};

#endif // DATABASEMANAGER_H
