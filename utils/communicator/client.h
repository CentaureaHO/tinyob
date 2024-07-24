#pragma once

#include <string>
#include "config.h"
#include "message.h"

/**
 * @brief 客户端类
 *
 * 负责管理客户端的启动、运行和与服务器的通信。
 */
class Client
{
  public:
    /**
     * @brief 构造函数
     *
     * @param config 客户端配置
     */
    Client(const ClientConfig& config);

    /**
     * @brief 运行客户端
     *
     * 开始与服务器通信，发送和接收消息。
     */
    void run();

  private:
    ClientConfig config_;       ///< 客户端配置
    bool         stop_client_;  ///< 标志位，用于指示是否应该停止客户端

    /**
     * @brief 发送消息到服务器
     *
     * @param sock 客户端socket文件描述符
     * @param command 要发送的SQL命令
     */
    void send_message(int& sock, const SqlCommand& command);

    /**
     * @brief 连接到服务器
     *
     * @param sock 客户端socket文件描述符
     * @return 是否成功连接到服务器
     */
    bool connect_to_server(int& sock);
};
