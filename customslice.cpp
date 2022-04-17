#include "customslice.h"

#include <QBrush>
#include <QDebug>

QT_USE_NAMESPACE

CustomSlice::CustomSlice(qreal value, QString label, QColor color)
    : m_label(label)
    , m_value(value)
{
    QBrush slice_brush = QBrush(color);
    setBrush(slice_brush);
    setValue(value);
    updateLabel();
    setLabelFont(QFont("Arial", 8));
    connect(this, &CustomSlice::percentageChanged, this, &CustomSlice::updateLabel);
}

CustomSlice::~CustomSlice()
{

}

void CustomSlice::updateLabel()
{
    QString label = m_label;
    label += QString(" %1").arg(QString::number(m_value));
    label += "€";
    label += ", ";
    label += QString::number(this->percentage() * 100, 'f', 1);
    label += "%";
    setLabel(label);
}

void CustomSlice::updateValue(qreal amount)
{
    qDebug() << "in update value" << amount;
    m_value += amount;
    setValue(m_value);
}

//#include "moc_customslice.cpp"