#pragma once

#include <string>

/**
 * @brief 返回码枚举
 */
enum class RC;

/**
 * @brief 将字符串日期转换为整数日期
 *
 * @param StrDate 输入的字符串日期，格式为"YYYY-MM-DD"
 * @param IntDate 输出的整数日期
 * @param rc 返回码，用于指示转换是否成功
 */
void StrDate2IntDate(const char* StrDate, int& IntDate, RC& rc);

/**
 * @brief 将整数日期转换为字符串日期
 *
 * @param IntDate 输入的整数日期
 * @return 转换后的字符串日期，格式为"YYYY-MM-DD"
 */
std::string IntDate2StrDate(int IntDate);
