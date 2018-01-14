#ifndef CMODBUS_H
#define CMODBUS_H

#include "project_define.h"
#include <QVector>

class CModBus
{
public:
    //CModBus();

    CModBus(void);
    CModBus(PROTOCOL_ENUM enumProto);
/*
    staitc CModBus *GetInstance(void)
    {
        static CModBus instance;
        return &instance;
    }
*/
    ~CModBus(void);

    //设置通讯协议，ASCII 或 RTU
    bool SetCommProtocol(PROTOCOL_ENUM enumProto);

    //设置通讯地址
    bool SetCommAddress(UCHAR cAddress);

    //解析数据包
    bool ParsePacket(const UCHAR *szData,int nDatatLen, QVector<SModBusResult>& vecResult);

    //封装数据包
    bool PackPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket &sPacket);

    //组装设定地址的查询指令
    bool Pack03Packet( USHORT uAddress, SModBusPacket &sPacket);

private:


    //解析数据包
    bool ParseASCIIPacket(const UCHAR *szData,UINT nDataLen,QVector<SModBusResult> &vecResult );

    //解析数据包
    bool ParseRTUPacket(const UCHAR *szData,UINT nDataLen,QVector<SModBusResult> &vecResult );

    //解析数据数据区域，无论ASCII还是RTU，最后都转为RTU格式，进行数据区解析,nChkIndex为校验位起始位置
    bool ParseHexPacket(const UCHAR *szData,UINT nDataLen,QVector<SModBusResult> &vecResult, UINT &nChkIndex );

    //封装数据包
    bool PackASCIIPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket);
    //封装数据包
    bool PackRTUPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket);
    //封装数据包，无论ASCII还是RTU，进行通讯地址到数据区的组装
    bool PackHexPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket);

    //CRC校验，按协议计算
    USHORT CRCChk(const UCHAR *data, UINT length);
    //LRC校验，按协议计算
    UCHAR  LRCChk(const UCHAR *data, UINT length);

    //十六进制字符转相应数字
    UINT  Char2Num(UCHAR ch);

    //十六进制字符串转char数组，返回转换后长度
    UINT  HexStrToPSZ(const UCHAR *str,UCHAR *&dest );

    //char数组转十六进制字符串，返回是否成功
    bool  PSZToHexStr(const UCHAR *src,UINT len,UCHAR *&dst);

    PROTOCOL_ENUM  m_enumProtocol;   //当前设置的协议格式
    UCHAR           m_cAddress;       //通讯地址
};

#endif // CMODBUS_H
