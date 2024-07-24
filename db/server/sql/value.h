#pragma once

#include <string>

enum class RC;

enum AttrType
{
    UNDEFINED,
    CHARS,
    INTS,
    FLOATS,
    DATES,
    BOOLEANS,
};

const char* strat(AttrType type);
AttrType    atstr(const char* str);

class Value
{
  public:
    Value() = default;

    Value(int val, RC& rc);
    Value(float val, RC& rc);
    Value(bool val, RC& rc);
    Value(const char* str, RC& rc);
    Value(const char* date, int this_is_date, RC& rc);

    void set_int(int val, RC& rc);
    void set_float(float val, RC& rc);
    void set_bool(bool val, RC& rc);
    void set_str(const char* str, RC& rc);
    void set_date(int date, RC& rc);
    void set_date(const char* date, RC& rc);

    int  get_int(RC& rc) const;
    float get_float(RC& rc) const;
    bool get_bool(RC& rc) const;
    const char* get_str(RC& rc) const;
    int get_date(RC& rc) const;

  private:
    AttrType attr_type_ = UNDEFINED;
    int      length_    = 0;

    union
    {
        int   int_value_;
        float float_value_;
        bool  bool_value_;
    } num_value_;
    std::string str_value_;
};