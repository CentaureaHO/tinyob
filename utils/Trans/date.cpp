#include "Trans/date.h"
#include <sstream>
#include <iomanip>
#include "ret.h"

const int MinInt = 0x80000000;

/**
 * @brief 判断是否为闰年
 *
 * @param Year 年份
 * @return 是否为闰年
 */
bool IsLeapYear(int Year)
{
    if ((Year % 4 == 0 && Year % 100 != 0) || Year % 400 == 0) return 1;
    return 0;
}

/**
 * @brief 判断日期是否有效
 *
 * @param y 年
 * @param m 月
 * @param d 日
 * @return 日期是否有效
 */
bool IsDateValid(int y, int m, int d)
{
    if (y > 5000000) return 0;
    int MonthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (IsLeapYear(y)) MonthDays[2] = 29;

    return m > 0 && m <= 12 && d > 0 && d <= MonthDays[m];
}

/**
 * @brief 将字符串日期转换为整数日期
 *
 * @param StrDate 输入的字符串日期，格式为"YYYY-MM-DD"
 * @param IntDate 输出的整数日期
 * @param rc 返回码，用于指示转换是否成功
 */
void StrDate2IntDate(const char* StrDate, int& IntDate, RC& rc)
{
    int Year, Month, Day;
    sscanf(StrDate, "%d-%d-%d", &Year, &Month, &Day);
    if (!IsDateValid(Year, Month, Day))
    {
        IntDate = MinInt;
        rc      = RC::INVALID_DATE;
        return;
    }

    bool IsBefore1970 = Year < 1970;
    IntDate           = 0;
    if (IsBefore1970)
        for (int y = 1969; y > Year; --y) IntDate -= (IsLeapYear(y) ? 366 : 365);
    else
        for (int y = 1970; y < Year; ++y) IntDate += (IsLeapYear(y) ? 366 : 365);

    int MonthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (IsLeapYear(Year)) MonthDays[2] = 29;

    for (int i = 0; i < Month - 1; ++i) IntDate += MonthDays[i + 1];
    IntDate += Day - 1;

    if (IsBefore1970) IntDate = IntDate - 365 - (IsLeapYear(Year) ? 1 : 0);
}

/**
 * @brief 将整数日期转换为字符串日期
 *
 * @param IntDate 输入的整数日期
 * @return 转换后的字符串日期，格式为"YYYY-MM-DD"
 */
std::string IntDate2StrDate(int IntDate)
{
    int Year = 1970;
    while (IntDate < 0 || IntDate >= (IsLeapYear(Year) ? 366 : 365))
    {
        if (IntDate < 0)
        {
            --Year;
            IntDate += IsLeapYear(Year) ? 366 : 365;
        }
        else
        {
            IntDate -= IsLeapYear(Year) ? 366 : 365;
            ++Year;
        }
    }

    int MonthDays[] = {31, IsLeapYear(Year) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int Month       = 0;
    while (IntDate >= MonthDays[Month])
    {
        IntDate -= MonthDays[Month];
        ++Month;
    }
    int Day = IntDate + 1;
    Month += 1;
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << Year << "-" << std::setw(2) << std::setfill('0') << Month << "-"
        << std::setw(2) << std::setfill('0') << Day;

    return oss.str();
}

/**
 * @brief 检查日期是否有效
 *
 * @param IntDate 输入的整数日期
 * @return 日期是否有效
 */
bool CheckDate(int IntDate) { return IntDate != MinInt; }
