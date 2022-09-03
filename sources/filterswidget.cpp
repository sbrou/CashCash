#include "filterswidget.h"

#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QAction>

filtersWidget::filtersWidget(QWidget *parent)
    : QWidget{parent}
{
    QDate today = QDate::currentDate();

    qdeDateFrom = new QDateEdit(this);
    qdeDateFrom->setCalendarPopup(true);
    qdeDateFrom->setDate(QDate(today.year(), today.month(), 1));

    QFormLayout *fromForm = new QFormLayout;
    fromForm->addRow("From :", qdeDateFrom);

    qdeDateTo = new QDateEdit(this);
    qdeDateTo->setCalendarPopup(true);
    qdeDateTo->setDate(QDate(today.year(), today.month(), today.daysInMonth()));

    QFormLayout *toForm = new QFormLayout;
    toForm->addRow("To :", qdeDateTo);

    qleSearch = new QLineEdit(this);
    qleSearch->setPlaceholderText(tr("Search"));
    qleSearch->addAction(QIcon(":/images/images/search_48px.png"), QLineEdit::LeadingPosition);

    QPushButton *qpbSearch = new QPushButton(this);
    qpbSearch->setText(tr("Apply"));
    connect(qpbSearch, SIGNAL(clicked()), this, SLOT(buildStatement()));

//    QPalette pal = qpbSearch->palette();
//    pal.setColor(QPalette::Button, QColor("#55aaff"));
//    qpbSearch->setAutoFillBackground(true);
//    qpbSearch->setPalette(pal);
    qpbSearch->setStyleSheet("QPushButton { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                             "stop: 0 #55aaff, stop: 1 #dadbde); }");

    qleMinAmount = new QLineEdit(this);
    QDoubleValidator * minValidator = new QDoubleValidator(qleMinAmount);
    minValidator->setLocale(QLocale::German);
    qleMinAmount->setValidator(minValidator);
    qleMinAmount->setPlaceholderText(tr("Min amount"));
    qleMinAmount->setWindowIcon(QIcon(""));
    qleMinAmount->addAction(QIcon(":/images/images/euro_48px.png"), QLineEdit::TrailingPosition);

    qleMaxAmount = new QLineEdit(this);
    QDoubleValidator * maxValidator = new QDoubleValidator(qleMaxAmount);
    maxValidator->setLocale(QLocale::German);
    qleMaxAmount->setValidator(maxValidator);
    qleMaxAmount->setPlaceholderText(tr("Max amount"));
    qleMaxAmount->setWindowIcon(QIcon(""));
    qleMaxAmount->addAction(QIcon(":/images/images/euro_48px.png"), QLineEdit::TrailingPosition);

    qcbCat = new QComboBox(this);
    qcbCat->setPlaceholderText(tr("Category"));

    qcbTag = new QComboBox(this);
    qcbTag->setPlaceholderText(tr("Tag"));

    QPushButton *qpbReset = new QPushButton(QIcon(":/images/images/reset_48px.png"), tr("Reset"), this);
    connect(qpbReset, SIGNAL(clicked()), this, SLOT(reset()));


    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addLayout(fromForm, 0, 0);
    mainLayout->addLayout(toForm, 0, 1);
    mainLayout->addWidget(qleSearch, 0, 2, 1, 2);
    mainLayout->addWidget(qpbSearch, 0, 4);
    mainLayout->addWidget(qleMinAmount, 1, 0);
    mainLayout->addWidget(qleMaxAmount, 1, 1);
    mainLayout->addWidget(qcbCat, 1, 2);
    mainLayout->addWidget(qcbTag, 1, 3);
    mainLayout->addWidget(qpbReset, 1, 4);
}

filtersWidget::~filtersWidget()
{
    qdeDateFrom->deleteLater();
    qdeDateTo->deleteLater();
    qleSearch->deleteLater();
    qleMinAmount->deleteLater();
    qleMaxAmount->deleteLater();
    qcbCat->deleteLater();
    qcbTag->deleteLater();
}

void filtersWidget::reset()
{
    QDate today = QDate::currentDate();
    qdeDateFrom->setDate(QDate(today.year(), today.month(), 1));
    qdeDateTo->setDate(QDate(today.year(), today.month(), today.daysInMonth()));

    qleSearch->clear();
    qleMinAmount->clear();
    qleMaxAmount->clear();

    qcbCat->setCurrentIndex(-1);
    qcbTag->setCurrentIndex(-1);


    QString statement = QString("op_date>='%1' AND op_date<='%2'").arg(qdeDateFrom->date().toString(Qt::ISODateWithMs))
                                                                    .arg(qdeDateTo->date().toString(Qt::ISODateWithMs));
    emit statementBuilt(statement);
}

void filtersWidget::buildStatement()
{
    QString statement = QString("op_date>='%1' AND op_date<='%2'").arg(qdeDateFrom->date().toString(Qt::ISODateWithMs))
                                                                    .arg(qdeDateTo->date().toString(Qt::ISODateWithMs));
    if (!qleSearch->text().isEmpty())
        statement += QString(" AND description LIKE '%%1%'").arg(qleSearch->text());

    if (!qleMinAmount->text().isEmpty())
        statement += QString(" AND amount>=%1").arg(qleMinAmount->text().toDouble());

    if (!qleMaxAmount->text().isEmpty())
        statement += QString(" AND amount<=%1").arg(qleMaxAmount->text().toDouble());

    if (qcbCat->currentIndex()>=0)
        statement += QString(" AND category=%1").arg(qcbCat->currentIndex()+1);

    if (qcbTag->currentIndex()>=0)
        statement += QString(" AND tag=%1").arg(qcbTag->currentIndex()+1);

    emit statementBuilt(statement);
}

void filtersWidget::populateComboBoxes(QSqlTableModel * cats, QSqlTableModel * tags)
{
    qcbCat->setModel(cats);
    qcbCat->setModelColumn(1);
    qcbCat->setCurrentIndex(-1);

    qcbTag->setModel(tags);
    qcbTag->setModelColumn(1);
    qcbTag->setCurrentIndex(-1);
}
