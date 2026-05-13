#ifndef KNOBWIDGET_H
#define KNOBWIDGET_H

#include <QWidget>
#include <QColor>

class KnobWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KnobWidget(QWidget *parent = nullptr);

    void setValue(int value); // 0-100
    int value() const;

    void setAccentColor(const QColor& color);
    QColor accentColor() const;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_value = 0;
    QColor m_accentColor{0x3B, 0x82, 0xF6}; // Default blue

    static constexpr int NUM_TICKS = 30;
    static constexpr float START_ANGLE = 225.0f;
    static constexpr float SWEEP_ANGLE = 270.0f;
};

#endif // KNOBWIDGET_H
