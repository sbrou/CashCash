#include "coloredprogressbar.h"

#include <QPainter>
#include <QBrush>
#include <QStyle>
#include <QPen>
#include <QColor>

ColoredProgressBar::ColoredProgressBar(QWidget *parent) : QProgressBar(parent)
{
    percentage = value();
}

ColoredProgressBar::~ColoredProgressBar()
{

}

void ColoredProgressBar::paint(QPainter *painter, const QRect& rec)
{
    painter->save();

    setGeometry(rec);
    int val = value();
    int w = rec.width();
    int h = 0.8 * rec.height();
    int xpos = rec.x();
    int ypos = rec.y() + (0.1 * rec.height());
    int pos = QStyle::sliderPositionFromValue(minimum(), maximum(), val, w);

    QLinearGradient linearGradient(xpos, ypos, w, h);
    linearGradient.setColorAt(0, Qt::green);
    linearGradient.setColorAt(0.5, Qt::yellow);
    linearGradient.setColorAt(1, Qt::red);

    // Paint the progress bar
    QBrush brush(linearGradient);
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->drawRoundedRect(xpos, ypos, pos, h, 8, 8);

    // Paint text
    painter->setPen(Qt::black);
    painter->setBrush(QBrush(Qt::black));
    painter->drawText(xpos, ypos, w, h, Qt::AlignCenter, QString::number(percentage, 'f', 2) + "%");

    painter->restore();
}

void ColoredProgressBar::setPercentage(double doubleValue)
{
    percentage = doubleValue;
}
