//---------------------------------------------------------------------------
#ifndef ComConnectionClassH
#define ComConnectionClassH
//---------------------------------------------------------------------------
//							  Built headers
#include <vector>
#include <System.hpp>
#include <System.SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ComConnectionErrors.cpp"
//---------------------------------------------------------------------------
class TComConnection
{
private:
public:

    String ComName;
    int ComNumber;
    int ExpectationDelay;

    HANDLE COMport;				//���������� �����
    //static HANDLE reader;				//���������� ������ ������ �� �����
	//static HANDLE writer;				//���������� ������ ������ � ����

    void COMOpen(int _ComNumber, int _BaudRate = 9600);
    void COMClose(void);

    //static DWORD WINAPI ReadThread(LPVOID);
	//static DWORD WINAPI WriteThread(LPVOID);

    void (__closure *DataReadyTrigger)(TComConnection *, std::vector<byte>);
    void (__closure *ConnectionErrorTrigger)(TComConnection *, int ErrorNumber);

    OVERLAPPED overlapped;		//����� ������������ ��� �������� ������ (��. ����� ReadThread)
	OVERLAPPED overlappedwr;       	//����� ������������ ��� �������� ������ (��. ����� WriteThread)
    void SendData(std::vector<byte> Data);
    void Write(std::vector<byte> Data);
    void WaitAnswer();

    TComConnection( String ComName,
                    int ComNumber,
                    int ExpectationDelay,
    				void (__closure *_DataReadyTrigger)(TComConnection *, std::vector<byte>),
                    void (__closure *_ConnectionErrorTrigger)(TComConnection *, int ErrorNumber),
     				int BaudRate = 9600);

    ~TComConnection();                                                            	//����������
};
//---------------------------------------------------------------------------
#endif
