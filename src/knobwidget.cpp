#include "knobwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

KnobWidget::KnobWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(100, 100);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void KnobWidget::setValue(int value)
{
    value = qBound(0, value, 100);
    if (m_value != value) {
        m_value = value;
        update();
    }
}

int KnobWidget::value() const { return m_value; }

void KnobWidget::setAccentColor(const QColor& color)
{
    if (m_accentColor != color) {
        m_accentColor = color;
        update();
    }
}

QColor KnobWidget::accentColor() const { return m_accentColor; }

QSize KnobWidget::minimumSizeHint() const { return QSize(100, 100); }
QSize KnobWidget::sizeHint() const { return QSize(130, 130); }

void KnobWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    int side = qMin(width(), height());
    float cx = width() / 2.0f;
    float cy = height() / 2.0f;

    float knobOuterRadius = side * 0.36f;
    float ledArcRadius    = side * 0.42f;
    float tickInner       = side * 0.46f;
    float tickOuter       = side * 0.50f;
    float indicatorRadius = side * 0.28f;

    float litFraction = m_value / 100.0f;
    int startA16 = (int)(START_ANGLE * 16);
    int spanA16  = -(int)(litFraction * SWEEP_ANGLE * 16);

    // ── Outer Tick marks (Scale) ──
    for (int i = 0; i < NUM_TICKS; ++i) {
        float t = (float)i / (NUM_TICKS - 1);
        float angleDeg = START_ANGLE - t * SWEEP_ANGLE;
        float angleRad = qDegreesToRadians(angleDeg);
        float x1 = cx + tickInner * cosf(angleRad);
        float y1 = cy - tickInner * sinf(angleRad);
        float x2 = cx + tickOuter * cosf(angleRad);
        float y2 = cy - tickOuter * sinf(angleRad);
        p.setPen(QPen(QColor(60, 65, 80), 1.5f, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    // ── LED Arc ──
    QRectF arcRect(cx - ledArcRadius, cy - ledArcRadius, ledArcRadius * 2, ledArcRadius * 2);
    
    // Background arc for the unlit portion
    if (litFraction < 1.0f) {
        float unlitSpanA16 = -(int)((1.0f - litFraction) * SWEEP_ANGLE * 16);
        float unlitStartA16 = startA16 + spanA16;
        p.setPen(QPen(QColor(20, 25, 35), side * 0.012f, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(arcRect, unlitStartA16, unlitSpanA16);
    }

    // Lit LED Arc
    if (litFraction > 0.0f) {
        // Soft Glow
        QColor glow = m_accentColor;
        glow.setAlpha(80);
        p.setPen(QPen(glow, side * 0.035f, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(arcRect, startA16, spanA16);

        // Solid inner core
        glow.setAlpha(255);
        p.setPen(QPen(glow, side * 0.012f, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(arcRect, startA16, spanA16);
    }

    // ── Knob body (Deep shadows & reflections) ──
    // Outer Metallic Ring Base
    QRadialGradient ring(cx, cy - knobOuterRadius * 0.5f, knobOuterRadius * 1.5f);
    ring.setColorAt(0.0, QColor(50, 55, 65));
    ring.setColorAt(0.5, QColor(15, 18, 25));
    ring.setColorAt(1.0, QColor(5, 7, 10));
    p.setPen(QPen(QColor(10, 12, 18), 1.5f));
    p.setBrush(ring);
    p.drawEllipse(QPointF(cx, cy), knobOuterRadius, knobOuterRadius);

    // Inner glossy center cap
    float knobInnerRadius = side * 0.32f;
    QRadialGradient innerCap(cx, cy + knobInnerRadius * 0.5f, knobInnerRadius * 1.5f);
    innerCap.setColorAt(0.0, QColor(30, 35, 45));
    innerCap.setColorAt(1.0, QColor(5, 5, 8));
    p.setPen(QPen(QColor(0, 0, 0), 1.0f));
    p.setBrush(innerCap);
    p.drawEllipse(QPointF(cx, cy), knobInnerRadius, knobInnerRadius);

    // Center highlight to make it look mechanical/convex
    float capR = side * 0.16f;
    QRadialGradient centerCap(cx - capR * 0.2f, cy - capR * 0.4f, capR * 1.2f);
    centerCap.setColorAt(0.0, QColor(60, 65, 75, 150));
    centerCap.setColorAt(0.5, QColor(25, 28, 35, 100));
    centerCap.setColorAt(1.0, QColor(15, 18, 22, 0));
    p.setPen(Qt::NoPen);
    p.setBrush(centerCap);
    p.drawEllipse(QPointF(cx, cy), capR, capR);

    // ── Indicator dot ──
    float angleDeg = START_ANGLE - litFraction * SWEEP_ANGLE;
    float angleRad = qDegreesToRadians(angleDeg);

    float ix = cx + indicatorRadius * cosf(angleRad);
    float iy = cy - indicatorRadius * sinf(angleRad);

    p.setBrush(m_accentColor);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(ix, iy), side * 0.025f, side * 0.025f); // Glow dot
    
    p.setBrush(m_accentColor.lighter(150));
    p.drawEllipse(QPointF(ix, iy), side * 0.012f, side * 0.012f); // Bright core
}
