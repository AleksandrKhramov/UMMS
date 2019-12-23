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
        bool Reiterating;

        TConnectionWithPatternNumber(TComConnection *_ComConnection, int _PatternNumber, bool _Reiterating)
        {
        	ComConnection = _ComConnection;
            PatternNumber = _PatternNumber;
            Reiterating = _Reiterating;
        }
    };

private:
    std::vector< std::vector<byte> > Patterns;
    std::vector<TConnectionWithPatternNumber *> ConnectionsWithPatternNumbers;

    int IndexOfComConnection(TComConnection *);
public:
	void AddConnectionOnIterating(TComConnection *, bool Reiterating = false);
    void AddPattern(std::vector<byte> Pattern);
    void ClearPatterns();
    void ClearConnections();
    bool IsConnectionOnIterating(TComConnection *);
    bool NextPatternForConnection(TComConnection *);
    bool RemoveConnection(TComConnection *);
    bool IsReiteratedConnection(TComConnection *);



    TIteratorByPatterns();
    ~TIteratorByPatterns();
};
//---------------------------------------------------------------------------
#endif
