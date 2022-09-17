#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QDataWidgetMapper>
#include <QSqlTableModel>

class filtersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit filtersWidget(QWidget *parent = nullptr);
    ~filtersWidget();

    void populateComboBoxes(QSqlTableModel * cats, QSqlTableModel * tags);
    void initComboBoxes();

public slots:
    void buildStatement();

signals:
    void statementBuilt(const QString &);

private slots:
    void reset();

private:
    QDateEdit *qdeDateFrom;
    QDateEdit *qdeDateTo;
    QLineEdit *qleSearch;
    QLineEdit *qleMinAmount;
    QLineEdit *qleMaxAmount;
    QComboBox *qcbCat;
    QComboBox *qcbTag;

    QDataWidgetMapper *mapper = nullptr;

};

#endif // FILTERSWIDGET_H
