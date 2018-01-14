#ifndef CPARAMDLG_H
#define CPARAMDLG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QGroupBox>

#include <spreadsheet.h>

class CParamDlg : public QDialog
{
    Q_OBJECT

public:
    CParamDlg( QDialog *parent=0);

private:
    void createAction();
    void createMenu();
    void createToolBar();
    void createHorizontGroup();

    QPushButton *readButton;
    QPushButton *writeButton;
    Spreadsheet *spreadsheet;

    QAction  *openfileAction;
    QAction  *savefileAction;

    QAction  *readselectAction;
    QAction  *readallaction;
    QAction  *writeselectAction;
    QAction  *writeallAction;

    QMenuBar  *menuBar;
    QMenu     *fileMenu;
    QMenu     *serilportMenu;

    QToolBar *fileToolBar;

    QToolBar *serialportToolBar;

    enum { NumGridRows = 3, NumButtons = 4 };

    QGroupBox * horizontalGroupBox;
    QPushButton *buttons[NumButtons];
};

#endif // CPARAMDLG_H
