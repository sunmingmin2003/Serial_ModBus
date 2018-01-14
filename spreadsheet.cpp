#include "spreadsheet.h"

#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QTableWidgetItem>
#include <QClipboard>
#include <QTableWidgetSelectionRange>

Spreadsheet::Spreadsheet(QWidget *parent):QTableWidget(parent)
{
  SParamData    sData;

  autoRecalc = true;
  setItemPrototype(new Cell);
  setSelectionMode(ContiguousSelection);
  verticalHeader()->setDefaultSectionSize(20);
  horizontalHeader()->setStyleSheet(QStringLiteral("background: yellow"));
  setAlternatingRowColors(true);
  setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
 // horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  verticalHeader()->setStyleSheet
  ("QHeaderView::section {background-color:darkcyan;"
  "color: red;padding-left: 4px;border: 1px solid #6c6c6c;}");

  horizontalHeader()->setStyleSheet
  ("QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
  "stop:0 #616161, stop: 0.5 yellow,stop: 0.6 green, stop:1 cyan);"
  "color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
  "QHeaderView::section:checked{background-color: red;}"
  "QHeaderView::down-arrow {image: url(down_arrow.png);}"
  "QHeaderView::up-arrow {image: url(up_arrow.png);}");

  CCheckBoxHeaderView *myHeader = new CCheckBoxHeaderView(0, Qt::Horizontal/*Qt::Vertical*/, this);
  setHorizontalHeader(myHeader);

  connect(myHeader,SIGNAL(checkStausChange(bool)),this,SLOT(checkStatusAllSlots(bool)));

  connect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
          this,SLOT(somethingChanged()));


  clear();

  QTableWidgetItem *item=new QTableWidgetItem("path");
  item->setCheckState(Qt::Unchecked);
  item->setIcon(QIcon("path"));
  setItem(0,0,item);

  m_cParamData.OpenParamListFile("D:/Qt_Project/Serial_ModBus/ACM_Prlist.csv");

  int nRow;
  int nColum;
  nRow = 900; //= m_cParamData.m_fpCSV.GetRowSize();
  nColum = 12 ;//m_cParamData.m_fpCSV.GetColumnSize();

  for(int i=0;i< nRow;i++)
  {   sData =m_cParamData.GetDataByIndex(i);
      //for( int j=0;j<nColum;j++)
      {
        //setItem(nRow,nColum,new QTableWidgetItem("C"));
          QModelIndex index = model()->index(i,0);
          model()->setData(index,sData.prNo,Qt::DisplayRole);

          QModelIndex index1 = model()->index(i,1);
          model()->setData(index1,sData.address,Qt::DisplayRole);

          QModelIndex index2 = model()->index(i,2);
          model()->setData(index2,sData.description,Qt::DisplayRole);

          QModelIndex index3 = model()->index(i,3);
          model()->setData(index3,sData.unit,Qt::DisplayRole);

          QModelIndex index4 = model()->index(i,4);
          model()->setData(index4,sData.data,Qt::DisplayRole);

          QModelIndex index5 = model()->index(i,5);
          model()->setData(index5,sData.defaultData,Qt::DisplayRole);

          QModelIndex index6 = model()->index(i,6);
          model()->setData(index6,sData.min,Qt::DisplayRole);

          QModelIndex index7 = model()->index(i,7);
          model()->setData(index7,sData.max,Qt::DisplayRole);

          QModelIndex index8 = model()->index(i,8);
          model()->setData(index8,sData.attribute,Qt::DisplayRole);
         // setItem(0,0,item);
      }
  }
  //setModel();
  //setItemDelegate();
}

void Spreadsheet::clear(void)
{
  setRowCount(0);
  setColumnCount(0);
  setRowCount(RowCount);
  setColumnCount(ColumuCount);

  for(int i =0; i<ColumuCount;++i)
  {
      QTableWidgetItem *item = new QTableWidgetItem;
      item->setText(QString(QChar('A'+i)));
      setHorizontalHeaderItem(i,item);
  }
  setCurrentCell(0,0);
}

Cell *Spreadsheet::cell(int row,int column) const
{
    return static_cast<Cell *>(item(row,column));
}

QString Spreadsheet::text(int row,int column) const
{
    Cell *c = cell(row,column);
    if(c)
    {
       return c->text();
    }
    else
    {
        return "";
    }
}

QString Spreadsheet::formula(int row,int column) const
{
    Cell *c = cell(row,column);
    if(c)
    {
        return c->formula();
    }
    else
    {
        return "";
    }
}

void Spreadsheet::setFormula(int row, int column, const QString &formula)
{
    Cell *c = cell(row,column);
    if(!c)
    {
        c = new Cell;
        setItem(row,column,c);
    }
    c->setFormula(formula);

}

bool Spreadsheet::writeFile(const QString &fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::WriteOnly))
  {
      QMessageBox::warning(this,tr("Spreadsheet"),
                           tr("Cannot write file %1:\n%2")
                           .arg(file.fileName()).arg(file.error()));
      return false;
  }

  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_5_9);

  out << quint32(MagicNumber);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  for(int row = 0; row < RowCount; row++)
  {
      for(int column = 0; column < ColumuCount; column++)
      {
          QString str = formula(row,column);
          if(!str.isEmpty())
              out << quint16(row) << quint16(column)<<str;
      }
  }
  QApplication::restoreOverrideCursor();
  return true;
}

bool Spreadsheet::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,tr("Spreadsheet"),
                             tr("Cannot read file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_9);

    quint32 magic;
    in >> magic;
    in >>magic;
    if(magic!= MagicNumber)
    {
        QMessageBox::warning(this,tr("Spreadsheet"),
                             tr("The file is not a Spreadsheet file."),
                             QMessageBox::NoButton,QMessageBox::Ok,QMessageBox::Yes);
        return false;
    }

    quint16 row;
    quint16 column;
    QString str;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    while(!in.atEnd())
    {
        in >> row >> column >> str;
        setFormula(row,column,str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

QString Spreadsheet::currentLocation()const
{
    return QChar('A'+ currentColumn()) + QString::number(currentRow());
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

   for(int i=0; i < range.rowCount();++i)
   {
       if(i>0)
           str += "\n";
       for(int j=0; j< range.columnCount();++j)
       {
           if(j>0)
               str += "\t";
           str += formula(range.topRow()+i, range.leftColumn()+j);
       }
   }
   QApplication::clipboard()->setText(str);
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if(ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

void Spreadsheet::paste()
{
   QTableWidgetSelectionRange range = selectedRange();
   QString str = QApplication::clipboard()->text();
   QStringList rows = str.split('\n');
   int numRows = rows.count();
   int numColumns = rows.first().count('\t')+1;

   if((range.rowCount()*range.columnCount()!=1)
           &&(range.rowCount()!= numRows || range.columnCount()!=numColumns))
   {
       QMessageBox::information(this,tr("Spreadsheet"),
                                tr("The information cannot be pasted because the copy"
                                   "and paste areas aren't the same size."));
       return;
   }

   for(int i=0;i< numRows;++i)
   {
       QStringList colums = rows[i].split('\t');
       for(int j =0;j<numColumns;++j)
       {
           int row = range.topRow()+i;
           int column = range.leftColumn() + j;
           if(row < RowCount && column < ColumuCount)
               setFormula(row,column,colums[j]);
       }
   }
   somethingChanged();
}

void Spreadsheet::del()
{
    QList<QTableWidgetItem *> items = selectedItems();
    if(!items.isEmpty())
    {
        foreach(QTableWidgetItem *item,items)
            delete item;
        somethingChanged();
    }
}

void Spreadsheet::selectCurrentRow()
{
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColum()
{
    selectColumn(currentColumn());
}

void Spreadsheet::findNext(const QString &str,Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn()+1;

    while(row < RowCount)
    {
        while(column < ColumuCount)
        {
          if(text(row,column).contains(str,cs))
          {
              clearSelection();
              setCurrentCell(row,column);
              activateWindow();
              return;
          }
          ++column;
        }
        column = 0;
        ++row;
    }
    QApplication::beep();
}

void Spreadsheet::findPrevious(const QString &str,Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn()+1;

    while(row > 0)
    {
        while(column >0)
        {
          if(text(row,column).contains(str,cs))
          {
              clearSelection();
              setCurrentCell(row,column);
              activateWindow();
              return;
          }
          --column;
        }
        column = 0;
        --row;
    }
    QApplication::beep();
}

void Spreadsheet::recalculate()
{
   for(int row = 0; row < RowCount; ++row)
   {
       for(int column=0; column < ColumuCount;++ column)
       {
           if(cell(row,column))
               cell(row,column)->setDirty();
       }
   }
   viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool recalc)
{
    autoRecalc = recalc;
    if(autoRecalc)
        recalculate();
}

void Spreadsheet::sort(const SpreadsheetCompare &compare)
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();
    int i;

    for(i = 0; i < range.rowCount();++i)
    {
        QStringList row;
        for(int j = 0; j < range.columnCount();++j )
        {
            row.append(formula(range.topRow() + i,
                               range.leftColumn()));
        }

    }
}

void Spreadsheet::somethingChanged()
{

}
/*
void setData(int nRow, int nColumn,int role, const QVariant &Value)
{
   // Cell *c =;
   // QTableWidgetItem *item = new QTableWidgetItem(QString(Value));
    //setItem(item);
}
*/
