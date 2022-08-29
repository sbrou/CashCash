/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "csvimporterwizard.h"

#include <QSqlRelationalTableModel>
#include <QScrollArea>
#include <set>

CSVImporterWizard::CSVImporterWizard(QSqlRelationalTableModel * mod, const QString & filename, QWidget *parent)
    : QWizard(parent),
      catsModel(mod->relationModel(2)),
      tagsModel(mod->relationModel(4))
{
    ops = new OperationsVector;

    setPage(Page_Intro, new IntroPage(filename));
    setPage(Page_Line, new LinePage);
    setPage(Page_Fields, new FieldsPage);
    setPage(Page_Categories, new CategoriesPage(catsModel));
    setPage(Page_Tags, new TagsPage(tagsModel));
    setPage(Page_Conclusion, new ConclusionPage);

    setStartId(Page_Intro);
//    setWizardStyle(ModernStyle);

    setOption(HaveHelpButton, true);
    setOption(NoDefaultButton, true);

//    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo.png"));

    connect(this, &QWizard::helpRequested, this, &CSVImporterWizard::showHelp);

    setWindowTitle(tr("CSV Importer"));
}

void CSVImporterWizard::showHelp()
{
    static QString lastHelpMessage;

    QString message;

    switch (currentId()) {
    case Page_Intro:
        message = tr("The decision you make here will affect which page you "
                     "get to see next.");
        break;
//    case Page_Evaluate:
//        message = tr("Make sure to provide a valid email address, such as "
//                     "toni.buddenbrook@example.de.");
//        break;
//    case Page_Register:
//        message = tr("If you don't provide an upgrade key, you will be "
//                     "asked to fill in your details.");
//        break;
//    case Page_Details:
//        message = tr("Make sure to provide a valid email address, such as "
//                     "thomas.gradgrind@example.co.uk.");
//        break;
    case Page_Conclusion:
        message = tr("You must accept the terms and conditions of the "
                     "license to proceed.");
        break;
    default:
        message = tr("This help is likely not to be of any help.");
    }

    if (lastHelpMessage == message)
        message = tr("Sorry, I already gave what help I could. "
                     "Maybe you should try asking a human?");

    QMessageBox::information(this, tr("License Wizard Help"), message);

    lastHelpMessage = message;
}

void CSVImporterWizard::accept()
{
    QTableWidget *tab = qobject_cast<ConclusionPage*>(page(Page_Conclusion))->table();
    ops->reserve(tab->rowCount());

    for (int r = 0; r < tab->rowCount(); ++r )
    {
        Operation op;
        op.date = QDate::fromString(tab->item(r, 0)->text(),"yyyy-MM-dd");
        op.cat = tab->item(r, 2)->text().toInt();
        op.amount = tab->item(r, 3)->text().toDouble();
        op.tag = tab->item(r, 5)->text().toInt();
        op.description = tab->item(r, 6)->text();

        ops->push_back(op);
    }

    qDebug() << "le nombre d'operations dans ops est : " << ops->size();
    QDialog::accept();
}

void CSVImporterWizard::setNbOperations(int nb)
{
    nbOps = nb;
    qDebug() << "le nombre d'operations à importer est : " << nbOps;
}

int CSVImporterWizard::getNbOperations() const
{
    return nbOps;
}

OperationsVector* CSVImporterWizard::getOperations()
{
    return ops;
}

GroupsMap* CSVImporterWizard::matchedCategories()
{
    return &catsMap;
}

GroupsMap* CSVImporterWizard::matchedTags()
{
    return &tagsMap;
}

/////////////////////////////////////

IntroPage::IntroPage(const QString & filename, QWidget *parent)
    : QWizardPage(parent),
      _file(filename)
{
    setTitle(tr("Introduction"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    topLabel = new QLabel(tr("This wizard will help you register your copy of "
                             "<i>Super Product One</i>&trade; or start "
                             "evaluating the product."));
    topLabel->setWordWrap(true);

    fileLabel = new QLabel(tr("F&ichier:"));
    qleFile = new QLineEdit;
    qleFile->setReadOnly(true);
    fileLabel->setBuddy(qleFile);

    qpbChooseFile = new QPushButton(tr("Choose..."));
    connect(qpbChooseFile, SIGNAL(clicked()), this, SLOT(chooseFile()));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(fileLabel);
    hbox->addWidget(qleFile);
    hbox->addWidget(qpbChooseFile);

    registerField("filename*", qleFile);

    newConfigRadioButton = new QRadioButton(tr("&Create a new configuration"));
    useConfigRadioButton = new QRadioButton(tr("&Use a configuration"));
    newConfigRadioButton->setChecked(true);

    configsList = new QComboBox;
    configsList->setEnabled(false);
    connect(useConfigRadioButton, SIGNAL(toggled(bool)), configsList, SLOT(setEnabled(bool)));

    QGridLayout *gbox = new QGridLayout;
    gbox->addWidget(newConfigRadioButton, 0, 0);
    gbox->addWidget(useConfigRadioButton, 1, 0);
    gbox->addWidget(configsList, 1, 1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addLayout(hbox);
    layout->addLayout(gbox);
    setLayout(layout);
}

void IntroPage::initializePage()
{
    if (!_file.isEmpty())
    {
        qleFile->setText(_file);
        emit qleFile->textChanged(_file);
    }
}

int IntroPage::nextId() const
{
    if (newConfigRadioButton->isChecked()) {
        return CSVImporterWizard::Page_Line;
    } else {
        return CSVImporterWizard::Page_Conclusion;
    }
}

void IntroPage::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Selectionner un fichier à importer"), QDir::home().dirName(),
                                                    tr("csv files (*.csv)"));
    if (filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "can't open input file";
        QMessageBox::critical(this, tr("Problème de fichier"),
                              tr("Impossible de lire le fichier sélectionné. Veuillez en choisir un autre."));
        return;
    }

    file.close();
    qleFile->setText(filename);
}

/////////////////////////////////////

LinePage::LinePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Evaluate <i>Super Product One</i>&trade;"));
    setSubTitle(tr("Please fill the following field with the operations header line number."));

    lineLabel = new QLabel(tr("L&igne sélectionnée:"));
    qleLineSelected = new QLineEdit;
    qleLineSelected->setValidator(new QIntValidator);
    lineLabel->setBuddy(qleLineSelected);

    registerField("OpHeaderLine*", qleLineSelected);

    enterOpLine = new QCheckBox(tr("The line right after the operations header line is not the first operation. In which case, please enter"
                                   " the first operation line number below."));
    qleOpLine = new QLineEdit;
    qleOpLine->setValidator(new QIntValidator);
    connect(enterOpLine, SIGNAL(stateChanged(int)), this, SLOT(handleOpLine(int)));
    connect(qleLineSelected, SIGNAL(textChanged(QString)), this, SLOT(updateOpLine(QString)));

    registerField("firstOpLine", qleOpLine);

    csvEditor = new CSVEditor;

    QVBoxLayout *vbox = new QVBoxLayout;

    QFormLayout *layout = new QFormLayout;
    layout->addRow(lineLabel, qleLineSelected);

    vbox->addLayout(layout);
    vbox->addWidget(enterOpLine);
    vbox->addWidget(qleOpLine);
    vbox->addWidget(csvEditor);
    setLayout(vbox);
}

void LinePage::handleOpLine(int state)
{
    if (state == Qt::Checked)
    {
        qleOpLine->setEnabled(true);
    }
    else
    {
        qleOpLine->setEnabled(false);
        setField("firstOpLine","");
    }
}

void LinePage::updateOpLine(const QString & line)
{
    int id_line = line.toInt() + 1;
    setField("firstOpLine",id_line);
}

int LinePage::nextId() const
{
    return CSVImporterWizard::Page_Fields;
}

void LinePage::initializePage()
{
    handleOpLine(0);

    QFile file(field("filename").toString());
    if (file.open(QFile::ReadOnly | QFile::Text))
        csvEditor->setPlainText(file.readAll());

    file.close();
}

/////////////////////////////////////

FieldsPage::FieldsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Evaluate <i>Super Product One</i>&trade;"));
    setSubTitle(tr("Please fill both fields. Make sure to provide a valid "
                   "email address (e.g., john.smith@example.com)."));

    qleDate = new QLineEdit;
    qleDate->setValidator(new QIntValidator);
    qleDateFormat = new QLineEdit;
    qleCat = new QLineEdit;
    qleCat->setValidator(new QIntValidator);
    qleAmount = new QLineEdit;
    qleAmount->setValidator(new QIntValidator);
    qleTag = new QLineEdit;
    qleTag->setValidator(new QIntValidator);
    qleDes = new QLineEdit;
    qleDes->setValidator(new QIntValidator);

    QFormLayout *form = new QFormLayout;
    form->addRow(tr("Date :"), qleDate);
    form->addRow(tr("Format Date (ex: dd/MM/yyyy) : "), qleDateFormat);
    form->addRow(tr("Catégorie : "), qleCat);
    form->addRow(tr("Montant : "), qleAmount);
    form->addRow(tr("Etiquette : "), qleTag);
    form->addRow(tr("Description : "), qleDes);

    registerField("dateColumn*", qleDate);
    registerField("dateFormat*", qleDateFormat);
    registerField("catColumn", qleCat);
    registerField("amountColumn*", qleAmount);
    registerField("tagColumn", qleTag);
    registerField("desColumn*", qleDes);

    qlHeaders = new QLabel;
    qlHeaders->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    csvEditor = new CSVEditor;

    QGridLayout *gbox = new QGridLayout;
    gbox->addLayout(form, 0, 0, Qt::AlignCenter);
    gbox->addWidget(qlHeaders, 0, 1);
    gbox->addWidget(csvEditor, 1, 0, 1, 2);
    setLayout(gbox);
}

int FieldsPage::nextId() const
{ 
    if (field("catColumn").toInt() > 0)
        return CSVImporterWizard::Page_Categories;
    else if (field("tagColumn").toInt() > 0)
        return CSVImporterWizard::Page_Tags;
    else
        return CSVImporterWizard::Page_Conclusion;
}

void FieldsPage::initializePage()
{
    int nbLines = 0;
    QFile file(field("filename").toString());
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString text;
        QTextStream in(&file);
        int id_line = field("OpHeaderLine").toInt();
        int id_opLine = field("firstOpLine").toInt();
        int id = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (id == id_line) {
                QStringList headers = line.split(QLatin1Char(';'));
                for (int i = 1; i <= headers.size(); ++i) {
                    text += QString::number(i) + " " + headers.at(i-1) + "\n";
                }
            }

            if (id >= id_opLine && !line.isEmpty())
                ++nbLines;

            ++id;
        }
        file.close();
        file.open(QFile::ReadOnly | QFile::Text);
        csvEditor->setPlainText(file.readAll());
        file.close();
        qlHeaders->setText(text);
    }

    qobject_cast<CSVImporterWizard*>(wizard())->setNbOperations(nbLines);
}

/////////////////////////////////////

CategoriesPage::CategoriesPage(QSqlTableModel *mod, QWidget *parent)
    : QWizardPage(parent),
      cats(mod)
{
    setTitle(tr("Evaluate <i>Super Product One</i>&trade;"));
    setSubTitle(tr("Please fill both fields. Make sure to provide a valid "
                   "email address (e.g., john.smith@example.com)."));

    QScrollArea * scrollArea = new QScrollArea;
    QWidget * scrollWidget = new QWidget;
    form = new QFormLayout(scrollWidget);

    scrollArea->setWidget(scrollWidget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(scrollArea);
    setLayout(vbox);
}

void CategoriesPage::initializePage()
{
    QFile file(field("filename").toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int op_line = field("firstOpLine").toInt();
    int iline = 1;
    int catCol = field("catColumn").toInt() - 1;
    std::set<QString> catsKeys;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (iline++ < op_line)
            continue;

        QStringList infos = line.split(QLatin1Char(';'));

        if (!infos.at(catCol).isEmpty())
        {
            QString cat = infos.at(catCol);
            cat.remove(QChar('"'));
            catsKeys.insert(cat);
        }
    }

    file.close();

    foreach (const QString &key, catsKeys)
    {
        QComboBox * cb = new QComboBox;
        cb->setModel(cats);
        cb->setModelColumn(1);
        cb->setCurrentIndex(0);

        form->addRow(key, cb);
    }
}

int CategoriesPage::nextId() const
{
    GroupsMap* catsMap = qobject_cast<CSVImporterWizard*>(wizard())->matchedCategories();
    for (int i = 0; i < form->rowCount(); ++i)
    {
        QLayoutItem *label = form->itemAt(i, QFormLayout::LabelRole);
        QString oldCat = qobject_cast<QLabel *>(label->widget())->text();

        QLayoutItem *field = form->itemAt(i, QFormLayout::FieldRole);
        QComboBox * cb = qobject_cast<QComboBox *>(field->widget());
        QString newCat = cb->currentText();
        int newCatId = cb->currentIndex()+1;

        catsMap->insert(oldCat, QPair<int,QString>(newCatId,newCat));
    }

    if (field("tagColumn").toInt() > 0)
        return CSVImporterWizard::Page_Tags;
    else
        return CSVImporterWizard::Page_Conclusion;
}

/////////////////////////////////////

TagsPage::TagsPage(QSqlTableModel *mod, QWidget *parent)
    : QWizardPage(parent),
      tags(mod)
{
    setTitle(tr("Evaluate <i>Super Product One</i>&trade;"));
    setSubTitle(tr("Please fill both fields. Make sure to provide a valid "
                   "email address (e.g., john.smith@example.com)."));

    QScrollArea * scrollArea = new QScrollArea;
    QWidget * scrollWidget = new QWidget;
    form = new QFormLayout(scrollWidget);

    scrollArea->setWidget(scrollWidget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(scrollArea);
    setLayout(vbox);
}

void TagsPage::initializePage()
{
    QFile file(field("filename").toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int op_line = field("firstOpLine").toInt();
    int iline = 1;
    int tagCol = field("tagColumn").toInt() - 1;
    std::set<QString> tagsKeys;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (iline++ < op_line)
            continue;

        QStringList infos = line.split(QLatin1Char(';'));

        if (!infos.at(tagCol).isEmpty())
        {
            QString tag = infos.at(tagCol);
            tag.remove(QChar('"'));
            tagsKeys.insert(tag);
        }
    }

    file.close();

    foreach (const QString &key, tagsKeys)
    {
        QComboBox * cb = new QComboBox;
        cb->setModel(tags);
        cb->setModelColumn(1);
        cb->setCurrentIndex(0);

        form->addRow(key, cb);
    }
}

int TagsPage::nextId() const
{
    GroupsMap* tagsMap = qobject_cast<CSVImporterWizard*>(wizard())->matchedTags();
    for (int i = 0; i < form->rowCount(); ++i)
    {
        QLayoutItem *label = form->itemAt(i, QFormLayout::LabelRole);
        QString oldTag = qobject_cast<QLabel *>(label->widget())->text();

        QLayoutItem *field = form->itemAt(i, QFormLayout::FieldRole);
        QComboBox * cb = qobject_cast<QComboBox *>(field->widget());
        QString newTag = cb->currentText();
        int newTagId = cb->currentIndex()+1;

        tagsMap->insert(oldTag, QPair<int,QString>(newTagId,newTag));
    }

    return CSVImporterWizard::Page_Conclusion;
}

/////////////////////////////////////

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Complete Your Registration"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);
    bottomLabel->setText("<u>Upgrade License Agreement:</u> "
                         "This software is licensed under the terms of your "
                         "current license.");

    tableWidget = new QTableWidget;
    tableWidget->setColumnCount(7);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->hide();
    tableWidget->setHorizontalHeaderLabels(QStringList({"Date", "Category", "CatId",
                                                        "Amount", "Tag", "TagId", "Description"}));
    tableWidget->hideColumn(2);
    tableWidget->hideColumn(5);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(tableWidget);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    CSVImporterWizard* wiz = qobject_cast<CSVImporterWizard*>(wizard());
    tableWidget->setRowCount(wiz->getNbOperations());
    GroupsMap * catsMap = wiz->matchedCategories();
    GroupsMap * tagsMap = wiz->matchedTags();

    QFile file(field("filename").toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int op_line = field("firstOpLine").toInt();
    int iline = 1;
    int row = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (iline++ < op_line)
            continue;

        process_line(row, catsMap, tagsMap, line);
        ++row;
    }

    file.close();
}

void ConclusionPage::process_line(int row, GroupsMap* catsMap, GroupsMap* tagsMap, const QString& line)
{
    int dateCol = field("dateColumn").toInt() - 1;
    int amountCol = field("amountColumn").toInt() - 1;
    int desCol = field("desColumn").toInt() - 1;
    int catCol = field("catColumn").toInt() - 1;
    int tagCol = field("tagColumn").toInt() - 1;

    QStringList infos = line.split(QLatin1Char(';'));

    if (infos.size() > 0)
    {
        //// date
        QDate op_date = QDate::fromString(infos.at(dateCol),field("dateFormat").toString());
        QTableWidgetItem *dateItem = new QTableWidgetItem(op_date.toString("yyyy-MM-dd"));
        tableWidget->setItem(row, 0, dateItem);

        //// category
        QString cat;
        int catId = 1;
        if (catCol >= 0) {
            GroupsMap::ConstIterator it_cat = catsMap->constFind(infos.at(catCol));
            if (it_cat != catsMap->constEnd()) {
                catId = it_cat.value().first;
                cat = it_cat.value().second;
            }
        }
        QTableWidgetItem *catItem = new QTableWidgetItem(cat);
        tableWidget->setItem(row, 1, catItem);

        //// Category id
        QTableWidgetItem *catIdItem = new QTableWidgetItem(QString::number(catId));
        tableWidget->setItem(row, 2, catIdItem);

        //// amount
        bool ok;
        double amount;
        QLocale german(QLocale::German);
        amount = german.toDouble(infos.at(amountCol), &ok);
        if (!ok) { // try with locale c
            QLocale c(QLocale::C);
            amount = c.toDouble(infos.at(amountCol), &ok);
        }
        QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(amount));
        tableWidget->setItem(row, 3, amountItem);

        //// tag
        QString tag;
        int tagId = 1;
        if (tagCol >= 0) {
            GroupsMap::ConstIterator it_tag = tagsMap->constFind(infos.at(tagCol));
            if (it_tag != tagsMap->constEnd()) {
                tagId = it_tag.value().first;
                tag = it_tag.value().second;
            }
        }
        QTableWidgetItem *tagItem = new QTableWidgetItem(tag);
        tableWidget->setItem(row, 4, tagItem);

        //// Tag id
        QTableWidgetItem *tagIdItem = new QTableWidgetItem(QString::number(tagId));
        tableWidget->setItem(row, 5, tagIdItem);

        //// description
        QString des = infos.at(desCol);
        des.remove(QChar('"'));
        QTableWidgetItem *desItem = new QTableWidgetItem(des);
        tableWidget->setItem(row, 6, desItem);
    }
}

QTableWidget * ConclusionPage::table()
{
    return tableWidget;
}
