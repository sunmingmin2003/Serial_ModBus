#ifndef CCSVFILE_H
#define CCSVFILE_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "project_define.h"

typedef struct _cellData
{
    QString  strSaveData;
    QString  strNewData;
    bool     bModify;
}cellData;

typedef QVector<cellData> rowData;
//QString FILE_PATH("D:/Qt_Project/Serial_ModBus/ACM_Prlist.csv");

class CCSVFile
{
public:
    CCSVFile();
    ~CCSVFile();

    // 打开CSV文件
    INT OpenCSVFile(const QString strFilePath);

    //获取列数
    INT GetColumnSize(void);

    //获取行数
    INT GetRowSize(void);

    //获取单元格数据
    QString GetCellData(INT nColumn,INT nRow);

    //设置单元格数据
    INT SetCellData(INT nColumn, INT nRow, const QString &data );

    //新增一行
    INT AddOneRow(void);

    //保存修改CSV文件
    INT SaveFile(const UCHAR *pszNewPath=NULL);

    //不保存修改内容，当用户在编辑框修改后，点击取消按钮则返回到上一次保存的状态
    INT NoSaveFile(void);

    //释放资源
    void ReleaseResource(void);

protected:
    //去除空格
    QString Trim(QString str);


private:
    QVector<rowData> m_vecTable;    //数据内容
  //  CFileEx            m_cFileOp;     //文件操作对象
    UCHAR            m_szFilePath[MAX_PATH];  //文件路径
    INT              m_nColumnSize; // 列总数
    INT              m_nRowSize;    // 行总数
    bool             m_bModified;   //修改标志
    INT              m_nAddRowNum;  //添加的行数
   // QFile            m_csvFile;
};

#endif // CCSVFILE_H
