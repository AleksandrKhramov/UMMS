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
        TConnectionWithPatternNumber *ConnectionWithPatternNumber = new TConnectionWithPatternNumber(ComConnection, 0);
        ConnectionsWithPatternNumbers.push_back(ConnectionWithPatternNumber);

        if(Patterns.size() != 0)
       	 	ComConnection->SendData(Patterns[ConnectionWithPatternNumber->PatternNumber]);
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
    if(IndexOfComConnection(ComConnection) >= 0)
    {
    	return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TIteratorByPatterns::NextPatternForConnection(TComConnection *ComConnection)
{
	int ConectionIndex = IndexOfComConnection(ComConnection);

    if(ConectionIndex >= 0)
    {
        if(ConnectionsWithPatternNumbers[ConectionIndex]->PatternNumber < (Patterns.size() - 1))
        {
        	ConnectionsWithPatternNumbers[ConectionIndex]->PatternNumber += 1;
            ConnectionsWithPatternNumbers[ConectionIndex]->ComConnection->SendData(Patterns[ConnectionsWithPatternNumbers[ConectionIndex]->PatternNumber]);
        	return true;
        }
        else
        	return false;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TIteratorByPatterns::RemoveConnection(TComConnection *ComConnection)
{
	int ConectionIndex = IndexOfComConnection(ComConnection);
    if(ConectionIndex >= 0)
    {
        ConnectionsWithPatternNumbers.erase(ConnectionsWithPatternNumbers.begin() + ConectionIndex);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void TIteratorByPatterns::ClearPatterns()
{
    Patterns.clear();
}
 //---------------------------------------------------------------------------
int TIteratorByPatterns::IndexOfComConnection(TComConnection *ComConnection)
{
    for (int i = 0; i < ConnectionsWithPatternNumbers.size(); ++i)
    {
    	if(ConnectionsWithPatternNumbers[i]->ComConnection == ComConnection)
        {
        	return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
void TIteratorByPatterns::ClearConnections()
{
	ConnectionsWithPatternNumbers.clear();

}
//---------------------------------------------------------------------------

