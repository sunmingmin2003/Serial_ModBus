#ifndef PROJECT_DEFINE_H
#define PROJECT_DEFINE_H


#include <cmath>

typedef unsigned int  UINT;
typedef int           INT;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long  DWORD;
typedef bool           BOOL;
#define TRUE           true;
#define FALSE          false;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define MAX_BUFFER_DATALEN 256

//内存安全释放
#define  SAFE_DELETE_ARRAY(p) if(NULL!=p) { delete[] p; p = NULL; }
#ifdef   SAFE_DELETE
#undef   SAFE_DELETE
#endif
#define  SAFE_DELETE(p)       if(NULL!=p) { delete   p; p = NULL; }
#define  SAFE_FREE(p)         if(NULL!=p) { free(p)   ; p = NULL; }

enum PROTOCOL_ENUM
{
    PROTO_ASCII = 0,
    PROTO_RTU
};

//串口属性
typedef struct _SSerialSet
{
    UINT  portNo;   //端口号
    UINT  baud;     //波特率
    char  parity;   //奇偶校验
    UINT  databits;  //数据位
    UINT  stopbits;  //停止位
}SSerialSet;

//Modbus数据格式
typedef struct _SModBus
{
    unsigned short  address;      //数据地址
    unsigned short  value;        //值
    UCHAR      deviceAddress;     //设备通讯地址
    UCHAR      functionId;        //功能码
   // _SModBus(){address=0; value=0; deviceAddress=0; functionId=0;}
} SModBusResult, SModBusWrite;


//下发设备的封包
typedef struct _SModBusPacket
{
    UCHAR content[MAX_BUFFER_DATALEN];
    UINT  length ;  //数据包长度
    UINT  waitLen;  //期待返回包长度
  /*  _SModBusPacket(){memset(content,0,MAX_BUFFER_DATALEN);length=0;waitLen=0;}

    _SModBusPacket(const _SModBusPacket &other)
    { length = other.length;
      waitLen = other.waitLen;
      memcpy(content,other.content,MAX_BUFFER_DATALEN);
    }
 */
}SModBusPacket;


#endif // PROJECT_DEFINE_H
