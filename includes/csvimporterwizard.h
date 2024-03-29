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
#include "sqlrelationalmodel.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QGroupBox;
class QComboBox;
class QTableWidget;
class QAbstractItemModel;
class QFormLayout;
class QSqlTableModel;

QT_END_NAMESPACE

struct Operation
{
    QDate date;
    int cat;
    float amount;
    int tag;
    QString description;
};

typedef QMap<QString,QPair<int,QString> > GroupsMap;
typedef QVector<Operation> OperationsVector;

namespace Ui {
class LinePage;
class FieldsPage;
}

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

    CSVImporterWizard(SqlRelationalTableModel * mod, const QString & filename = "", QWidget *parent = nullptr);
    void accept() override;

    void setNbOperations(int nb);
    int getNbOperations() const;
    OperationsVector* getOperations();
    GroupsMap* matchedCategories();
    GroupsMap* matchedTags();

private:
    int nbOps;
    OperationsVector *ops;
    QSqlTableModel * catsModel;
    QSqlTableModel * tagsModel;
    GroupsMap catsMap;
    GroupsMap tagsMap;
};


class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(const QString & filename = "", QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;

private slots:
    void chooseFile();

private:
    QLabel *topLabel;
    QLabel *fileLabel;
    QLineEdit   *qleFile;
    QPushButton *qpbChooseFile;
//    QRadioButton *newConfigRadioButton;
//    QRadioButton *useConfigRadioButton;
//    QComboBox *configsList;

    QString _file;
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
    void updateOpLine(const QString &);

private:
    Ui::LinePage *ui;
};


class FieldsPage : public QWizardPage
{
    Q_OBJECT

public:
    FieldsPage(QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;

private:
    Ui::FieldsPage *ui;
};


class CategoriesPage : public QWizardPage
{
    Q_OBJECT

public:
    CategoriesPage(QSqlTableModel *mod, QWidget *parent = nullptr);
    void initializePage() override;
    int nextId() const override;

private:
    QFormLayout *form;
    QSqlTableModel *cats;
};


class TagsPage : public QWizardPage
{
    Q_OBJECT

public:
    TagsPage(QSqlTableModel *mod, QWidget *parent = nullptr);
    void initializePage() override;
    int nextId() const override;

private:
    QFormLayout *form;
    QSqlTableModel *tags;
};


class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;

    QTableWidget * table();

private:
    QTableWidget *tableWidget;

    void process_line(int row, GroupsMap*, GroupsMap*, const QString&);
};

#endif // CSVIMPORTERWIZARD_H
