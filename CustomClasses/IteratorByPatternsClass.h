//---------------------------------------------------------------------------
#ifndef IteratorByPatternsClassH
#define IteratorByPatternsClassH
//---------------------------------------------------------------------------
//							  Built headers
#include <map>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// 								My headers
#include "ComConnectionClass.h"
//---------------------------------------------------------------------------
class TIteratorByPatterns
{
private:
    std::vector< std::vector<byte> > Patterns;
    std::vector<TComConnection *> ComConnections;
    std::map<TComConnection *, int> ConnectionsPatternNumbers;
public:
	void AddConnectionOnIterating(TComConnection *);
    void AddPattern(std::vector<byte> Pattern);
    void ClearPatterns();
    bool IsConnectionOnIterating(TComConnection *);
    bool NextPatternForConnection(TComConnection *);
    bool RemoveConnection(TComConnection *);

    TIteratorByPatterns();
    ~TIteratorByPatterns();
};
//---------------------------------------------------------------------------
#endif
