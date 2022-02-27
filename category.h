#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>

class Category : public QObject
{
    Q_OBJECT

public:

    enum Type
    {
        NONE=0x0,
        SPENDING=0x1,
        INCOME=0x2,
        SAVING=0x4
    };

    explicit Category(QObject *parent = nullptr);
    explicit Category(Category::Type type, QObject *parent = nullptr);

    double amount();
    void addOperation(double amount);

signals:

private:
    unsigned _type;
    double _budget;
    double _amount;

};

typedef QMap<QString,Category*> Categories;

#endif // CATEGORY_H
