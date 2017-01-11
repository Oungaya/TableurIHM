#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gotocelldialog.h"
#include "FindDialog.h"

class QAction;
class QLabel;
class Spreadsheet;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void goToCell();
    void about();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();
    void onSearch();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Spreadsheet *spreadsheet;
    QLabel *locationLabel;
    QLabel *formulaLabel;
    QStringList recentFiles;
    QString curFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *goToCellAction;
    QAction *findAction;
    QAction *recalculateAction;
    QAction *showGridAction;
    QAction *autoRecalcAction;
    QAction *aboutAction;

    QMenu * sortSubMenu;
    QAction * sortSelectionAction;
    QAction * sortSelectionActionReversed;
    QAction * sortSelectionByLeftColumnAction;

    QMenu * selectSubMenu;
    QAction * selectRowAction;
    QAction * selectColumnAction;
    QAction * selectAllAction;

    GoToCellDialog *dialog;
    FindDialog * findDialog;
};

#endif
