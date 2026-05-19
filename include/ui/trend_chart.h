#ifndef TREND_CHART_H
#define TREND_CHART_H

#include <QWidget>
#include <QList>
#include <QColor>

class TrendChart : public QWidget {
    Q_OBJECT
public:
    explicit TrendChart(QWidget *parent = nullptr);
    explicit TrendChart(const QString &title, const QColor &lineColor,
                        QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setLineColor(const QColor &color);
    void addDataPoint(double value);
    void setMaxPoints(int maxPoints);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_title;
    QColor m_lineColor;
    QColor m_fillColor;
    QList<double> m_data;
    int m_maxPoints;
    double m_minValue;
    double m_maxValue;
};

#endif // TREND_CHART_H
