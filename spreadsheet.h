#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <QTableWidget>

class Cell;

class Spreadsheet : public QTableWidget
{
    Q_OBJECT

public:
    Spreadsheet(QWidget *parent = 0);

    bool autoRecalculate() const { return autoRecalc; }
    QString currentLocation() const;
    QString currentFormula() const;
    QTableWidgetSelectionRange selectedRange() const;
    void clear();
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    void searchCell(QString textToFind);

public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void recalculate();
    void setAutoRecalculate(bool recalc);
    void selectRow();
    void selectColumn();
    void sortSelectedItems();
    void sortSelectedItemsReversed();
    void sortSelectedItemsByLeftColumn();

signals:
    void modified();

private slots:
    void somethingChanged();

private:
    enum { MagicNumber = 0x7F51C883, RowCount = 999, ColumnCount = 26 };

    QList<QTableWidgetItem *> lastItemsSearched;
    Cell *cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);

    bool autoRecalc;
};

#endif
