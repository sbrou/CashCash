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

#include "ui_linepage.h"
#include "ui_fieldspage.h"
#include "csvimporterwizard.h"

#include <QScrollArea>
#include <set>

CSVImporterWizard::CSVImporterWizard(SqlRelationalTableModel * mod, const QString & filename, QWidget *parent)
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
    setOption(NoDefaultButton, true);

    setWindowTitle(tr("Assistant d'importation de fichier CSV"));
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
        op.amount = tab->item(r, 3)->text().toFloat();
        op.tag = tab->item(r, 5)->text().toInt();
        op.description = tab->item(r, 6)->text();

        ops->push_back(op);
    }
    QDialog::accept();
}

void CSVImporterWizard::setNbOperations(int nb)
{
    nbOps = nb;
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

    topLabel = new QLabel(tr("Cet assistant vous aidera à importer des opérations à partir d'un fichier CSV."));
    topLabel->setWordWrap(true);

    fileLabel = new QLabel(tr("F&ichier:"));
    qleFile = new QLineEdit;
    qleFile->setReadOnly(true);
    fileLabel->setBuddy(qleFile);

    qpbChooseFile = new QPushButton(tr("Choisir..."));
    connect(qpbChooseFile, SIGNAL(clicked()), this, SLOT(chooseFile()));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(fileLabel);
    hbox->addWidget(qleFile);
    hbox->addWidget(qpbChooseFile);

    registerField("filename*", qleFile);

//    newConfigRadioButton = new QRadioButton(tr("&Create a new configuration"));
//    newConfigRadioButton->setChecked(true);

//    useConfigRadioButton = new QRadioButton(tr("&Use a configuration"));
//    configsList = new QComboBox;
//    configsList->setEnabled(false);
//    connect(useConfigRadioButton, SIGNAL(toggled(bool)), configsList, SLOT(setEnabled(bool)));

//    QGridLayout *gbox = new QGridLayout;
//    gbox->addWidget(newConfigRadioButton, 0, 0);
//    gbox->addWidget(useConfigRadioButton, 1, 0);
//    gbox->addWidget(configsList, 1, 1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addLayout(hbox);
//    layout->addLayout(gbox);
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
    return CSVImporterWizard::Page_Line;
}

void IntroPage::chooseFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Sélectionner un fichier à importer"), QDir::home().dirName(),
                                                    tr("Fichiers CSV (*.csv)"));
    if (filename.isEmpty())
        return;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Problème de fichier"),
                              tr("Impossible de lire le fichier sélectionné. Veuillez en choisir un autre."));
        return;
    }

    file.close();
    qleFile->setText(filename);
}

/////////////////////////////////////

LinePage::LinePage(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::LinePage)
{
    ui->setupUi(this);

    ui->qleLineSelected->setValidator(new QIntValidator);
    registerField("OpHeaderLine*", ui->qleLineSelected);

    ui->qleOpLine->setValidator(new QIntValidator);
    connect(ui->enterOpLine, SIGNAL(stateChanged(int)), this, SLOT(handleOpLine(int)));
    connect(ui->qleLineSelected, SIGNAL(textChanged(QString)), this, SLOT(updateOpLine(QString)));

    registerField("firstOpLine", ui->qleOpLine);
}

void LinePage::handleOpLine(int state)
{
    if (state == Qt::Checked)
    {
        ui->qleOpLine->setEnabled(true);
    }
    else
    {
        ui->qleOpLine->setEnabled(false);
        bool ok;
        int line = ui->qleLineSelected->text().toInt(&ok);
        if (ok)
            setField("firstOpLine",line+1);
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
        ui->csvEditor->setPlainText(file.readAll());

    file.close();
}

/////////////////////////////////////

FieldsPage::FieldsPage(QWidget *parent)
    : QWizardPage(parent),
      ui(new Ui::FieldsPage)
{
    ui->setupUi(this);

    ui->qcbDateFormat->addItems({"dd/MM/yyyy", "MM/dd/yyyy"});

    registerField("dateColumn*", ui->qleDate);
    registerField("dateFormat*", ui->qcbDateFormat, "currentText", SIGNAL(currentTextChanged(QString)));
    registerField("catColumn", ui->qleCat);
    registerField("amountColumn*", ui->qleAmount);
    registerField("tagColumn", ui->qleTag);
    registerField("desColumn*", ui->qleDes);

    ui->qlHeaders->setFrameStyle(QFrame::Panel | QFrame::Sunken);
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
        ui->csvEditor->setPlainText(file.readAll());
        file.close();
        ui->qlHeaders->setText(text);
    }

    qobject_cast<CSVImporterWizard*>(wizard())->setNbOperations(nbLines);
}

/////////////////////////////////////

CategoriesPage::CategoriesPage(QSqlTableModel *mod, QWidget *parent)
    : QWizardPage(parent),
      cats(mod)
{
    setTitle(tr("Associer les catégories"));
    setSubTitle(tr("Veuiller associer les catégories du fichier CSV à celles de votre compte.<br>"
                   "<b>N.B.: Les affectations automatiques sont prioritaires sur les affectations de l'importation.</b>"));

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
    setTitle(tr("Associer les tags"));
    setSubTitle(tr("Veuiller associer les tags du fichier CSV à ceux de votre compte.<br>"
                   "<b>N.B.: Les affectations automatiques sont prioritaires sur les affectations de l'importation.</b>"));

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
    setTitle(tr("Terminer l'importation"));
    setSubTitle(tr("Pour compléter l'importation, veuillez vérifier les opérations."));

    tableWidget = new QTableWidget;
    tableWidget->setColumnCount(7);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->hide();
    tableWidget->setHorizontalHeaderLabels(QStringList({"Date", "Catégorie", "CatId",
                                                        "Montant", "Tag", "TagId", "Description"}));
    tableWidget->hideColumn(2);
    tableWidget->hideColumn(5);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QVBoxLayout *layout = new QVBoxLayout;
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
        float amount;
        QLocale german(QLocale::German);
        amount = german.toFloat(infos.at(amountCol), &ok);
        if (!ok) { // try with locale c
            QLocale c(QLocale::C);
            amount = c.toFloat(infos.at(amountCol), &ok);
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
