#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>

class filtersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit filtersWidget(QWidget *parent = nullptr);

signals:

private slots:

private:
    QDateEdit *qdeDateFrom;

};

#endif // FILTERSWIDGET_H
