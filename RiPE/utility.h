#ifndef __MSEA_OBJECTS_UTILITY_H
#define __MSEA_OBJECTS_UTILITY_H

#define NEAR_FAST __fastcall

#pragma pack(1)
template <class T> struct TSecData
{
    T data;
    unsigned char bKey;
    unsigned char FakePtr1;
    unsigned char FakePtr2;
    unsigned char Trash1;
    unsigned short wChecksum;
    unsigned short Unknown1;
};

class UINT128
{
public:
    unsigned long m_data[4];
};

template <class T> class TSingleton
{
public:
    static T *ms_pInstance;
};

template <class T> class TSecType
{
public:
    unsigned int FakePtr1;
    unsigned int FakePtr2;
    TSecData <T>* m_secdata;
};

template <class T> class ZArray
{
public:
    T* a;
};

template <class T> class ZRef
{
public:
    unsigned int Unknown1;
    T* p;
};

template <class T> class ZList
{
public:
    unsigned int Unknown1;
    unsigned int Unknown2;
    unsigned int _m_uCount;
    T* _m_pHead;
    T* _m_pTail;
};

template <class T, class U, class V> class ZMap
{
public:
    struct _PAIR 
    {
        _PAIR* pNext;
        T Key;
        U Value;
    };

    unsigned int Unknown1;
    _PAIR **_m_apTable;
    unsigned int _m_uTableSize;
    unsigned int _m_uCount;
    unsigned int _m_uAutoGrowEvery128;
    unsigned int _m_uAutoGrowLimit;
};

template <class T> class ZXString
{
public:
    T* _m_pStr;
};

template <class T, class U, class V, class W, class X> class TRSTree
{
public:
    struct I2
    {
        long l;
        long t;
        long r;
        long b;
    };

    struct VECTORDATA
    {
        long x0;
        long y0;
        long x1;
        long y1;
        V d;
    };

    struct NODE
    {
        struct ENTRY
        {
            TRSTree<U, V, W, X>::I2 i;
            ZRef<TRSTree<U, V, W, X>::VECTORDATA> pData;
            TRSTree<U, V, W, X>::NODE *pChild;
        };

        TRSTree<T, U, V, W, X>::NODE *pParent;
        TRSTree<T, U, V, W, X>::NODE *pAvNext;
        int nLevel;
        TRSTree<T, U, V, W, X>::NODE::ENTRY E[5];
        int nCount;
        int bValidMBR;
        TRSTree<T, U, V, W, X>::I2 iMBR;
    };
    
    ZList<TRSTree<T, U, V, W, X>::NODE> m_lAllNodes;
    TRSTree<T, U, V, W, X>::NODE *m_pAvNodeHead;
    TRSTree<T, U, V, W, X>::NODE *m_pRoot;
    int m_nCount;
    ZList<ZRef<TRSTree<T, U, V, W, X>::VECTORDATA> > m_lpVecData;
};

class ZRefCounted
{
public:
    long _m_nRef;
    ZRefCounted *_m_pNext;
    ZRefCounted *_m_pPrev;
};

class ZFatalsection
{
public:
    static void *NEAR_FAST ZFatalSection::_s_pfnTry (void *)*;
};

struct CONSTANTS
{
    double dWalkForce;
    double dWalkSpeed;
    double dWalkDrag;
    double dSlipForce;
    double dSlipSpeed;
    double dFloatDrag1;
    double dFloatDrag2;
    double dFloatCoefficient;
    double dSwimForce;
    double dSwimSpeed;
    double dFlyForce;
    double dFlySpeed;
    double dGravityAcc;
    double dFallSpeed;
    double dJumpSpeed;
    double dMaxFriction;
    double dMinFriction;
    double dSwimSpeedDec;
    double dFlyJumpDec;
};

struct RANGE
{
    long low;
    long high;
};
#pragma pack()

#endif //__MSEA_OBJECTS_UTILITY_H