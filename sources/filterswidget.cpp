#include "filterswidget.h"

#include <QPushButton>
#include <QComboBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QAction>

#include "utilities.h"

using namespace utilities;

filtersWidget::filtersWidget(QWidget *parent)
    : QWidget{parent}
{
    QDate today = QDate::currentDate();

    qdeDateFrom = new QDateEdit(this);
    qdeDateFrom->setCalendarPopup(true);
    qdeDateFrom->setDate(QDate(today.year(), today.month(), 1));

    QFormLayout *fromForm = new QFormLayout;
    fromForm->addRow("De :", qdeDateFrom);

    qdeDateTo = new QDateEdit(this);
    qdeDateTo->setCalendarPopup(true);
    qdeDateTo->setDate(QDate(today.year(), today.month(), today.daysInMonth()));

    QFormLayout *toForm = new QFormLayout;
    toForm->addRow("À :", qdeDateTo);

    qleSearch = new QLineEdit(this);
    qleSearch->setPlaceholderText(tr("Rechercher"));
    qleSearch->setClearButtonEnabled(true);
    qleSearch->addAction(QIcon(":/images/images/search.png"), QLineEdit::LeadingPosition);

    QPushButton *qpbSearch = new QPushButton(QIcon(":/images/images/filter.png"), tr("Appliquer"), this);
    connect(qpbSearch, SIGNAL(clicked()), this, SLOT(buildStatement()));

//    qpbSearch->setStyleSheet("QPushButton { background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//                             "stop: 0 #55aaff, stop: 1 #dadbde); }");

    qleMinAmount = new QLineEdit(this);
    QDoubleValidator * minValidator = new QDoubleValidator(qleMinAmount);
    minValidator->setLocale(QLocale::German);
    qleMinAmount->setValidator(minValidator);
    qleMinAmount->setPlaceholderText(tr("Montant min"));
    qleMinAmount->setWindowIcon(QIcon(""));
    qleMinAmount->addAction(QIcon(":/images/images/euro.png"), QLineEdit::TrailingPosition);

    qleMaxAmount = new QLineEdit(this);
    QDoubleValidator * maxValidator = new QDoubleValidator(qleMaxAmount);
    maxValidator->setLocale(QLocale::German);
    qleMaxAmount->setValidator(maxValidator);
    qleMaxAmount->setPlaceholderText(tr("Montant max"));
    qleMaxAmount->setWindowIcon(QIcon(""));
    qleMaxAmount->addAction(QIcon(":/images/images/euro.png"), QLineEdit::TrailingPosition);

    qcbCat = new QComboBox(this);
    qcbCat->setPlaceholderText(tr("Catégorie"));

    qcbTag = new QComboBox(this);
    qcbTag->setPlaceholderText(tr("Tag"));

    QPushButton *qpbReset = new QPushButton(QIcon(":/images/images/redo.png"), tr("Reset"), this);
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

    initComboBoxes();

    QString statement = lowerDateCondition(qdeDateFrom->date()) + COND_SEP + upperDateCondition(qdeDateTo->date());
    emit statementBuilt(statement);
}

void filtersWidget::buildStatement()
{
    QStringList condList;
    condList << lowerDateCondition(qdeDateFrom->date());
    condList << upperDateCondition(qdeDateTo->date());

    if (!qleSearch->text().isEmpty())
        condList << descriptionCondition(qleSearch->text());

    if (!qleMinAmount->text().isEmpty())
        condList << lowerAmountCondition(qleMinAmount->text().toFloat());

    if (!qleMaxAmount->text().isEmpty())
        condList << upperAmountCondition(qleMaxAmount->text().toFloat());

    if (qcbCat->currentIndex()>=0)
        condList << categoryCondition(qcbCat->currentIndex()+1);

    if (qcbTag->currentIndex()>=0)
        condList << tagCondition(qcbTag->currentIndex()+1);

    emit statementBuilt(condList.join(COND_SEP));
}

void filtersWidget::populateComboBoxes(QSqlTableModel * cats, QSqlTableModel * tags)
{
    qcbCat->setModel(cats);
    qcbCat->setModelColumn(1);

    qcbTag->setModel(tags);
    qcbTag->setModelColumn(1);

    initComboBoxes();
}

void filtersWidget::initComboBoxes()
{
    qcbCat->setCurrentIndex(-1);
    qcbTag->setCurrentIndex(-1);
}
