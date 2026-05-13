#ifndef CHANNELSTRIP_H
#define CHANNELSTRIP_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QIcon>
#include "knobwidget.h"

class ChannelStrip : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelStrip(int channelId, QWidget *parent = nullptr);
    ~ChannelStrip();

    void updateVolume(int volume);
    void populateApps(const QStringList& apps);
    QString getAssignedApp() const;
    void setAssignedApp(const QString& appName);
    void setVisualMode(bool isKnob); // True for Knob, False for Slider
    void setAccentColor(const QColor& color);

signals:
    void appAssignmentChanged(int channelId, const QString& appName);

private:
    int m_channelId;
    QSlider* m_slider;
    KnobWidget* m_knob;
    QStackedWidget* m_visualStack;
    QLabel* m_volumeLabel;
    QComboBox* m_appDropdown;
    QLabel* m_channelNameLabel;
    QLabel* m_iconLabel;
    QLabel* m_speakerIcon;
    bool m_isKnobMode = false;

    void setupUi();
    void updateAppIcon();
    static QIcon fetchAppIcon(const QString& appName);
};

#endif // CHANNELSTRIP_H
