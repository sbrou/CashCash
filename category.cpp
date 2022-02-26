#include "category.h"

namespace category
{
    enum type
    {
        SPENDING=0x1,
        INCOME=0x2,
        SAVING=0x4
    };
}

Category::Category(QString title, QObject *parent)
    : QObject{parent},
      _title(title)
{

}
