#include "ui/gauge_card.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

GaugeCard::GaugeCard(QWidget *parent)
    : QFrame(parent), m_accentColor(QColor("#53d8fb"))
{
    initUI();
    m_unitLabel->hide();

    m_titleLabel->setStyleSheet("color: #888; font-size: 11px;");
    m_valueLabel->setStyleSheet("color: #53d8fb; font-size: 22px; font-weight: bold;");
    m_progressBar->setStyleSheet(
        "QProgressBar { background-color: #0f3460; border: none; border-radius: 2px; }"
        "QProgressBar::chunk { background-color: #53d8fb; border-radius: 2px; }"
    );
}

GaugeCard::GaugeCard(const QString &title, const QString &unit,
                     const QColor &accentColor, QWidget *parent)
    : QFrame(parent), m_title(title), m_unit(unit), m_accentColor(accentColor)
{
    initUI();

    m_titleLabel->setText(m_title);
    m_unitLabel->setText(m_unit);
    m_valueLabel->setStyleSheet(
        QString("color: %1; font-size: 22px; font-weight: bold;")
        .arg(m_accentColor.name()));
    m_progressBar->setStyleSheet(
        QString(
            "QProgressBar { background-color: #0f3460; border: none; border-radius: 2px; }"
            "QProgressBar::chunk { background-color: %1; border-radius: 2px; }"
        ).arg(m_accentColor.name()));
}

void GaugeCard::initUI()
{
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet(
        "QFrame { background-color: #1a1a2e; border: 1px solid #16213e; border-radius: 8px; padding: 10px; }"
    );
    setMinimumSize(140, 90);

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(10, 8, 10, 8);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet("color: #888; font-size: 11px;");
    layout->addWidget(m_titleLabel);

    auto *valueRow = new QHBoxLayout();
    m_valueLabel = new QLabel("--", this);
    m_valueLabel->setStyleSheet("font-size: 22px; font-weight: bold;");
    valueRow->addWidget(m_valueLabel);

    m_unitLabel = new QLabel(this);
    m_unitLabel->setStyleSheet("color: #666; font-size: 11px; padding-top: 8px;");
    valueRow->addWidget(m_unitLabel);
    valueRow->addStretch();
    layout->addLayout(valueRow);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(4);
    layout->addWidget(m_progressBar);
}

void GaugeCard::setTitle(const QString &title)
{
    m_title = title;
    m_titleLabel->setText(title);
}

void GaugeCard::setUnit(const QString &unit)
{
    m_unit = unit;
    m_unitLabel->setText(unit);
    m_unitLabel->show();
}

void GaugeCard::setAccentColor(const QColor &color)
{
    m_accentColor = color;
    m_valueLabel->setStyleSheet(
        QString("color: %1; font-size: 22px; font-weight: bold;")
        .arg(m_accentColor.name()));
    m_progressBar->setStyleSheet(
        QString(
            "QProgressBar { background-color: #0f3460; border: none; border-radius: 2px; }"
            "QProgressBar::chunk { background-color: %1; border-radius: 2px; }"
        ).arg(m_accentColor.name()));
}

void GaugeCard::updateValue(double value, const QString &text)
{
    m_valueLabel->setText(text);
    m_progressBar->setValue(static_cast<int>(qBound(0.0, value, 100.0)));
}

void GaugeCard::updateValue(const QString &text)
{
    m_valueLabel->setText(text);
}
