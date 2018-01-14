#include "cmodbus.h"
#include <QDebug>

#define  PACKET_HEADER_FLG  ':' // ASCII ,数据包头
CModBus::CModBus(void)
{
   m_enumProtocol = PROTO_ASCII;
   m_cAddress     = 1;
}

CModBus::~CModBus(void)
{

}


CModBus::CModBus(PROTOCOL_ENUM enumProto)
{
    m_enumProtocol = enumProto;
}

bool CModBus::SetCommProtocol(PROTOCOL_ENUM enumProto)
{
    m_enumProtocol = enumProto;
    return true;
}

//
bool CModBus::SetCommAddress(UCHAR cAddress)
{
    m_cAddress = cAddress;
    return true;
}

//解析数据包
bool CModBus::ParsePacket(const UCHAR *szData, int nDatatLen, QVector<SModBusResult> &vecResult)
{
    if( (NULL == szData ) || ( nDatatLen <=0 ))
    {
        qDebug() << "Invalid parameter";
        return false;
    }

    bool bRet = false;
    switch(m_enumProtocol)
    {
    case PROTO_ASCII:
        bRet = ParseASCIIPacket(szData,nDatatLen,vecResult);
        break;
    case PROTO_RTU:
        bRet = ParseRTUPacket(szData,nDatatLen,vecResult);
        break;
    default:
        qDebug() << "Protocol type is invalid";
    }

    return bRet;
}

//封装数据包
bool CModBus::PackPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket &vecPacket)
{
  if(vecWrite.size() <= 0)
  {
      qDebug() << "Invalid parameter";
      return false;
  }

  bool bRet = false;
  switch(m_enumProtocol)
  {
  case PROTO_ASCII:
      bRet = PackASCIIPacket(vecWrite,vecPacket);
      break;
  case PROTO_RTU:
      bRet = PackRTUPacket(vecWrite,vecPacket);
      break;
  default:
       qDebug() << "Protocol type is invalid";
  }
  return bRet;
}

//ASCII解析数据包
bool CModBus::ParseASCIIPacket(const UCHAR *szData, UINT nDataLen, QVector<SModBusResult> &vecResult)
{
    if( (NULL == szData ) || ( nDataLen <=0 ))
    {
        qDebug() << "Invalid parameter";
        return false;
    }

    UCHAR *pszData = const_cast<UCHAR *>(szData);
    UCHAR *pszPkt  = NULL;
    UCHAR cCheckSum = 0;
    UINT  uPktLen  = 0;   //
    UINT   nIndex   = 0;   //遍历数据的序号
    INT   nStart   = 0;   //封包的起始位置，即":"后一位
    INT   nStop    = 0;   //封包结束位置，即"\r\n"位置
    UINT   nCheck   = 0;   //校验位位置

    //
    for(nIndex = 0; nIndex < nDataLen; nIndex++ )
    {
        if(pszData[nIndex] == PACKET_HEADER_FLG)
            break;
    }
    //
    nStart = nIndex + 1;

    //
    for(nIndex = nDataLen-1; nIndex >0; nIndex--)
    {
        if(pszData[nIndex-1] == 0x0D &&  pszData[nIndex] == 0x0A)
                    break;
    }
    nStop = nIndex -1;
    if(nStop - nStart < 8)
    {
        qDebug() << "ASCII dataLen is less than 8";
        return false;
    }
    pszData[nStop] = 0;
    pszData[nStop+1] = 0;

    //ASCII转为RTU数据
    uPktLen = HexStrToPSZ(pszData+nStart,pszPkt);
    if(uPktLen <= 0)
        return false;

    //解析数据数据区域，无论ASCII还是RTU，最后都转为RTU格式，进行数据区解析,nChkIndex为校验位起始位置
    if(!ParseHexPacket(pszPkt,uPktLen,vecResult,nCheck))
    {
        vecResult.clear();
        return false;
    }

    if(nCheck >= (INT)uPktLen)
    {
        qDebug()<< "Packet miss checksum value";
        vecResult.clear();
        return false;
    }

    cCheckSum =  LRCChk(pszPkt, nCheck);
    if( pszPkt[nCheck] != cCheckSum)
    {
            //CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"[%s] ASCII pakcet check sum is error.now[%02X] Actual[%02X]  ", __FUNCTION__,pszPkt[nCheck],cCheckSum);
            SAFE_DELETE_ARRAY(pszPkt);
            vecResult.clear();
            return false;
    }
    SAFE_DELETE_ARRAY(pszPkt);
    return true;
}

//解析数据包
bool CModBus::ParseRTUPacket(const UCHAR *szData, uint nDataLen, QVector<SModBusResult>& vecResult)
{
    if(NULL==szData || nDataLen<=0)
    {
       // ERRLOG("Invalid parameter");
        qDebug() << "Invalid parameter";
        return false;
    }

    UINT/*int */            nCheck = 0;
    unsigned short  cCheckSum = 0;   //计算的校验值
    unsigned short  cAutalChk = 0;   //数据包校验值

    if(!ParseHexPacket(szData, nDataLen, vecResult, nCheck))
    {
        vecResult.clear();
        return false;
    }

    if(nCheck > (int)nDataLen)
    {
       // ERRLOG("Packet miss checksum value");
        qDebug() << "Packet miss checksum value";
        vecResult.clear();
        return false;
    }

    cCheckSum =  CRCChk(szData, nCheck);
    cAutalChk =  (szData[nCheck+1]<<8) | szData[nCheck];
    if( cAutalChk != cCheckSum)
    {
       // CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"[%s] RTU pakcet check sum is error.now[%04X] Actual[%04X]  ", __FUNCTION__,cAutalChk,cCheckSum);
        vecResult.clear();
        return false;
    }

    return true;
}
//封装数据包
bool CModBus::PackASCIIPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket)
{
    if(!PackHexPacket(vecWrite, vecPacket))
        return false;


    unsigned char*    pszData = NULL;
    unsigned int      uDataLen = 0;
    unsigned char     cCheckSum = 0;

    if(NULL==vecPacket.content || vecPacket.length<=0)
    {
      //  ERRLOG("Invalid write packet");
        qDebug() << "Invalid write packet";
        return false;
    }
    cCheckSum = LRCChk(vecPacket.content, vecPacket.length);
    vecPacket.content[vecPacket.length] = cCheckSum;
    vecPacket.length++;
    if(!PSZToHexStr(vecPacket.content, vecPacket.length, pszData))
        return false;

    uDataLen = strlen((char*)pszData);

    pszData[uDataLen] = 0x0D;
    pszData[uDataLen+1] = 0x0A;
    uDataLen += 2;

    strcpy_s((char*)vecPacket.content, MAX_BUFFER_DATALEN,(char*)pszData);
    vecPacket.length  = uDataLen;
    vecPacket.waitLen = vecPacket.waitLen*2 + 5;  //:1,LRC2,CRLF2
    SAFE_DELETE_ARRAY(pszData);


    return true;
}
//封装数据包
bool CModBus::PackRTUPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket)
{
    if(!PackHexPacket(vecWrite, vecPacket))
        return false;


    unsigned char*    pszData = NULL;
    unsigned int      uDataLen = 0;
    unsigned short    nCheckSum = 0;

    if(NULL==vecPacket.content || vecPacket.length<=0)
    {
      //  ERRLOG("Invalid write packet");
        qDebug() << "Invalid write packet";
        return false;
    }
    nCheckSum = CRCChk(vecPacket.content, vecPacket.length);

    vecPacket.content[vecPacket.length]   = nCheckSum&0xFF;
    vecPacket.content[vecPacket.length+1] = (nCheckSum>>8)&0xFF;

    vecPacket.length  += 2;
    vecPacket.waitLen += 2;  //CRC 2

    return true;
}

//CRC校验，按协议计算
unsigned short CModBus::CRCChk(const unsigned  char* data, unsigned int length)
{
    //assert(NULL!=data);
    Q_ASSERT(NULL != data);
    int            j;
    unsigned short reg_crc=0xffff;
    while(length--)
    {
        reg_crc^=*data++;
        for(j=0;j<8;j++)
        {
            if(reg_crc & 0x01)
            {
                reg_crc=(reg_crc>>1)^0xa001;
            }
            else
            {
                reg_crc=reg_crc>>1;
            }
        }
    }

    return reg_crc;
}

//LRC校验，按协议计算
unsigned char  CModBus::LRCChk(const unsigned char* data, unsigned int length)
{
   // assert(NULL!=data);
    Q_ASSERT(NULL != data);

    unsigned char cRet= 0;
    while(length--)
    {
        cRet += *data++;
    }

    cRet = (~cRet)+1;
    return cRet;
}
//十六进制字符转相应数字
UINT   CModBus::Char2Num(UCHAR ch)
{
    if(ch>='0'&&ch<='9')  return ch-'0';
    else if(ch>='a'&&ch<='f') return ch-'a'+10;
    else if(ch>='A'&&ch<='F') return ch-'A'+10;

    return -1;
}
//十六进制字符串转char数组，返回转换后长度
unsigned int   CModBus::HexStrToPSZ(const unsigned char* str,unsigned char* &dest )
{
    if(NULL ==str)
    {
       // ERRLOG("Invalid parameter");
        qDebug() << "Invalid parameter";
        return 0;
    }
    size_t len = strlen((const char*)str); //十六进制字符串长度

    if(len == 0 || len%2 !=0)  //无内容 或 长度不是2的整数倍
    {
       // ERRLOG("String length is null or odd number")
        qDebug() << "String length is null or odd number";
        return 0;
    }

    len  = len/2;
    dest = new unsigned char[len+1];
    if(NULL == dest)
    {
      //  ERRLOG("Malloc memory");
        qDebug() << "Malloc memory";
        return 0;
    }
    memset(dest, 0 , len+1);

    int iHigh=0;  //高四位
    int iLow =0;  //低四位
    for( size_t i=0 ; i<len ; i++)
    {
        iHigh = Char2Num(str[2*i]);
        iLow  = Char2Num(str[2*i+1]);
        if(iHigh == -1  || iLow == -1)
        {
           // ERRLOG("String include invalid hex char");
            qDebug() << "String include invalid hex char";
            delete[] dest; dest=NULL;
            return 0;
        }
        dest[i] =(unsigned char)( iHigh*16 + iLow);
    }
    dest[len] = 0;
    return (int)len;
}

//char数组转ANSCII制字符串，返回是否成功
bool   CModBus::PSZToHexStr(const UCHAR *src,UINT len, UCHAR* &dst)
{
    if (NULL == src || len <= 0)
    {
       // ERRLOG("Invalid parameter");
        qDebug()<< "Invalid parameter";
        return false;
    }

    unsigned int index=0;
    char         szTmp[3] = {0};
    unsigned int uTotal = 2*len+10;
    dst = new unsigned char[uTotal];
    if(NULL == dst)
    {
        //ERRLOG("Malloc memory");
        qDebug()<< "Malloc memory";
        return false;
    }
    memset(dst,0,uTotal);
    dst[0] = PACKET_HEADER_FLG;
    for(index=0 ; index<len ; index++)
    {
        sprintf_s(szTmp, 3, "%02X",src[index]);
        strcat_s((char*)dst, uTotal,szTmp);
        memset(szTmp, 0, 3);
    }

    return true;
}

//解析数据数据区域，无论ASCII还是RTU，最后都转为RTU格式，进行数据区解析,nChkIndex为校验位起始位置
bool CModBus::ParseHexPacket(const UCHAR *szData, UINT nDataLen, QVector<SModBusResult> &vecResult, UINT &nChkIndex)
{
    if(NULL==szData || nDataLen<4)  //最少也包含地址1、功能码1、数据1、校验1 共4个字节
    {
       // ERRLOG("Invalid parameter");
        qDebug() << "Invalid parameter";
        return false;
    }
    vecResult.clear();
    SModBusResult    sRecord;   //一条结果

    int    nIndex = 0;        //逐步解析此指令
    int    nTmp   = 0;        //解析过程需要用到的临时变量
    int    nDataIndex =0;     //数据序号


    //第一位为设备通讯地址位
    sRecord.deviceAddress   = szData[nIndex];
    nIndex++;

    //第二位为功能码位置
    sRecord.functionId      = szData[nIndex];
    nIndex++;

    if(sRecord.functionId&0x80) //返回异常码
    {
        sRecord.address = 0;
        sRecord.value   = szData[nIndex];
        nIndex++;
      //  CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"Receive packet function code[%02X] exception code[%02X]",sRecord.functionId,sRecord.value);
        vecResult.push_back(sRecord);
        nChkIndex = nIndex;
        return true;
    }
    //紧接为数据区，根据功能码进行解析
    switch(sRecord.functionId)
    {
    case 0x03:  //读取缓存器内容
        nTmp  =  szData[nIndex];  //数据字节数
        nIndex++;
        if( nTmp == 0 || (nIndex+nTmp > nDataLen) || nTmp%2 !=0) //数据丢失，不是双字节数
        {
           // ERRLOG("Receive packet data imperfect");
            qDebug() << "Receive packet data imperfect";
            return false;
        }
        //读取多个数据地址结果
        for(nDataIndex=0; nDataIndex<nTmp; nDataIndex+=2)
        {
            sRecord.address = nDataIndex;  //当功能码为03时，address代表数据序号，与发送查询对比即可知道相应地址
            sRecord.value   = (szData[nIndex]<<8)|szData[nIndex+1];
            vecResult.push_back(sRecord);
            nIndex+=2;
        }
        break;
    case 0x06:  //写入一笔数据到缓存器
        if(nDataLen < 8) //ASCII为9byte RTU为8 byte
        {
            //ERRLOG("Receive packet data len less than 8 under 03 function code.");
            return false;
        }
        sRecord.address = (szData[nIndex]<<8)|szData[nIndex+1] ; //写入的地址
        sRecord.value   = (szData[nIndex+2]<<8) | szData[nIndex+3]; //返回的写入的值
        nIndex += 4;
        vecResult.push_back(sRecord);
        break;
    case 0x10: //写入多笔到缓存器
        if(nDataLen < 8) //ASCII为9byte RTU为8 byte
        {
            //ERRLOG("Receive packet data len less than 8 under 10 function code.");
            return false;
        }
        sRecord.address = (szData[nIndex]<<8)|szData[nIndex+1] ;    //写入的起始地址
        sRecord.value   = (szData[nIndex+2]<<8) | szData[nIndex+3]; //返回的资料量，每笔资料单位为Word
        vecResult.push_back(sRecord);
        nIndex += 4;
        break;
    default: //其他功能码
        //CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"[%s] Funciton code [0x%02X] is not support ", __FUNCTION__,sRecord.functionId);
        return false;
    }

    //所有数据区数据解析完后，紧接即为校验位
    nChkIndex = nIndex;

    return true;
}

//封装数据包，无论ASCII还是RTU，进行通讯地址到数据区的组装
bool CModBus::PackHexPacket(QVector<SModBusWrite>& vecWrite, SModBusPacket& vecPacket)
{
    UINT     uSize = 0  ;  //待封装数据 vector的大小
    UINT     uIndex= 0  ;  //遍历Vector 索引
    UINT     uPktLen = 0;  //封包后数据长度，

    uSize = vecWrite.size();
    if(uSize < 1)
    {
       // WARNLOG("Write vector is empty");
        qDebug() << "Write vector is empty";
        return true;
    }

    switch(vecWrite[0].functionId)
    {
    case 0x03:  //读取缓存器内容
        if(uSize > 1)
        {
            //WARNLOG("FunctionID=3 records > 1.one record in general.Will read first record and dircard other.");
            qDebug() << "FunctionID=3 records > 1.one record in general.Will read first record and dircard other.";
        }
        uPktLen  =  6;   //通讯地址1、功能码1、数据地址2、数据长度2
        vecPacket.length  = uPktLen;
        vecPacket.waitLen = 3+2*vecWrite[0].value;  //返回数据包固定长度3，每地址长度2
        vecPacket.content[0]   =  vecWrite[0].deviceAddress;   //通讯地址
        vecPacket.content[1]   =  0x03;                        //功能码
        vecPacket.content[2]   =  (vecWrite[0].address>>8) & 0xFF;   //地址高位
        vecPacket.content[3]   =  (vecWrite[0].address)    & 0xFF;   //地址低位
        vecPacket.content[4]   =  (vecWrite[0].value>>8)   & 0xFF;   //读取字数高位
        vecPacket.content[5]   =  (vecWrite[0].value)      & 0xFF;   //读取字数低位

        break;
    case 0x06:  //写入一笔数据到缓存器
        if(uSize != 1)
        {
            //WARNLOG("FunctionID=6 records > 1.one record in general.Will read first record and dircard other.");
             qDebug() << "FunctionID=6 records > 1.one record in general.Will read first record and dircard other.";
        }
        uPktLen  =  6;   //通讯地址1、功能码1、数据地址2、数据内容2
        vecPacket.length  = uPktLen;
        vecPacket.waitLen = 6;
        vecPacket.content[0]   =  vecWrite[0].deviceAddress;   //通讯地址
        vecPacket.content[1]   =  0x06;                        //功能码
        vecPacket.content[2]   =  (vecWrite[0].address>>8) & 0xFF;   //地址高位
        vecPacket.content[3]   =  (vecWrite[0].address)    & 0xFF;   //地址低位
        vecPacket.content[4]   =  (vecWrite[0].value>>8)   & 0xFF;   //读取字数高位
        vecPacket.content[5]   =  (vecWrite[0].value)      & 0xFF;   //读取字数低位

        break;
    case 0x10: //写入多笔到缓存器
        uPktLen = uSize*2 + 7;   //每笔资料2*n，通讯地址1、功能码1、数据起始地址2、数据字数2，数据字节数1
        vecPacket.length  = uPktLen;
        vecPacket.waitLen = 6;
        vecPacket.content[0]   =  vecWrite[0].deviceAddress;   //通讯地址
        vecPacket.content[1]   =  0x10;                        //功能码
        vecPacket.content[2]   =  (vecWrite[0].address>>8) & 0xFF;   //地址高位
        vecPacket.content[3]   =  (vecWrite[0].address)    & 0xFF;   //地址低位
        vecPacket.content[4]   =  (uSize>>8)               & 0xFF;   //Vector 数目代表待写入地址数高位
        vecPacket.content[5]   =  (uSize)                  & 0xFF;   //Vector 数目代表待写入地址数低位
        vecPacket.content[6]   =  (unsigned char)(uSize*2)       ;   //数据字节数为字数*2

        for(uIndex=0 ; uIndex<uSize ; uIndex++)
        {
            vecPacket.content[7+ (2*uIndex)  ]   =  (vecWrite[uIndex].value>>8)   & 0xFF;   //数据高位
            vecPacket.content[7+ (2*uIndex+1)]   =  (vecWrite[uIndex].value)      & 0xFF;   //数据低位
        }

        break;
    default: //其他功能码
       // CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"[%s] Funciton code [0x%02X] is not support ", __FUNCTION__,vecWrite[0].functionId);
        return false;
    }

    return true;
}

//组装设定地址的查询指令
bool CModBus::Pack03Packet(unsigned short uAddress, SModBusPacket &sPacket)
{
    QVector<SModBusWrite>   vecWrite;
    SModBusWrite           sWrite;

    vecWrite.clear();
    sWrite.deviceAddress  = m_cAddress;
    sWrite.functionId     = 0x03;
    sWrite.address        = uAddress;
    sWrite.value          = 1;
    vecWrite.push_back(sWrite);

    if(!PackPacket(vecWrite, sPacket))
    {
        return false;
    }

    return true;
}
