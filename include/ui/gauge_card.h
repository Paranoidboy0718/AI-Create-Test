#ifndef GAUGE_CARD_H
#define GAUGE_CARD_H

#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include <QVariantMap>

class GaugeCard : public QFrame {
    Q_OBJECT
public:
    explicit GaugeCard(QWidget *parent = nullptr);
    explicit GaugeCard(const QString &title, const QString &unit,
                       const QColor &accentColor, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setUnit(const QString &unit);
    void setAccentColor(const QColor &color);
    void updateValue(double value, const QString &text);
    void updateValue(const QString &text);

private:
    void initUI();

    QLabel *m_titleLabel;
    QLabel *m_valueLabel;
    QLabel *m_unitLabel;
    QProgressBar *m_progressBar;
    QString m_title;
    QString m_unit;
    QColor m_accentColor;
};

#endif // GAUGE_CARD_H
