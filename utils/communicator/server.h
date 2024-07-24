#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>
#include <mutex>
#include "Thread/ThreadPool.h"
#include "config.h"
#include "message.h"

/**
 * @brief 服务器类
 *
 * 负责管理服务器的启动、运行和处理客户端连接。
 */
class Server
{
  public:
    /**
     * @brief 构造函数
     *
     * @param config 服务器配置
     */
    Server(const ServerConfig& config);

    /**
     * @brief 运行服务器
     *
     * 开始接受和处理客户端连接。
     */
    void run();

  private:
    /**
     * @brief 处理客户端连接
     *
     * @param client_socket 客户端socket文件描述符
     */
    void handle_client(int client_socket);

    /**
     * @brief 打印客户端信息
     *
     * @param address 客户端地址
     * @param connected 是否连接
     */
    void print_client_info(const sockaddr_in& address, bool connected);

    ServerConfig config_;               ///< 服务器配置
    int          server_fd_;            ///< 服务器socket文件描述符
    fd_set       readfds_;              ///< 文件描述符集合，用于select调用
    ThreadPool   thread_pool_;          ///< 线程池，用于处理客户端连接
    unsigned int current_connections_;  ///< 当前连接数
    std::mutex   connections_mutex_;    ///< 互斥锁，用于保护current_connections_

    /**
     * @brief 绑定并监听
     *
     * 绑定服务器地址并开始监听连接。
     */
    void bind_and_listen();

    /**
     * @brief 接受新连接
     *
     * 接受新的客户端连接并将其分配给线程池。
     */
    void accept_new_connection();

    /**
     * @brief 处理SQL命令
     *
     * @param command SQL命令
     * @return SqlResult* SQL命令的结果
     */
    SqlResult* handle_sql_command(const SqlCommand& command);

    /**
     * @brief 拒绝新连接
     *
     * 当达到最大连接数时拒绝新的客户端连接。
     *
     * @param client_socket 客户端socket文件描述符
     */
    void reject_new_connection(int client_socket);
};