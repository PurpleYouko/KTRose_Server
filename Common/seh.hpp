
#ifndef __FV_SEH_H__
#define __FV_SEH_H__

#ifdef __WIN32__
#include <windows.h>
#include <excpt.h>
#endif

#include <setjmp.h>
//#include <fv/defs.hpp>

//NAMESPACE_BEGIN(PU)
//NAMESPACE_BEGIN(SEH)

#ifdef __WIN32__

class __SEH_HANDLER;

typedef struct tag__SEH_EXCEPTION_REGISTRATION
{
    tag__SEH_EXCEPTION_REGISTRATION* prev;
    PEXCEPTION_HANDLER handler;
    __SEH_HANDLER* exthandler;
} __SEH_EXCEPTION_REGISTRATION;


class __SEH_HANDLER
{
public:
    __SEH_HANDLER() { }
    static EXCEPTION_DISPOSITION ExceptionRouter(PEXCEPTION_RECORD pRecord,
                                                 __SEH_EXCEPTION_REGISTRATION* pReg,
                                                 PCONTEXT pContext,
                                                 PEXCEPTION_RECORD pRecord2);

    virtual EXCEPTION_DISPOSITION ExceptionHandler(PEXCEPTION_RECORD pRecord,
                                                   __SEH_EXCEPTION_REGISTRATION* pReg,
                                                   PCONTEXT pContext,
                                                   PEXCEPTION_RECORD pRecord2);

    jmp_buf context;
    EXCEPTION_RECORD excRecord;
    CONTEXT excContext;
};

#define __seh_try                                                      \
{                                                                      \
    __SEH_EXCEPTION_REGISTRATION _lseh_er;                             \
    __SEH_HANDLER _lseh_handler;                                       \
                                                                       \
    _lseh_er.handler = \
        reinterpret_cast<PEXCEPTION_HANDLER>(__SEH_HANDLER::ExceptionRouter);     \
    _lseh_er.exthandler = &_lseh_handler;                               \
    asm volatile ("movl %%fs:0, %0" : "=r" (_lseh_er.prev));           \
    asm volatile ("movl %0, %%fs:0" : : "r" (&_lseh_er));              \
    int _lseh_setjmp_res = setjmp(_lseh_handler.context);              \
    while(true) {                                                      \
        if(_lseh_setjmp_res != 0) {                                    \
            break;                                                     \
        }                                                              \

#define __seh_finally                                                  \
        break;                                                         \
    }                                                                  \
    while(true) {


#define __seh_except(rec, ctx)                                         \
        break;                                                         \
    }                                                                  \
    PEXCEPTION_RECORD rec = &_lseh_handler.excRecord;                  \
    PCONTEXT ctx = &_lseh_handler.excContext;                          \
                                                                       \
    asm volatile ("movl %0, %%fs:0" : : "r" (_lseh_er.prev));          \
    if(_lseh_setjmp_res != 0)

#define __seh_end }

#else

#define __seh_try

#define __seh_except(rec, ctx) if(false) {

#define __seh_end }

#define __seh_finally

#endif

//NAMESPACE_END(SEH)
//NAMESPACE_END(PU)

#endif

