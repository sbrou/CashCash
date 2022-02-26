#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>

class Category : public QObject
{
    Q_OBJECT
public:
    explicit Category(QString title, QObject *parent = nullptr);

signals:

private:
    unsigned _type;
    QString _title;
    double _budget;
    double _total;

};

#endif // CATEGORY_H
