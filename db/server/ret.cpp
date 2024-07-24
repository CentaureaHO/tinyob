#include "ret.h"

RC& operator++(RC& rc)
{
    if (rc == RC::UNKNOW_RET)
        rc = RC::SUCCESS;
    else
        rc = static_cast<RC>(static_cast<int>(rc) + 1);
    return rc;
}

RC operator++(RC& rc, int)
{
    RC old = rc;
    if (rc == RC::UNKNOW_RET)
        rc = RC::SUCCESS;
    else
        rc = static_cast<RC>(static_cast<int>(rc) + 1);
    return old;
}

const char* strrc(RC rc)
{
    if (rc < RC::SUCCESS || rc >= RC::UNKNOW_RET) return "UNKNOWN_RET";

    switch (rc)
    {
#define RET_CODE(code) \
    case RC::code: return #code;
        RET_CODES
#undef RET_CODE
    }

    // 正常情况下不会执行到这里
    return "UNKNOWN_RET";
}

bool SUCC(RC rc) { return rc == RC::SUCCESS; }

bool FAIL(RC rc) { return rc != RC::SUCCESS; }
