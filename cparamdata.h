#ifndef CPARAMDATA_H
#define CPARAMDATA_H

#include "project_define.h"
#include "ccsvfile.h"

#include <QString>
#include <QStringList>
#include <QVector>

#define COLUMN_PRNO              0            //编号序号
#define COLUMN_MODBUS_ADDRSS     1            //MODBUS地址
#define COLUMN_DESCRIPTION       2            //描述
#define COLUMN_UNIT              3            //单位
#define COLUMN_DATA              4            //数据
#define COLUMN_DEFAULT           5            //默认值
#define COLUMN_MIN               6            //最小值
#define COLUMN_MAX               7            //最大值
#define COLUMN_ATTRIBUTE         8            //属性列表

#define MIN_COLUMN_SIZE          9            //最小列数
#define PARAMETER_TYPE_FLAG      "*****"      //如果参数编号为此，则表示一种新的类型的开始

#define ATTR_READ                    0x00000001     //只读
#define ATTR_WRUN                    0x00000002     //在电机运行过程中，参数不可修改
#define ATTR_DPB                     0x00000004     //二进制显示
#define ATTR_DPH                     0x00000008     //十六进制显示
#define ATTR_SIGN                    0x00000010     //有符号数
#define ATTR_DOT0                    0x00000020     //没有小数点
#define ATTR_DOT1                    0x00000040     //1个小数点
#define ATTR_DOT2                    0x00000080     //2个小数点
#define ATTR_DOT3                    0x00000100     //3个小数点

#define STATUS_INIT                  0x00000001     //初始化完成
#define STATUS_EXCEPTION             0x00000002     //是不是值有异常

//0-255 为MODBUS协议 使用异常
#define EXCEPTION_WRERROR            256            //读写指令错误或者超时
#define EXCEPTION_OUTOFRANGE         257            //超出范围异常
//
typedef struct _SParamData
{
  QString  prNo;          // 参数编号
  QString  description;   //
  QString  unit;          //
  USHORT   address;       //ModBus 地址
  USHORT   data;          //
  USHORT   defaultData;   //默认数据
  USHORT   min;           //
  USHORT   max;           //
  USHORT   attribute;     // 属性

  UINT     dataId;       //数据编号
  INT      typeId;       //数据类型编号
  UINT     status;       //行数据状态展示，0：初始化，1：初始化结束
}SParamData;

class CParamData
{
public:
    CParamData();

    //初始化参数列表文件
    bool OpenParamListFile(const QString strFilePath);

    //查找对应类型数据，从开始位置(包含)查找此类型
    SParamData FindDataByType(int nType,UINT nStartPos=0);

    //获取对应编号数据，如果数据为空，则prNo为空
    SParamData GetDataByIndex(UINT nIndex);


    //设置对应编号数据
    bool       SetDataByIndex(UINT nIndex, QString strValue);

    //设置对应地址数据
    bool       SetDataByAddress(unsigned short uAddress, unsigned short uData, int &nDataIndex);
    //设置对应地址异常
    bool       SetExceptionByAddress(unsigned short uAddress, unsigned short uExp, int &nDataIndex);

    //获取数据行数
    int        GetDataCount(void);

    //获取表标题头
    const QStringList&  GetDataHeader(void);

    //获取类型说明数据
    const QStringList&  GetDataTypeDes(void);

    //通过属性描述，将字符串值转为保存值，比如“655.35”+属性“DOT2”，获取65535
    USHORT  GetValueByAttr(QString strVal, DWORD uAttr);
    //通过属性描述，将字符串值转为保存值，如果超过2字节最大值，则返回FALSE
    bool    GetValueByAttr(QString strVal, DWORD uAttr, unsigned short &uData);

    //通过属性描述，将数值值转为字符串，比如“65535”+属性“DOT2”，获取"655.35"字符串
    QString  FormatDataValue(unsigned short uVal, DWORD uAttr);

    //获取地址显示格式
    QString  FormatAddress(unsigned short uAddress);

    //获取属性显示格式
    QString  FormatAttribute(UINT uAttr);

    //在字符中查找对应属性标志符，如果找到返回属性值，否则返回0
    DWORD FindAttr(QString& strTotal, QString strAttr, DWORD uAttr);

    //通过属性字符说明，获得DWORD 属性值
    DWORD GetAttr(QString strVal);

    //将字符串strVal中小数点，向后（nStep为正）或向前（nStep为负）移动nStep位
    void  MoveStrDot(QString &strVal, int nStep);
private:
    QVector<SParamData>  m_vecData; //存放数据对象
    QStringList          m_strListDataType;
    QStringList          m_strListHeader;  //表头
    CCSVFile             m_fpCSV;

};

#endif // CPARAMDATA_H
