#ifndef SORTDIALOG_H
#define SORTDIALOG_H

#include <QDialog>
#include "ui_sortdialog.h"


namespace Ui {
class SortDialog;
}

class SortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SortDialog(QWidget *parent = 0);
    ~SortDialog();
    QString getText() {return ui->ColOrLineEdit->text();}
    bool byLine() {return ui->ByLineBtn->isChecked();}
    bool byCulumn() {return ui->ByColBtn->isChecked();}
    void setWrong(QString value) {ui->Wrong->setText(value);}
private:
    Ui::SortDialog *ui;
};

#endif // SORTDIALOG_H
