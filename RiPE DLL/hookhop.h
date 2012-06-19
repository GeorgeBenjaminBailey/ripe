#ifndef __HOOK_HOP_H
#define __HOOK_HOP_H

// a class that replaces the first 5 bytes of a function with
// mov edi,edi
// push ebp
// mov ebp,esp
#pragma warning( push )
#pragma warning( disable : 4701 4731 4740 )
class HookHop
{
public:
    HookHop()
    {
    }

    ~HookHop()
    {
    }

    static void * FunctionHop( void *function )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T>
    static void * FunctionHop( void *function, T a )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U>
    static void * FunctionHop( void *function, T a, U b )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U, typename V>
    static void * FunctionHop( void *function, T a, U b, V c )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U, typename V, typename W>
    static void * FunctionHop( void *function, T a, U b, V c, W d )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U, typename V, typename W, typename X>
    static void * FunctionHop( void *function, T a, U b, V c, W d, X e )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U, typename V, typename W, typename X, typename Y>
    static void * FunctionHop( void *function, T a, U b, V c, W d, X e, Y f )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename T, typename U, typename V, typename W, typename X, typename Y, typename Z>
    static void * FunctionHop( void *function, T a, U b, V c, W d, X e, Y f, Z g )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename S, typename T, typename U, typename V, typename W, typename X, typename Y, typename Z>
    static void * FunctionHop( void *function, S a, T b, U c, V d, W e, X f, Y g, Z h )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
    ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push k
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push l
            push k
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push m
            push l
            push k
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push n
            push m
            push l
            push k
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }

    template<typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N, typename O>
    static void * FunctionHop( void *function, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o )
    {
        __asm
        {
            mov  eax,function // temporary holder for function
            add  eax,5   // function offset

            //parameters
            push o
            push n
            push m
            push l
            push k
            push j
            push i
            push h
            push g
            push f
            push e
            push d
            push c
            push b
            push a

            push ReturnLocation
            mov  edi,edi // first 5 bytes of the function
            push ebp
            mov  ebp,esp
            jmp  eax
ReturnLocation:
        }
    }
};
#pragma warning( pop )

#endif //__HOOK_HOP_H