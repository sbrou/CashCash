#ifndef CUSTOMSLICE_H
#define CUSTOMSLICE_H

#include <QtCharts/QPieSlice>

QT_BEGIN_NAMESPACE
class QAbstractSeries;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class CustomSlice : public QPieSlice
{
    Q_OBJECT

public:
    CustomSlice(qreal value, QString label, QColor color);
    virtual ~CustomSlice();

public Q_SLOTS:
    void updateLabel();
    void updateValue(qreal amount);

private:
    QString m_label;
    qreal m_value;
};

#endif // CUSTOMSLICE_H
