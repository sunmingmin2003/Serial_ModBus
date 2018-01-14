#include "ccsvfile.h"

CCSVFile::CCSVFile()
{

}

CCSVFile::~CCSVFile()
{

}

//获取列数
INT CCSVFile::GetColumnSize(void)
{
    return m_nColumnSize;
}

//获取csv 行数
INT CCSVFile::GetRowSize(void)
{
    return m_nRowSize;
}

//获取单元格数据
QString CCSVFile::GetCellData(INT nColumn,INT nRow)
{
    QString retStr;

    if(nColumn>-1 && nColumn<m_nColumnSize && nRow>-1 && nRow<m_nRowSize)
       {
          retStr = m_vecTable[nRow][nColumn].bModify ? m_vecTable[nRow][nColumn].strNewData : m_vecTable[nRow][nColumn].strSaveData;
       }

        return retStr;
}

INT CCSVFile::SetCellData(INT nColumn, INT nRow, const QString &data)
{
    if( nColumn >= m_nColumnSize  ||
            nRow    >= m_nRowSize     ||
            nColumn <  0              ||
            nRow    <  0              )
        {
            //ERRLOG("column or row index is over range");
            qDebug() << "column or row index is over range";
            return -1;
        }
        //if(Trim(data).empty())  //可以设置空值
        //{
        //	ERRLOG("can't set blank value to cell data");
        //	return -1;
        //}

        m_vecTable[nRow][nColumn].strNewData = data;
        m_vecTable[nRow][nColumn].bModify    = true;
        m_bModified = true;
        return 0;
}

INT CCSVFile::OpenCSVFile(const QString strFilePath)
{

   INT nRet = -1;
   INT nTmp     = 0;       //临时整型变量
   QString strValue;
   QString::size_type nStartPos = 0; //每行以","分割起始位置
   QString::size_type nEndPos   = 0; //每行以","分割结束位置
   rowData  vecRowData;
   cellData sCellData;

    QFile m_csvFile(strFilePath);

  if( m_csvFile.open(QIODevice::ReadWrite))
  {

      QTextStream stream(&m_csvFile);
      for(strValue = stream.readLine();!stream.atEnd();strValue = stream.readLine())//读一行
      {
        vecRowData.clear();
        nStartPos   = 0;
        nEndPos     = 0;
        while((nEndPos = strValue.indexOf(',',nStartPos)) != -1)
        {
            sCellData.strNewData.clear();
            sCellData.bModify = false;
            sCellData.strSaveData = strValue.mid(nStartPos, nEndPos-nStartPos);
            vecRowData.push_back(sCellData);
            nStartPos = nEndPos+1;
        }
        sCellData.strNewData.clear();
        sCellData.bModify = false;
        sCellData.strSaveData = strValue.mid(nStartPos);
        vecRowData.push_back(sCellData); //最后一段加入Vector中
        if(vecRowData.empty()) //如果本行数据为空，则继续解析
            {
                continue;
            }

        if(m_vecTable.empty())  //如果表中为空，则设置标题头的数为列数
            {
                 m_nColumnSize = vecRowData.size();
            }
        else  //不为空时，如果列数大于标题头，则错误，小于的话，自动补空
            {
                if(m_nColumnSize < (int)vecRowData.size())
                   {
                       //CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"[%s] [Row:%d]Data column size greater than title column size", __FUNCTION__,m_vecTable.size()+1);
                       qDebug()<<"Data column size greater than title column size";
                       nRet = -1;
                       goto END;
                    }
                else
                    {
                       sCellData.strSaveData.clear();
                       sCellData.strNewData.clear();
                       sCellData.bModify  = false;
                       nTmp = m_nColumnSize - vecRowData.size();
                       while(nTmp-- > 0)
                      vecRowData.push_back(sCellData);
                    }
            }
        m_vecTable.push_back(vecRowData);
      }//读一行结束

      m_nRowSize = m_vecTable.size();

      nRet = 0;
      END:
     // SAFE_DELETE_ARRAY(pszData);
      return nRet;
  }
      m_csvFile.close();
//  }

  return false;
}
