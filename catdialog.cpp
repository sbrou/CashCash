#include "catdialog.h"

#include <QColorDialog>
#include <QRadioButton>

CatDialog::CatDialog()
{
    setWindowIcon(QIcon(":/images/images/category_48px.png"));
    inputLayout = new QGridLayout;

    qlName = new QLabel(tr("&Name:"));
    inputLayout->addWidget(qlName, 0, 0);

    qleName = new QLineEdit;
    qlName->setBuddy(qleName);
    inputLayout->addWidget(qleName, 0, 1);

    qlColor = new QLabel(tr("&Color:"));
    inputLayout->addWidget(qlColor, 1, 0);

    pixmap = new QPixmap(16,16);
    qpbColor = new QPushButton(tr("Choose..."));
    qlColor->setBuddy(qpbColor);
    connect(qpbColor, SIGNAL(clicked()), this, SLOT(choose_color()));
    inputLayout->addWidget(qpbColor, 1, 1);

    qgbType = new QGroupBox;
    expenses = new QRadioButton(tr("&Expenses"));
    earnings = new QRadioButton(tr("&Earnings"));
    expenses->setChecked(true);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(expenses);
    hbox->addWidget(earnings);
//    hbox->addStretch(2);
    qgbType->setLayout(hbox);

    inputLayout->addWidget(qgbType, 2, 0, 1, 3);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

void CatDialog::choose_color()
{
    const QColorDialog::ColorDialogOptions options = QFlag(QColorDialog::DontUseNativeDialog);
    const QColor color = QColorDialog::getColor(cat_color, this, tr("Select Color"), options);

    if (color.isValid()) {
        cat_color = color;
        pixmap->fill(color);
        qpbColor->setIcon(QIcon(*pixmap));
    }
}

QString CatDialog::name()
{
    return qleName->text();
}

QString CatDialog::color()
{
    return cat_color.name();
}

int CatDialog::type()
{
    if (expenses->isChecked())
    {
        return 0;
    }

    else
    {
        return 1;
    }
}

void CatDialog::setFields(QSqlRecord rec)
{
    qleName->setText(rec.value(1).toString());
    cat_color = QColor(rec.value(2).toString());
    pixmap->fill(cat_color);
    qpbColor->setIcon(QIcon(*pixmap));
    if (!rec.value(3).toInt())
        expenses->setChecked(true);
    else
        earnings->setChecked(true);
}
