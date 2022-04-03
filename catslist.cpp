#include "catslist.h"

#include <QSqlQuery>
#include <QSqlError>
#include "catdialog.h"

CatsList::CatsList(QSqlTableModel * mod, QWidget *parent)
    : QDialog{parent}
    , model(mod)
{
    setWindowIcon(QIcon(":/images/images/category_48px.png"));
    setWindowTitle(tr("Gérer les catégories"));

    mainLayout = new QGridLayout(this);

    catsView = new QListWidget(this);

    QSqlQuery q;
    q.exec("SELECT * from categories");
    while (q.next())
        catsView->addItem(q.value(1).toString());

    catsView->setSortingEnabled(true);
    catsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mainLayout->addWidget(catsView, 0, 0, 5, 3);

    qpbAddNew = new QPushButton(QIcon(":/images/images/add_48px.png"),tr("New Category"),this);
    connect(qpbAddNew, SIGNAL(clicked()), this, SLOT(addNewCategory()));
    mainLayout->addWidget(qpbAddNew, 0, 3);

    qpbEdit = new QPushButton(QIcon(":/images/images/edit_48px.png"),tr("Edit Category"),this);
    connect(qpbEdit, SIGNAL(clicked()), this, SLOT(editCategory()));
    mainLayout->addWidget(qpbEdit, 1, 3);

    qpbRemove = new QPushButton(QIcon(":/images/images/Remove_48px.png"),tr("Remove Category"),this);
    connect(qpbRemove, SIGNAL(clicked()), this, SLOT(removeCategories()));
    mainLayout->addWidget(qpbRemove, 2, 3);
}

CatsList::~CatsList()
{
    catsView->deleteLater();
    qpbAddNew->deleteLater();
    qpbEdit->deleteLater();
    qpbRemove->deleteLater();
}

void CatsList::addNewCategory()
{
    qDebug() << "new category";
    CatDialog diag;
    if (diag.exec())
    {
        catsView->addItem(diag.name());

        int row = 0;
        model->insertRows(row, 1);
        model->setData(model->index(row, 0), catsView->count());
        model->setData(model->index(row, 1), diag.name());
        model->setData(model->index(row, 2), diag.color());
        model->setData(model->index(row, 3), diag.type());
        emit commit();
    }
    qDebug() << "end";
}

void CatsList::editCategory()
{
    qDebug() << "current row is : " << catsView->currentRow();
}

void CatsList::removeCategories()
{
    QItemSelectionModel *selectionModel = catsView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;

    foreach (index, indexes) {
        int row = index.row();
        qDebug() << row;
    }
}
