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

#ifndef CSVIMPORTERWIZARD_H
#define CSVIMPORTERWIZARD_H

#include <QWizard>
#include <QDate>

#include "csveditor.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QGroupBox;
class QComboBox;
class QTableView;
class QStandardItemModel;
QT_END_NAMESPACE

class CSVImporterWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro,
           Page_Line,
           Page_Fields,
           Page_Categories,
           Page_Tags,
           Page_Conclusion };

    CSVImporterWizard(const QString & filename = "", QWidget *parent = nullptr);
    void accept() override;
    QStandardItemModel* getOperations();

private slots:
    void showHelp();

private:
    QStandardItemModel *ops;

};


class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(const QString & filename = "", QWidget *parent = nullptr);
    int nextId() const override;

private slots:
    void chooseFile();

private:
    QLabel *topLabel;
    QLabel *fileLabel;
    QLineEdit   *qleFile;
    QPushButton *qpbChooseFile;
    QRadioButton *newConfigRadioButton;
    QRadioButton *useConfigRadioButton;
    QComboBox *configsList;
};


class LinePage : public QWizardPage
{
    Q_OBJECT

public:
    LinePage(QWidget *parent = nullptr);

    int nextId() const override;

    void initializePage() override;

private slots:
    void handleOpLine(int);

private:
    QLabel *topLabel;
    CSVEditor *csvEditor;
    QLabel *lineLabel;
    QLineEdit *qleLineSelected;
    QCheckBox *enterOpLine;
    QLineEdit *qleOpLine;
};


class FieldsPage : public QWizardPage
{
    Q_OBJECT

public:
    FieldsPage(QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;

private:
    QLineEdit *qleDate;
    QLineEdit *qleCat;
    QLineEdit *qleAmount;
    QLineEdit *qleTag;
    QLineEdit *qleDes;
    QLabel *qlHeaders;
    CSVEditor *csvEditor;
};


//class CategoriesPage : public QWizardPage
//{
//    Q_OBJECT

//public:
//    CategoriesPage(QWidget *parent = nullptr);

//    int nextId() const override;

//private:
//    QLabel *nameLabel;
//    QLabel *upgradeKeyLabel;
//    QLineEdit *nameLineEdit;
//    QLineEdit *upgradeKeyLineEdit;
//};


//class TagsPage : public QWizardPage
//{
//    Q_OBJECT

//public:
//    TagsPage(QWidget *parent = nullptr);

//    int nextId() const override;

//private:
//    QLabel *companyLabel;
//    QLabel *emailLabel;
//    QLabel *postalLabel;
//    QLineEdit *companyLineEdit;
//    QLineEdit *emailLineEdit;
//    QLineEdit *postalLineEdit;
//};


class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QStandardItemModel *ops, QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;

    struct Operation
    {
        QDate date;
        int cat;
        double amount;
        int tag;
        QString description;
    };

private:
    QLabel *bottomLabel;

    QVector<Operation> operations;
    QStandardItemModel *model;
    QTableView *table;

    void process_line(const QString&);
};

#endif // CSVIMPORTERWIZARD_H
