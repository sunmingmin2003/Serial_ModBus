#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QMenu>
#include <QStatusBar>
#include <QMenuBar>

#include "spreadsheet.h"
#include "settingsdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    spreadsheet = new Spreadsheet;
   // setCentralWidget(spreadsheet);
   // spreadsheet->setColumnCount(spreadsheet->m_ParamData.m_strListHeader.count());
   // spreadsheet->setHorizontalHeaderLabels( spreadsheet->m_ParamData.m_strListHeader/*QStringList() << tr("X")<<tr("Y")*/);

    createActions();
    createMenus();
    settings = new SettingsDialog;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
/***************** Action belong to File Menu **********************************/

    newAction = new QAction(tr("&New"),this);
    newAction->setIcon(QIcon(":/images/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new spreadsheet file"));
    connect(newAction,SIGNAL(triggered()),this,SLOT(newFile()));

    openAction = new QAction(tr("&Open"),this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a spreadsheet file"));
    connect(newAction,SIGNAL(triggered()),this,SLOT(open()));

    saveAction = new QAction(tr("&Save"),this);


    saveAsAction = new QAction(tr("&SaveAs"),this);


    for(int i=0; i<MaxRecentFiles;i++)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i],SIGNAL(triggered()),
                this,SLOT(openRecentFile()));
    }

    exitAction = new QAction(tr("E&xit"),this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction,SIGNAL(triggered()),
            this,SLOT(close()));

/***************** Action belong to Edit Menu **********************************/
    cutAction = new QAction(tr("Cut"),this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut the Content"));
   // connect(cutAction,SIGNAL(triggered()),
    //        this, SLOT(cut()));

    copyAction = new QAction(tr("Copy"),this);

    pasteAction = new QAction(tr("Paste"),this);

    deleteAction = new QAction(tr("Delete"),this);

    selectRowAction = new QAction(tr("Row"),this);
    selectColumnAction = new QAction(tr("Column"),this);

    selectAllAction = new QAction(tr("&All"),this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Select all the cells in the "
                                     "spreadsheet"));
    connect(selectAllAction,SIGNAL(triggered()),
            spreadsheet,SLOT(selectAll()));

    findAction = new QAction(tr("Find"),this);
    goToCellAction = new QAction(tr("Go to Cell"),this);
/*******************Tools*******************************/
    recalcAction = new QAction(tr("Recalculate "),this);
    sortAction = new QAction(tr("Sort"),this);
/********************Options*********************************/
    showGridAction = new QAction(tr("&Show Grid"),this);
    showGridAction->setCheckable(true);
    showGridAction->setChecked(spreadsheet->showGrid());
    showGridAction->setStatusTip(tr("Show or hide the spreadsheet's" "grid"));
    connect(showGridAction,SIGNAL(toggled(bool)),
            spreadsheet,SLOT(setShowGrid(bool)));

    autoRecalcAtion = new QAction(tr("Auto Recalculate"),this);
/*******************Help*********************/
    aboutAction = new QAction;

    aboutQtAction = new QAction;
    aboutQtAction->setStatusTip(tr("Show the Qt libary's About box"));
    connect(aboutQtAction,SIGNAL(triggered(bool)),
            qApp,SLOT(aboutQt()));

    /****************************************/
    connectAction    = new QAction(tr("Connect"),this);
  //  connect(connectAction,&QAction::triggered,
  //          this,&MainWindow::openSerialPort);
    disconnectAction  =new QAction(tr("DisConnect"),this);

    comportsetAction = new QAction(tr("Setup Com"),this);
    comportsetAction->setStatusTip(tr("Setup Comport"));
    connect(comportsetAction,SIGNAL(triggered(bool)),
            this,SLOT(openSerialPort()));

    paramAction = new QAction(tr("Parameter"),this);
    connect(paramAction,&QAction::triggered,
            this,&MainWindow::openParamDlg);
}

void MainWindow::createMenus()
{
/*****************  File Menu **********************************/
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    separatorAction = fileMenu->addSeparator();
    for(int i=0;i<MaxRecentFiles;i++)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
/*****************  Edit Menu **********************************/

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);

    selectSubMenu = editMenu->addMenu(tr("Select"));
    selectSubMenu->addAction(selectRowAction);
    selectSubMenu->addAction(selectColumnAction);
    selectSubMenu->addAction(selectAllAction);

    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(goToCellAction);

/*****************  Tool Menu **********************************/

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(recalcAction);
    toolsMenu->addAction(sortAction);

/*****************  Option Menu **********************************/

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(showGridAction);
    optionsMenu->addAction(autoRecalcAtion);

    menuBar()->addSeparator();
/***********************************************/

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

/***********************************************/
    comportMenu = menuBar()->addMenu(tr("&RS232"));
    comportMenu->addAction(connectAction);
    comportMenu->addAction(disconnectAction);
    comportMenu->addAction(comportsetAction);
    comportMenu->addAction(paramAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(findAction);
    editToolBar->addAction(goToCellAction);
}

void MainWindow::createContextMenu()
{
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(pasteAction);
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::createStatusBar()
{
  locationLabel = new QLabel("W999");
  locationLabel->setAlignment(Qt::AlignCenter);
  locationLabel->setMinimumSize(locationLabel->sizeHint());

  formulaLabel = new QLabel;
  formulaLabel->setIndent(3);

  statusBar()->addWidget(locationLabel);
  statusBar()->addWidget(formulaLabel,1);

  connect(spreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),
          this,SLOT(updateStatusBar()));
  connect(spreadsheet,SIGNAL(modified()),
          this,SLOT(spreadsheetModified()));
  updateStatusBar();
}

void MainWindow::updateStatusBar()
{
   // locationLabel->setText(spreadsheet->currentLocation());
}

void MainWindow::open()
{

}

void MainWindow::newFile()
{
    if(okToContinue())
    {
       // spreadsheet->clear();
        setCurrentFile("");
    }
}

bool MainWindow::save()
{
    if(curFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
   return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
   // if(!spreadsheet->writeFile(fileName))
    {
        statusBar()->showMessage(tr("Saving canceled"),2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"),2000);
    return true;
}

void MainWindow::openRecentFile()
{

}
void MainWindow::setCurrentFile(const QString &fileName)
{

}

bool MainWindow::okToContinue()
{
    return true;
}

void MainWindow::spreadsheetModified()
{

}

void MainWindow::find()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{

}

void MainWindow::goToCell()
{

}

void MainWindow::sort()
{

}

void MainWindow::about()
{

}

void MainWindow::openSerialPort()
{
    //settings = new SettingsDialog;
    settings->show();
    SettingsDialog::Settings p = settings->settings();
}

void MainWindow::openParamDlg()
{
   CParamDlg *pParamDlg = new CParamDlg();
   pParamDlg->show();
   // CParamDlg ParamDlg;
   // ParamDlg.show();
}
