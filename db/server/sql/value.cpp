#include "value.h"
#include "Trans/date.h"
#include <cstring>
#include "ret.h"

const char* AttrTypeStr[] = {
    "UNDEFINED",
    "CHARS",
    "INTS",
    "FLOATS",
    "DATES",
    "BOOLEANS",
};

const char* strat(AttrType type)
{
    if (type < AttrType::UNDEFINED || type > AttrType::BOOLEANS) return "UNDEFINED";
    return AttrTypeStr[type];
}

AttrType atstr(const char* str)
{
    for (unsigned int i = 0; i < sizeof(AttrTypeStr) / sizeof(AttrTypeStr[0]); i++)
        if (strcmp(str, AttrTypeStr[i]) == 0) return (AttrType)i;
    return AttrType::UNDEFINED;
}

Value::Value(int val, RC& rc) { set_int(val, rc); }
Value::Value(float val, RC& rc) { set_float(val, rc); }
Value::Value(bool val, RC& rc) { set_bool(val, rc); }
Value::Value(const char* str, RC& rc) { set_str(str, rc); }
Value::Value(const char* date, int this_is_date, RC& rc) { set_date(date, rc); }

void Value::set_int(int val, RC& rc)
{
    attr_type_            = AttrType::INTS;
    num_value_.int_value_ = val;
    length_               = sizeof(val);
}
void Value::set_float(float val, RC& rc)
{
    attr_type_              = AttrType::FLOATS;
    num_value_.float_value_ = val;
    length_                 = sizeof(val);
}
void Value::set_bool(bool val, RC& rc)
{
    attr_type_             = AttrType::BOOLEANS;
    num_value_.bool_value_ = val;
    length_                = sizeof(val);
}
void Value::set_str(const char* str, RC& rc)
{
    attr_type_ = AttrType::CHARS;
    str_value_ = str;
    length_    = str_value_.length();
}
void Value::set_date(int date, RC& rc)
{
    attr_type_            = AttrType::DATES;
    num_value_.int_value_ = date;
    length_               = sizeof(date);
}
void Value::set_date(const char* date, RC& rc)
{
    int date_int = 0;
    StrDate2IntDate(date, date_int, rc);
    if (rc != RC::SUCCESS) return;
    set_date(date_int, rc);
}

int Value::get_int(RC& rc) const
{
    if (attr_type_ == AttrType::INTS) return num_value_.int_value_;
}