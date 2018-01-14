#include "cparamdata.h"




CParamData::CParamData()
{
  m_vecData.clear();
  m_strListHeader << "Pr.No" << "Adress" << "Description" << "Unit" << "Data"
                  << "Default" << "Min" << "Max" << "Attribute";
}

/*
CParamData::~CParamData()
{

}
*/
bool CParamData::OpenParamListFile(const QString strFilePath)
{
   if(m_fpCSV.OpenCSVFile(strFilePath) !=0)
   {
       qDebug()<< "Open file error";
       return false;
   }

   m_vecData.clear();
   m_strListDataType.clear()/*removeAll()*/;
   m_strListHeader.clear()/*removeAll())*/;

   SParamData   sData;
   INT          nSize = 0;
   INT          nIndex= 0;
   UINT         uTypeId=0;
   UINT         uIndex =1;   //数据编号从1开始
   bool         ok;
   QString      strVal("");

   QString strTmp;
   UINT lengthTmp;

   if(m_fpCSV.GetColumnSize()< MIN_COLUMN_SIZE)
       {
          // CLogger::getInstance()->Write(LOG_ERROR,WM_LOGMESSAGE_TO_WINDOWS,"CSV conf file column [%s] < min column[%d]",m_fpCSV.GetColumnSize(), MIN_COLUMN_SIZE);
          qDebug()<< "CSV conf file column [%s] < min column[%d]";
          return false;
       }
       nSize = m_fpCSV.GetRowSize();


       //读取CSV文件标题头
    for(nIndex=0; nIndex<MIN_COLUMN_SIZE ; nIndex++)
       {
           strVal = m_fpCSV.GetCellData(nIndex, 0);
           m_strListHeader.append(strVal);
       }

    //读取CSV文件内容,数据从第二行开始，即序号1开始
      for(nIndex=1; nIndex<nSize ; nIndex++)
        {
            strVal = m_fpCSV.GetCellData(COLUMN_PRNO, nIndex);  //读取参数编号
            if(strVal.compare(PARAMETER_TYPE_FLAG) == 0)        //如果是新的类型的开始
            {
                uTypeId++;                                              //类型编号递增
                sData.prNo        =  strVal;                                                       //参数编号
                sData.description =  m_fpCSV.GetCellData(COLUMN_DESCRIPTION,   nIndex);    //描述说明
                sData.typeId      =  uTypeId;                                                      //类型编号
                sData.dataId      =  uIndex++;                                                     //数据编号
                //其他字段判断当 prNo为PARAMETER_TYPE_FLAG时，全部在UI中显示为PARAMETER_TYPE_FLAG
                m_vecData.push_back(sData);                                                        //数据保存到容器中
                m_strListDataType.append(sData.description);                                           //将描述加到类型中
            }
            else  //此行数据为数据行
            {
                sData.prNo        =  strVal;                                                       //参数编号
                strTmp= m_fpCSV.GetCellData(COLUMN_MODBUS_ADDRSS, nIndex);
                lengthTmp  = strTmp.length();
                strTmp = strTmp.left(lengthTmp-1);
                sData.address =strTmp.toInt(&ok,16);// (m_fpCSV.GetCellData(COLUMN_MODBUS_ADDRSS, nIndex)).toInt();/*toInt(&ok,16)*/;//((unsigned short)strtol(m_fpCSV.GetCellData(COLUMN_MODBUS_ADDRSS, nIndex).c_str(), NULL, 16);    //MODBUS 地址
                sData.description =  m_fpCSV.GetCellData(COLUMN_DESCRIPTION,   nIndex)/*.c_str()*/;  //描述说明
                sData.unit        =  m_fpCSV.GetCellData(COLUMN_UNIT,          nIndex)/*.c_str()*/;    //单位
                sData.attribute   =  GetAttr(m_fpCSV.GetCellData(COLUMN_ATTRIBUTE,nIndex)/*.c_str()*/);  //属性值
                sData.defaultData =  GetValueByAttr(m_fpCSV.GetCellData(COLUMN_DEFAULT,nIndex)/*.c_str()*/,sData.attribute);  //默认值
                sData.data        =  sData.defaultData;                                            //初始化时，未读值，全部默认为默认值
                sData.min         =  GetValueByAttr(m_fpCSV.GetCellData(COLUMN_MIN,nIndex)/*.c_str()*/,sData.attribute);     //最小值
                sData.max         =  GetValueByAttr(m_fpCSV.GetCellData(COLUMN_MAX,nIndex)/*.c_str()*/,sData.attribute);     //最大值
                sData.typeId      =  uTypeId;                                                      //类型编号
                sData.dataId      =  uIndex++;                                                     //数据编号
                m_vecData.push_back(sData);
            }
        }
}

//在字符中查找对应属性标志符，如果找到返回属性值，否则返回0
DWORD      CParamData::FindAttr(QString& strTotal, QString strAttr, DWORD uAttr)
{
    if(strTotal.isEmpty())
        return 0;
    int nPos =  strTotal.indexOf(strAttr);
    if(nPos >= 0) // 有此内容
    {
        if( (nPos-1 >=0) &&(strTotal[nPos-1] == '|') ) //说明前方还有内容，并且是分隔符'|'
        {
            strTotal.remove/*delete*/(nPos-1, strAttr.length()/*GetLength()*/+1);

        }
        else
        {
            strTotal.remove/*Delete*/(nPos, strAttr.length()/*GetLength()*/);

        }

        return uAttr;
    }

    return 0;
}

//通过属性字符说明，获得DWORD 属性值
DWORD      CParamData::GetAttr(QString strVal)
{
    DWORD uRet = 0;

    uRet |=  FindAttr(strVal, "READ",  ATTR_READ);
    uRet |=  FindAttr(strVal, "WRUN",  ATTR_WRUN);
    uRet |=  FindAttr(strVal, "DPB",   ATTR_DPB);
    uRet |=  FindAttr(strVal, "DPH",   ATTR_DPH);
    uRet |=  FindAttr(strVal, "SIGN",  ATTR_SIGN);
    uRet |=  FindAttr(strVal, "DOT0",  ATTR_DOT0);
    uRet |=  FindAttr(strVal, "DOT1",  ATTR_DOT1);
    uRet |=  FindAttr(strVal, "DOT2",  ATTR_DOT2);
    uRet |=  FindAttr(strVal, "DOT3",  ATTR_DOT3);

    return uRet;
}




//通过属性描述，将字符串值转为保存值，比如“655.35”+属性“DOT2”，获取65535
USHORT CParamData::GetValueByAttr(QString strVal, DWORD uAttr)
{
    unsigned short uRet = 0;
    int            nRadix = 10;
    bool           ok; //mmsun

    //获取显示方式，默认为10进制
    if(uAttr & ATTR_DPB)
    {
        nRadix = 2;
    }
    else if(uAttr & ATTR_DPH)
    {
        nRadix = 16;
    }
    else
    {
        nRadix = 10;
    }

    //判断小数位
    if(uAttr & ATTR_DOT1)
    {
        MoveStrDot(strVal,1);
    }
    else if(uAttr & ATTR_DOT2)
    {
        MoveStrDot(strVal,2);
    }
    else if(uAttr & ATTR_DOT3)
    {
        MoveStrDot(strVal,3);
    }
    else
        ;

    uRet = strVal.toInt(&ok,nRadix)/*(unsigned short)strtol(strVal.GetBuffer(), NULL, nRadix)*/;

    //strtol(
    return uRet;
}

//通过属性描述，将字符串值转为保存值，如果超过2字节最大值，则返回FALSE
bool   CParamData::GetValueByAttr(QString strVal, DWORD uAttr, unsigned short &uData)
{
    int      nRadix = 10;
    long     nValue = 0;
    bool     ok; //mmsun
    //获取显示方式，默认为10进制
    if(uAttr & ATTR_DPB)
    {
        nRadix = 2;
    }
    else if(uAttr & ATTR_DPH)
    {
        nRadix = 16;
    }
    else
    {
        nRadix = 10;
    }

    //判断小数位
    if(uAttr & ATTR_DOT1)
    {
        MoveStrDot(strVal,1);
    }
    else if(uAttr & ATTR_DOT2)
    {
        MoveStrDot(strVal,2);
    }
    else if(uAttr & ATTR_DOT3)
    {
        MoveStrDot(strVal,3);
    }
    else
        ;

    //判断是否超过short 范围
    nValue = strVal.toInt(&ok,nRadix)/*strtol(strVal.GetBuffer(), NULL, nRadix)*/;

    //有符号数判断
    if(uAttr & ATTR_SIGN)
    {
        if(nValue< -32768 || nValue > 32767)
            return false;
    }
    else//无符号判断
    {
        if(nValue< 0 || nValue > 65535)
            return false;
    }

    uData = (unsigned short)nValue;

    return true;
}

//将字符串strVal中小数点，向后（nStep为正）或向前（nStep为负）移动nStep位
void  CParamData::MoveStrDot(QString &strVal, int nStep)
{
    QString     strTmp("");
    int nDotPos = strVal.indexOf/*Find*/(".");    //小数点位置，多个则查找第一个
    int nLen    = strVal.length()/*GetLength()*/;  //字符串总长
    int nDotNewPos  = 0;                   //新小数点位置
    int nIndex      = 0;
    int nDotBackLen = 0;
    bool   bNegative = false;             //非负数

    strVal  = strVal.simplified()/*Trim()*/;
    if(strVal[0] == '-')   //是否是负数
    {
        bNegative = TRUE;
        strVal.remove/*Delete*/(0,1);  //删除负号
    }

    if(nDotPos == -1)   //如果没有小数点，则直接补一位小数点
    {
        strVal.append/*Append*/(".");
    }
    nLen    = strVal.length/*GetLength*/();
    nDotPos = strVal.indexOf/*Find*/(".");          //如果没有则默认小数点在最后的位置

    nDotNewPos  = nDotPos + nStep;
    nDotBackLen = nLen-nDotPos-1;          //小数点后的数据长度，当向后移动位数不足时需填充0

    if(nStep == 0 || strVal.simplified().isEmpty()/*Trim().IsEmpty()*/ ) //不做任何移动
        return ;
    if(nDotNewPos > 0) // 如果新小数点前面还有数字，如X.xxxx
    {
        if(nStep > 0) //向后移动
        {
            while(nDotBackLen++ < nStep ) //如果位数不够自动补0
            {
                strVal.append/*Append*/("0");
            }
            strTmp.append/*Append*/(strVal.left/*Left*/(nDotPos));
            strTmp.append/*Append*/(strVal.mid/*Mid*/(nDotPos+1, nStep));
            strTmp.append/*Append*/(".");
            strTmp.append/*Append*/(strVal.right/*Right*/(nLen-nDotNewPos-1));
        }
        else //向前移动
        {
            strTmp.append/*Append*/(strVal.left/*Left*/(nDotNewPos));
            strTmp.append/*Append*/(".");
            strTmp.append/*Append*/(strVal.mid/*Mid*/(nDotNewPos, nDotPos-nDotNewPos));
            strTmp.append/*Append*/(strVal.right/*Right*/(nLen-nDotPos));
        }
    }
    else// 小于1的数，如0.xx
    {
        strTmp.append/*Append*/("0.");
        nIndex = nDotNewPos;
        while(nIndex<0)
        {
            strTmp.append("0");
            nIndex++;
        }
        strTmp.append(strVal.left/*Left*/(nDotPos));
        strTmp.append(strVal.left/*Right*/(nLen-nDotPos-1));
    }

    strVal = strTmp;

    //如果最后位为"."，则删除
    nLen   = strVal.length/*GetLength*/();
    if(strVal[nLen-1] == '.')
        strVal.remove/*Delete*/(nLen-1);

    //如果是负数
    if(bNegative)   //是否是负数
    {
        strVal = (QString)"-" + strVal;  //删除负号
    }
}

QString  CCSVFile::Trim(QString str)
{
    QString strVal = str;
    /*std::string::size_type*/ QString::size_type spos = 0;
    /*std::string::size_type*/ QString::size_type epos = strVal.length() - 1;

    if(strVal.isEmpty())
        return strVal;
    while ((spos <= epos) && (/*isspace((unsigned char)strVal[spos])*/ strVal.at(spos).isSpace()))
    {
        ++spos;
    }

    if (spos <= epos)
    {
        while ((epos >= spos) && (/*isspace((unsigned char)strVal[epos])*/strVal.at(epos).isSpace()))
        {
            --epos;
        }
    }

    if (spos <= epos)
    {
        strVal = strVal.mid/*substr*/(spos, epos - spos + 1);
    }
    else
    {
        strVal = "";
    }

    return strVal;
}

SParamData CParamData::GetDataByIndex(UINT nIndex)
{
    SParamData sData;
    if(nIndex >= m_vecData.size())
    {
        /*ERRLOG("Out of vector range");*/
        return sData;
    }

    sData = m_vecData[nIndex];
    return sData;
}

QString  CParamData::FormatDataValue(unsigned short uVal, DWORD uAttr)
{
  QString strRet("");
  int     nRadix = 10;
  char    szNumber[33]= {0};
  short   nValue = (short)uVal;

  //获取显示方式，默认为10进制
  if(uAttr & ATTR_DPB)
  {

  }
  else if(uAttr & ATTR_DPH)
  {

  }
  else
  {
      if(uAttr & ATTR_SIGN)
      {

      }
      else
      {

      }

      //判断小数位
      if(uAttr & ATTR_DOT1)
        {
           MoveStrDot(strRet,-1);
        }
      else if(uAttr & ATTR_DOT2)
        {
           MoveStrDot(strRet,-2);
        }
      else if(uAttr & ATTR_DOT3)
        {
            MoveStrDot(strRet,-3);
        }
  }
 return strRet;
}
