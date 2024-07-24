#pragma once

#include <string>
#include <fstream>
#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"

/**
 * @brief 服务器配置结构体
 *
 * 包含服务器的相关配置信息，如服务器地址、端口号、缓冲区大小、最大客户端数和B+树搜索线程数。
 */
struct ServerConfig
{
    ServerConfig() = default;
    ServerConfig(const char* file);

    std::string  server_address;      ///< 服务器地址
    unsigned int port;                ///< 服务器端口号
    unsigned int buffer_size;         ///< 缓冲区大小
    unsigned int max_clients;         ///< 最大客户端数
    unsigned int bplus_tree_threads;  ///< B+树搜索线程数

    /**
     * @brief 序列化函数
     *
     * 使用cereal库进行序列化
     *
     * @tparam Archive 序列化归档类型
     * @param ar 序列化归档对象
     */
    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(CEREAL_NVP(server_address),
            CEREAL_NVP(port),
            CEREAL_NVP(buffer_size),
            CEREAL_NVP(max_clients),
            CEREAL_NVP(bplus_tree_threads));
    }

    /**
     * @brief 从文件加载配置
     *
     * @param filename 配置文件名
     */
    void load(const std::string& filename);

    /**
     * @brief 保存配置到文件
     *
     * @param filename 配置文件名
     */
    void save(const std::string& filename) const;
};

/**
 * @brief 客户端配置结构体
 *
 * 包含客户端的相关配置信息，如服务器地址、端口号、缓冲区大小、最大重试次数和重试间隔。
 */
struct ClientConfig
{
    ClientConfig() = default;
    ClientConfig(const char* file);

    std::string  server_address;      ///< 服务器地址
    unsigned int port;                ///< 服务器端口号
    unsigned int buffer_size;         ///< 缓冲区大小
    unsigned int max_retry_attempts;  ///< 最大重试次数
    unsigned int retry_interval;      ///< 重试间隔

    /**
     * @brief 序列化函数
     *
     * 使用cereal库进行序列化
     *
     * @tparam Archive 序列化归档类型
     * @param ar 序列化归档对象
     */
    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(CEREAL_NVP(server_address),
            CEREAL_NVP(port),
            CEREAL_NVP(buffer_size),
            CEREAL_NVP(max_retry_attempts),
            CEREAL_NVP(retry_interval));
    }

    /**
     * @brief 从文件加载配置
     *
     * @param filename 配置文件名
     */
    void load(const std::string& filename);

    /**
     * @brief 保存配置到文件
     *
     * @param filename 配置文件名
     */
    void save(const std::string& filename) const;
};