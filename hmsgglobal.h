#ifndef HMSGGLOBAL_H
#define HMSGGLOBAL_H

#define M_ERROR 00
#define M_RECV  01
#define M_SEND  02

const int recvMax = 500;

typedef struct _tagSndData
{
    char* data;
    int len;
}SndData;


typedef struct _tagRecvData
{
    char* data;
    int len;
    int type;
}RecvData;


typedef struct _tagShowData
{
    char* data;
    int len;
    int type;
}ShowData;

#endif
