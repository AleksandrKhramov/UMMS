//---------------------------------------------------------------------------
#ifndef IteratorByPatternsClassH
#define IteratorByPatternsClassH
//---------------------------------------------------------------------------
//							  Built headers

//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
class TIteratorByPatterns
{
	struct TConnectionWithPatternNumber
	{
    	TComConnection *ComConnection;
        int PatternNumber;

        TConnectionWithPatternNumber(TComConnection *_ComConnection, int _PatternNumber)
        {
        	ComConnection = _ComConnection;
            PatternNumber = _PatternNumber;
        }
    };

private:
    std::vector< std::vector<byte> > Patterns;
    std::vector<TConnectionWithPatternNumber *> ConnectionsWithPatternNumbers;

    int IndexOfComConnection(TComConnection *);
public:
	void AddConnectionOnIterating(TComConnection *);
    void AddPattern(std::vector<byte> Pattern);
    void ClearPatterns();
    void ClearConnections();
    bool IsConnectionOnIterating(TComConnection *);
    bool NextPatternForConnection(TComConnection *);
    bool RemoveConnection(TComConnection *);


    TIteratorByPatterns();
    ~TIteratorByPatterns();
};
//---------------------------------------------------------------------------
#endif
