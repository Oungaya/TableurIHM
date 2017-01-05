#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include "ui_finddialog.h"

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = 0);
    QString getText() {return ui->lineEdit_2->text();}

    ~FindDialog();

private:
    Ui::FindDialog *ui;

};

#endif // FINDDIALOG_H
