#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QVector>

class HardwareManager : public QObject
{
    Q_OBJECT
public:
    explicit HardwareManager(QObject *parent = nullptr);
    ~HardwareManager();

    void startAutoDetect();
    void disconnectDevice();
    bool isConnected() const;

signals:
    void hardwareConnected(const QString& portName);
    void hardwareDisconnected();
    void sliderValuesReceived(const QVector<int>& values);

private slots:
    void onReadyRead();
    void onPortError(QSerialPort::SerialPortError error);
    void attemptConnection();
    void connectionTimeout();

private:
    QSerialPort* m_serial;
    QTimer* m_scanTimer;
    QTimer* m_readTimer;
    QString m_buffer;
    int m_currentPortIndex;
    int m_currentBaudIndex;
    QVector<qint32> m_baudRates;
    bool m_isSniffing;
    bool m_isConnected;

    void processLine(const QString& line);
};

#endif // HARDWAREMANAGER_H
