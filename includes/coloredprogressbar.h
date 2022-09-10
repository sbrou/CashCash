#ifndef COLOREDPROGRESSBAR_H
#define COLOREDPROGRESSBAR_H

#include <QWidget>
#include <QProgressBar>
#include <QPaintEvent>
#include <QRect>

class ColoredProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    explicit ColoredProgressBar(QWidget *parent = 0);
    ~ColoredProgressBar();

    void paint(QPainter *painter, const QRect& rec);
    void setPercentage(double);

signals:

public slots:

private:
    double percentage;
};

#endif // COLOREDPROGRESSBAR_H
