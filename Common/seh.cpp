
//#include <cstring>
#include "seh.hpp"

#ifdef __WIN32__

EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionRouter(PEXCEPTION_RECORD pRecord,
                                                     __SEH_EXCEPTION_REGISTRATION* pReg,
                                                     PCONTEXT pContext,
                                                     PEXCEPTION_RECORD pRecord2)
{
    return pReg->exthandler->ExceptionHandler(pRecord, pReg, pContext, pRecord2);
}

EXCEPTION_DISPOSITION __SEH_HANDLER::ExceptionHandler(PEXCEPTION_RECORD pRecord,
                                                      __SEH_EXCEPTION_REGISTRATION* pReg,
                                                      PCONTEXT pContext,
                                                      PEXCEPTION_RECORD pRecord2)
{
    /*std::memcpy(&excContext, pContext, sizeof(_CONTEXT));
    std::memcpy(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));*/
    memcpy(&excContext, pContext, sizeof(_CONTEXT));
    memcpy(&excRecord, pRecord, sizeof(_EXCEPTION_RECORD));
    longjmp(context, 1);
}

#endif


