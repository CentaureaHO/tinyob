#pragma once

#include <string>
#include <vector>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>

/**
 * @brief 消息基类
 * 所有消息类的基类，包含一个虚析构函数和一个空的序列化函数。
 */
class Message
{
  public:
    virtual ~Message() = default;

    template <class Archive>
    void serialize(Archive& ar)
    {}
};

/**
 * @brief SQL命令类
 * 表示一个SQL命令，包含一个SQL查询字符串。
 * 实际也复用于客户端的连接断开请求。
 */
class SqlCommand : public Message
{
  public:
    std::string query;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("base", cereal::base_class<Message>(this)), CEREAL_NVP(query));
    }
};

/**
 * @brief 结果基类
 * 表示一个客户端命令的结果，包含一个是否需要断开连接的标志。
 */
class SqlResult : public Message
{
  public:
    bool need_disconnect;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("base", cereal::base_class<Message>(this)), CEREAL_NVP(need_disconnect));
    }
};

/**
 * @brief 执行结果类
 * 表示一个客户端执行命令的结果，包括sql操作返回的信息以及客户端连接的额外信息。
 */
class SqlExecuteResult : public SqlResult
{
  public:
    std::string extra_info;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("base", cereal::base_class<SqlResult>(this)), CEREAL_NVP(extra_info));
    }
};

/**
 * @brief SQL查询结果类
 * 表示一个SQL查询命令的结果，包含查询结果的二维字符串数组。
 */
class SqlQueryResult : public SqlResult
{
  public:
    std::vector<std::vector<std::string>> results;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::make_nvp("base", cereal::base_class<SqlResult>(this)), CEREAL_NVP(results));
    }
};
