#include "ui/trend_chart.h"
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

TrendChart::TrendChart(QWidget *parent)
    : QWidget(parent), m_title("Chart"), m_lineColor(Qt::white), m_maxPoints(200)
{
    m_fillColor = m_lineColor;
    m_fillColor.setAlpha(40);
    m_minValue = 0.0;
    m_maxValue = 100.0;
    setMinimumHeight(120);
    setStyleSheet("background-color: #16213e; border-radius: 8px;");
}

TrendChart::TrendChart(const QString &title, const QColor &lineColor,
                       QWidget *parent)
    : QWidget(parent), m_title(title), m_lineColor(lineColor), m_maxPoints(200)
{
    m_fillColor = lineColor;
    m_fillColor.setAlpha(40);
    m_minValue = 0.0;
    m_maxValue = 100.0;
    setMinimumHeight(120);
    setStyleSheet("background-color: #16213e; border-radius: 8px;");
}

void TrendChart::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void TrendChart::setLineColor(const QColor &color)
{
    m_lineColor = color;
    m_fillColor = color;
    m_fillColor.setAlpha(40);
    update();
}

void TrendChart::addDataPoint(double value)
{
    m_data.append(value);
    if (m_data.size() > m_maxPoints) {
        m_data.removeFirst();
    }

    if (!m_data.isEmpty()) {
        m_minValue = *std::min_element(m_data.begin(), m_data.end());
        m_maxValue = *std::max_element(m_data.begin(), m_data.end());
        double range = m_maxValue - m_minValue;
        if (range < 1.0) {
            m_minValue -= 5.0;
            m_maxValue += 5.0;
        } else {
            m_minValue -= range * 0.1;
            m_maxValue += range * 0.1;
        }
        if (m_minValue < 0) m_minValue = 0;
    }

    update();
}

void TrendChart::setMaxPoints(int maxPoints)
{
    m_maxPoints = maxPoints;
    while (m_data.size() > m_maxPoints) {
        m_data.removeFirst();
    }
}

void TrendChart::clear()
{
    m_data.clear();
    update();
}

void TrendChart::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int margin = 10;
    int plotW = w - 2 * margin;
    int plotH = h - 2 * margin - 15;

    painter.setPen(QColor("#888"));
    painter.setFont(QFont("Arial", 9));
    painter.drawText(margin, margin, m_title);

    if (m_data.isEmpty()) {
        painter.setPen(QColor("#555"));
        painter.drawText(rect(), Qt::AlignCenter, "等待数据...");
        return;
    }

    double range = m_maxValue - m_minValue;
    if (range <= 0) range = 1.0;

    painter.setPen(QPen(QColor("#0f3460"), 1));
    for (int i = 1; i <= 4; ++i) {
        int y = margin + 15 + plotH * i / 5;
        painter.drawLine(margin, y, w - margin, y);
    }

    QPainterPath path;
    int dataCount = m_data.size();
    for (int i = 0; i < dataCount; ++i) {
        double x = margin + static_cast<double>(i) / (dataCount - 1) * plotW;
        double normalized = (m_data[i] - m_minValue) / range;
        double y = margin + 15 + plotH * (1.0 - normalized);
        if (i == 0) path.moveTo(x, y);
        else path.lineTo(x, y);
    }

    painter.setPen(QPen(m_lineColor, 2));
    painter.setBrush(m_fillColor);
    painter.drawPath(path);

    painter.setPen(m_lineColor);
    painter.setFont(QFont("Arial", 8));
    painter.drawText(margin, h - 5,
        QString::number(m_data.last(), 'f', 1));
}
