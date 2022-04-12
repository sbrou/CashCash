#include "filterswidget.h"

#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QAction>

filtersWidget::filtersWidget(QWidget *parent)
    : QWidget{parent}
{
    qdeDateFrom = new QDateEdit(this);
    qdeDateFrom->setCalendarPopup(true);
//    qdeDateFrom->setStyleSheet("QDateEdit::drop-down {  image: url(:/images/images/calendar_48px.png);  }");

    QLineEdit *qleDateTo = new QLineEdit(this);
    qleDateTo->setReadOnly(true);

    QLineEdit *qleSearch = new QLineEdit(this);
    qleSearch->setPlaceholderText(tr("Search"));
    qleSearch->addAction(QIcon(":/images/images/search_48px.png"), QLineEdit::LeadingPosition);

    QPushButton *qpbSearch = new QPushButton(this);
    qpbSearch->setText(tr("Apply"));

    QPalette pal = qpbSearch->palette();
    pal.setColor(QPalette::Button, QColor("#55aaff"));
    qpbSearch->setAutoFillBackground(true);
    qpbSearch->setPalette(pal);

    QLineEdit *qleMinAmount = new QLineEdit(this);
    qleMinAmount->setPlaceholderText(tr("Min amount"));
    qleMinAmount->setWindowIcon(QIcon(""));
    qleMinAmount->addAction(QIcon(":/images/images/euro_48px.png"), QLineEdit::TrailingPosition);

    QLineEdit *qleMaxAmount = new QLineEdit(this);
    qleMaxAmount->setPlaceholderText(tr("Max amount"));
    qleMaxAmount->setWindowIcon(QIcon(""));
    qleMaxAmount->addAction(QIcon(":/images/images/euro_48px.png"), QLineEdit::TrailingPosition);

    QComboBox *qcbCat = new QComboBox(this);
    qcbCat->setPlaceholderText(tr("Category"));

    QComboBox *qcbTag = new QComboBox(this);
    qcbTag->setPlaceholderText(tr("Tag"));

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(qdeDateFrom, 0, 0);
    mainLayout->addWidget(qleDateTo, 0, 1);
    mainLayout->addWidget(qleSearch, 0, 2);
    mainLayout->addWidget(qpbSearch, 0, 3);
    mainLayout->addWidget(qleMinAmount, 1, 0);
    mainLayout->addWidget(qleMaxAmount, 1, 1);
    mainLayout->addWidget(qcbCat, 1, 2);
    mainLayout->addWidget(qcbTag, 1, 3);
}
