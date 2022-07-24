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

signals:

public slots:
};

#endif // COLOREDPROGRESSBAR_H
