 #include "cell.h"
#include "spreadsheet.h"

#include <QtGui>
#include <QMessageBox>
#include <QApplication>
#include <QMap>

Spreadsheet::Spreadsheet(QWidget *parent)
    : QTableWidget(parent)
{
    autoRecalc = true;

    setItemPrototype(new Cell);
    setSelectionMode(ContiguousSelection);

    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(somethingChanged()));
    clear();
}

QString Spreadsheet::currentLocation() const
{
    return QChar('A' + currentColumn())
           + QString::number(currentRow() + 1);
}

QString Spreadsheet::currentFormula() const
{
    return formula(currentRow(), currentColumn());
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

void Spreadsheet::clear()
{
    setRowCount(0);
    setColumnCount(0);
    setRowCount(RowCount);
    setColumnCount(ColumnCount);

    for (int i = 0; i < ColumnCount; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A' + i)));
        setHorizontalHeaderItem(i, item);
    }

    setCurrentCell(0, 0);
}

bool Spreadsheet::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_1);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("The file is not a Spreadsheet file."));
        return false;
    }

    clear();

    quint16 row;
    quint16 column;
    QString str;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (!in.atEnd()) {
        in >> row >> column >> str;
        setFormula(row, column, str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_1);

    out << quint32(MagicNumber);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            QString str = formula(row, column);
            if (!str.isEmpty())
                out << quint16(row) << quint16(column) << str;
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void Spreadsheet::cut()
{
    copy();
    del();
}

void Spreadsheet::copy()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str;

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

void Spreadsheet::paste()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;

    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Spreadsheet"),
                tr("The information cannot be pasted because the copy "
                   "and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < RowCount && column < ColumnCount)
                setFormula(row, column, columns[j]);
        }
    }
    somethingChanged();
}

void Spreadsheet::del()
{
    foreach (QTableWidgetItem *item, selectedItems())
        delete item;
}

void Spreadsheet::recalculate()
{
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            if (cell(row, column))
                cell(row, column)->setDirty();
        }
    }
    viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool recalc)
{
    autoRecalc = recalc;
    if (autoRecalc)
        recalculate();
}

void Spreadsheet::somethingChanged()
{
    if (autoRecalc)
        recalculate();
    emit modified();
}

Cell *Spreadsheet::cell(int row, int column) const
{
    return static_cast<Cell *>(item(row, column));
}

void Spreadsheet::setFormula(int row, int column,
                             const QString &formula)
{
    Cell *c = cell(row, column);
    if (!c) {
        c = new Cell;
        setItem(row, column, c);
    }
    c->setFormula(formula);
}

QString Spreadsheet::formula(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->formula();
    } else {
        return "";
    }
}

QString Spreadsheet::text(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->text();
    } else {
        return "";
    }
}

void Spreadsheet::selectRow()
{
    QTableWidget::selectRow(currentRow());
}

void Spreadsheet::selectColumn()
{
    QTableWidget::selectColumn(currentColumn());
}

void Spreadsheet::searchCell(QString textToFind)
{
    //setSelectionBehavior(QAbstractItemView::ExtendedSelection);

    int i=0,j=0;
    bool found = false;
    while(i <= RowCount && !found){
        while(j <= ColumnCount && !found) {
            if(text(i,j).contains(textToFind, Qt::CaseSensitive)){
                found = true;
                clearSelection();
                setCurrentCell(i, j);
                activateWindow();
                //lastItemsSearched = this->findItems(textToFind, Qt::CaseInsensitive);
                //selectionModel()->select(model()->index(i,j), QItemSelectionModel::Select);
            }
            j++;
        }
        i++;
    }
    if(!found) {
        QMessageBox box;
        box.setText("Expression not found");
        box.exec();
    }
}

void Spreadsheet::sortSelectedItems()
{
    QList<QTableWidgetItem*> items = selectedItems();
    QStringList itemsValues;
    foreach(QTableWidgetItem *item, items) {
        itemsValues.append(item->text());
    }
    qSort(itemsValues);
    int i=0;
    if(items.size() == itemsValues.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(itemsValues.at(i));
            i++;
        }
    }
}

void Spreadsheet::sortSelectedItemsReversed()
{
    QList<QTableWidgetItem*> items = selectedItems();
    QStringList itemsValues;
    foreach(QTableWidgetItem *item, items) {
        itemsValues.append(item->text());
    }
    qSort(itemsValues);
    int i = itemsValues.size()-1;
    if(items.size() == itemsValues.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(itemsValues.at(i));
            i--;
        }
    }
}

void Spreadsheet::sortSelectedItemsByLeftColumn()
{
    QStringList first, final;
    QMap<QString, QStringList> map;
    QList<QTableWidgetItem*> items = selectedItems();
    QTableWidgetSelectionRange range = selectedRange();
    foreach(QTableWidgetItem *item, items) {
        if(item->column() == range.leftColumn()) {
            first.append(item->text());
            QStringList list;
            foreach(QTableWidgetItem * item2, items) {
                if(item2->row() == item->row() && item2->column() != item->column())
                    list.append(item2->text());
            }
            map.insert(item->text(), list);
        }
    }
    qSort(first);
    foreach(QString value, first) {
        final.append(value);
        final.append(map.value(value));
    }
    int i=0;
    if(items.size() == final.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(final.at(i));
            i++;
        }
    }
}

void Spreadsheet::sortSelectedItemsByLeftRow()
{
    QStringList first, final;
    QMap<QString, QStringList> map;
    QList<QTableWidgetItem*> items = selectedItems();
    QTableWidgetSelectionRange range = selectedRange();
    foreach(QTableWidgetItem *item, items) {
        if(item->row() == range.topRow()) {
            first.append(item->text());
            QStringList list;
            foreach(QTableWidgetItem * item2, items) {
                if(item2->column() == item->column() && item2->row() != item->row())
                    list.append(item2->text());
            }
            map.insert(item->text(), list);
        }
    }
    qSort(first);
    foreach(QString value, first) {
        final.append(value);
        final.append(map.value(value));
    }
    int i=0;
    if(items.size() == final.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(final.at(i));
            i++;
        }
    }
}

void Spreadsheet::sortSelectedItemsByColumn(int column)
{
    QStringList first, final;
    QMap<QString, QStringList> map;
    QList<QTableWidgetItem*> items = selectedItems();
    //QTableWidgetSelectionRange range = selectedRange();
    foreach(QTableWidgetItem *item, items) {
        if(item->column() == column) {
            first.append(item->text());
            QStringList list;
            foreach(QTableWidgetItem * item2, items) {
                if(item2->row() == item->row() && item2->column() != item->column())
                    list.append(item2->text());
            }
            map.insert(item->text(), list);
        }
    }
    qSort(first);
    foreach(QString value, first) {
        final.append(value);
        final.append(map.value(value));
    }
    int i=0;
    if(items.size() == final.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(final.at(i));
            i++;
        }
    }
}

void Spreadsheet::sortSelectedItemsByRow(int row)
{
    QStringList first, final;
    QMap<QString, QStringList> map;
    QList<QTableWidgetItem*> items = selectedItems();
    //QTableWidgetSelectionRange range = selectedRange();
    foreach(QTableWidgetItem *item, items) {
        if(item->row() == row) {
            first.append(item->text());
            QStringList list;
            foreach(QTableWidgetItem * item2, items) {
                if(item2->column() == item->column() && item2->row() != item->row())
                    list.append(item2->text());
            }
            map.insert(item->text(), list);
        }
    }
    qSort(first);
    foreach(QString value, first) {
        final.append(value);
        final.append(map.value(value));
    }
    int i=0;
    if(items.size() == final.size()) {
        foreach(QTableWidgetItem *item, items) {
            item->setText(final.at(i));
            i++;
        }
    }
}

