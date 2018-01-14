#include "cparamdlg.h"

CParamDlg::CParamDlg(QDialog *parent):QDialog(parent)
{
    setWindowTitle(tr("Parameter"));
    resize(1000,1000);
   // setFixedHeight(sizeHint().height());
    createAction();
    createToolBar();
    createMenu();
    createHorizontGroup();

   spreadsheet = new Spreadsheet(this);
  // spreadsheet->setColumnCount(spreadsheet->m_ParamData.m_strListHeader.count());
  // spreadsheet->setHorizontalHeaderLabels( spreadsheet->m_ParamData.m_strListHeader/*QStringList() << tr("X")<<tr("Y")*/);

    //setCentralWidget(spreadsheet);
   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->setMenuBar(menuBar);
   mainLayout->addWidget(horizontalGroupBox);
   mainLayout->addWidget(spreadsheet);
   setLayout(mainLayout);
}

void CParamDlg::createAction()
{
    openfileAction = new QAction(tr("OpenFile"),this);
    readselectAction = new QAction(tr("ReadSelect"),this);
}

void CParamDlg::createMenu()
{
  menuBar = new QMenuBar;

  fileMenu = new QMenu(tr("File"),this);
  fileMenu->addAction(openfileAction);

  menuBar->addMenu(fileMenu);
}

void CParamDlg::createToolBar()
{

    fileToolBar =new  QToolBar(tr("&File"),this);
    fileToolBar->addAction(openfileAction);
    //fileToolBar->addSeparator();
    fileToolBar->addAction(readselectAction);

}


void CParamDlg::createHorizontGroup()
{
    horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *layout = new QHBoxLayout;

    for (int i = 0; i < NumButtons; ++i) {
        buttons[i] = new QPushButton(tr("Button %1").arg(i + 1));
        layout->addWidget(buttons[i]);
    }
    horizontalGroupBox->setLayout(layout);
}
