#pragma once

/**
 * @brief 定义执行查询语句过程中可能的状态码
 */
#define RET_CODES              \
    RET_CODE(SUCCESS)          \
    RET_CODE(UNIMPLENMENT)     \
    RET_CODE(SQL_SYNTAX)       \
    RET_CODE(INVALID_ARGUMENT) \
    RET_CODE(INVALID_DATE)     \
    RET_CODE(DB_NOT_EXIST)     \
    RET_CODE(TABLE_NOT_EXIST)  \
    RET_CODE(COLUMN_NOT_EXIST) \
    RET_CODE(LOCKED)           \
    RET_CODE(OTHER_RET)        \
    RET_CODE(UNKNOW_RET)

/**
 * @brief 根据宏定义生成枚举类型
 */
enum class RC
{
#define RET_CODE(code) code,
    RET_CODES
#undef RET_CODE
};

/**
 * @brief 重载前置自增运算符
 *
 * C++支持运算符重载，此处重载前置自增运算符。
 * 习惯上前置自增返回引用，便于链式调用。
 * 仅为调试时使用。
 *
 * @param rc 要自增的RC枚举
 * @return 自增后的RC枚举
 */
RC& operator++(RC& rc);

/**
 * @brief 重载后置自增运算符
 *
 * C++支持运算符重载，此处重载后置自增运算符。
 * 仅为调试时使用。
 *
 * @param rc 要自增的RC枚举
 * @param unused 用于区分前置和后置自增运算符的占位参数
 * @return 自增前的RC枚举
 */
RC operator++(RC& rc, int);

/**
 * @brief 返回RC对应的字符串
 *
 * 用于打印执行结果。
 *
 * @param rc 要转换为字符串的RC枚举
 * @return 对应的字符串
 */
extern const char* strrc(RC rc);

/**
 * @brief 判断操作是否成功
 *
 * @param rc 要判断的RC枚举
 * @return 操作是否成功
 */
extern bool SUCC(RC rc);

/**
 * @brief 判断操作是否失败
 *
 * @param rc 要判断的RC枚举
 * @return 操作是否失败
 */
extern bool FAIL(RC rc);
