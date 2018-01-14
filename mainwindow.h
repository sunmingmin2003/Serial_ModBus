#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QtCore/QtGlobal>

#include "spreadsheet.h"
#include "settingsdialog.h"
#include "cparamdlg.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void goToCell();
    void sort();
    void about();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();

    void openSerialPort();
    void openParamDlg();

private:
    Ui::MainWindow *ui;

    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFile();
    QString strippedName(const QString &fullName);

    Spreadsheet *spreadsheet;
  //  QFindDialog *findDialg;
    QLabel  *locationLabel;
    QLabel  *formulaLabel;
    QStringList recentFiles;
    QString curFile;



    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QMenu *comportMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
/*****************  *******************/
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;

    enum {MaxRecentFiles = 5};
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QAction *exitAction;
/********************************/

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectSubAction;
    QAction *selectRowAction;
    QAction *selectColumnAction;
    QAction *selectAllAction;
    QAction *findAction;
    QAction *goToCellAction;



    QAction *recalcAction;
    QAction *sortAction;
 /***********Option action*********************/
    QAction *showGridAction;
    QAction *autoRecalcAtion;
 /***********Help action*********************/
    QAction *aboutAction;
    QAction *aboutQtAction;
/************COM*****************************/
    QAction *connectAction;
    QAction *disconnectAction;
    QAction *comportsetAction;
    QAction *paramAction;

    /*************************************/
    SettingsDialog *settings;
  // CParamDlg pParamDlg;
};

#endif // MAINWINDOW_H
