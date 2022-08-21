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

CSVImporterWizard::CSVImporterWizard(const QString & filename, QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage(filename));
    setPage(Page_Line, new LinePage);
    setPage(Page_Fields, new FieldsPage);
//    setPage(Page_Categories, new CategoriesPage);
//    setPage(Page_Tags, new TagsPage);
    ops = new QStandardItemModel;
    setPage(Page_Conclusion, new ConclusionPage(ops));

    setStartId(Page_Intro);

//    setWizardStyle(ModernStyle);

    setOption(HaveHelpButton, true);
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
    QDialog::accept();
}

QStandardItemModel* CSVImporterWizard::getOperations()
{
    return ops;
}


/////////////////////////////////////

IntroPage::IntroPage(const QString & filename, QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    topLabel = new QLabel(tr("This wizard will help you register your copy of "
                             "<i>Super Product One</i>&trade; or start "
                             "evaluating the product."));
    topLabel->setWordWrap(true);

    fileLabel = new QLabel(tr("F&ichier:"));
    qleFile = new QLineEdit;
//    qleFile->setText(filename);
    qleFile->setReadOnly(true);
    fileLabel->setBuddy(qleFile);

    qpbChooseFile = new QPushButton(tr("Choose..."));
    connect(qpbChooseFile, SIGNAL(clicked()), this, SLOT(chooseFile()));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(fileLabel);
    hbox->addWidget(qleFile);
    hbox->addWidget(qpbChooseFile);

    registerField("filename*", qleFile);
//    if (!filename.isEmpty())
//        setField("filename", filename);

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
        setField("firstOpLine",-1);
    }
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
    form->addRow(tr("Catégorie : "), qleCat);
    form->addRow(tr("Montant : "), qleAmount);
    form->addRow(tr("Etiquette : "), qleTag);
    form->addRow(tr("Description : "), qleDes);

    registerField("dateColumn*", qleDate);
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
//    return CSVImporterWizard::Page_Categories;
    return CSVImporterWizard::Page_Conclusion;
}

void FieldsPage::initializePage()
{
    QFile file(field("filename").toString());
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString text;

        QTextStream in(&file);
        int id_line = field("OpHeaderLine").toInt();
        int id = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (id++ == id_line) {
                QStringList headers = line.split(QLatin1Char(';'));
                for (int i = 1; i <= headers.size(); ++i) {
                    text += QString::number(i) + " " + headers.at(i-1) + "\n";
                }
                break;
            }
        }
        file.close();
        file.open(QFile::ReadOnly | QFile::Text);
        csvEditor->setPlainText(file.readAll());
        file.close();
        qlHeaders->setText(text);
    }
}

/////////////////////////////////////

ConclusionPage::ConclusionPage(QStandardItemModel *ops, QWidget *parent)
    : QWizardPage(parent),
      model(ops)
{
    setTitle(tr("Complete Your Registration"));
//    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);
    bottomLabel->setText("<u>Upgrade License Agreement:</u> "
                         "This software is licensed under the terms of your "
                         "current license.");

    model->setColumnCount(5);
    model->setHeaderData(0, Qt::Horizontal, tr("Date"));
    model->setHeaderData(1, Qt::Horizontal, tr("Category"));
    model->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(3, Qt::Horizontal, tr("Tag"));
    model->setHeaderData(4, Qt::Horizontal, tr("Description"));
    table = new QTableView;
    table->setModel(model);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->hide();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(table);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    QFile file(field("filename").toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int op_line = field("firstOpLine").toInt() < 0 ?
                field("OpHeaderLine").toInt() + 1 : field("firstOpLine").toInt();
    int iline = 1;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (iline++ < op_line)
            continue;

        process_line(line);
    }
}

void ConclusionPage::process_line(const QString& line)
{
    int dateCol = field("dateColumn").toInt() - 1;
    int amountCol = field("amountColumn").toInt() - 1;
    int desCol = field("desColumn").toInt() - 1;

    int catCol = field("catColumn").toInt() - 1;
    int tagCol = field("tagColumn").toInt() - 1;

    QStringList infos = line.split(QLatin1Char(';'));

    QStringList date = infos.at(dateCol).split(QLatin1Char('/'));
    QDate op_date(date.at(2).toInt(),date.at(1).toInt(),date.at(0).toInt());

    bool ok;
    double amount;

    QLocale german(QLocale::German);
    amount = german.toDouble(infos.at(amountCol), &ok);
    if (!ok) { // try with locale c
        QLocale c(QLocale::C);
        amount = c.toDouble(infos.at(amountCol), &ok);
    }

    QString des = infos.at(desCol);
    des.remove(QChar('"'));

    QString cat = catCol < 0 ? "" : infos.at(catCol);
    QString tag = tagCol < 0 ? "" : infos.at(tagCol);

    QList<QStandardItem *> items;
    items.append(new QStandardItem(op_date.toString("yyyy-MM-dd")));
    items.append(new QStandardItem(cat));
    items.append(new QStandardItem(QString::number(amount)));
    items.append(new QStandardItem(tag));
    items.append(new QStandardItem(des));
    model->appendRow(items);
}
