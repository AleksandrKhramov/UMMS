//---------------------------------------------------------------------------
#include "IteratorByPatternsClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//Конструктор
TIteratorByPatterns::TIteratorByPatterns()
{

}
//---------------------------------------------------------------------------
//Деструктор
TIteratorByPatterns::~TIteratorByPatterns()
{

}
//---------------------------------------------------------------------------
void TIteratorByPatterns::AddConnectionOnIterating(TComConnection *ComConnection)
{
	if(ComConnection != NULL)
    {

		ComConnections.push_back(ComConnection);
        ConnectionsPatternNumbers.
        if(Patterns.size() != 0)
       	 	ComConnection->SendData(Patterns[0]);
    }
}
//---------------------------------------------------------------------------
void TIteratorByPatterns::AddPattern(std::vector<byte> Pattern)
{
	Patterns.push_back(Pattern);

}
//---------------------------------------------------------------------------
bool TIteratorByPatterns::IsConnectionOnIterating(TComConnection *ComConnection)
{

}
//---------------------------------------------------------------------------
bool TIteratorByPatterns::NextPatternForConnection(TComConnection *ComConnection)
{
	/* if(There is not patterns for ComConnection) */
		return false;
}
//---------------------------------------------------------------------------
bool TIteratorByPatterns::RemoveConnection(TComConnection *ComConnection)
{
    for (int i = 0; i < ComConnections.size(); ++i)
    {
     	if(ComConnections[i] == ComConnection)
        {
        	ComConnections.erase(ComConnections.begin() + i);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void TIteratorByPatterns::ClearPatterns()
{

}
 //---------------------------------------------------------------------------
