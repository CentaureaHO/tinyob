#include "client.h"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <sstream>
#include <thread>
#include <chrono>

/**
 * @brief 客户端构造函数
 *
 * @param config 客户端配置
 */
Client::Client(const ClientConfig& config) : config_(config), stop_client_(false) {}

/**
 * @brief 运行客户端
 *
 * 开始与服务器通信，发送和接收消息。
 */
void Client::run()
{
    int retry_count = 0;

    while (retry_count < config_.max_retry_attempts && !stop_client_)
    {
        int sock;
        if (connect_to_server(sock))
        {
            while (true)
            {
                if (stop_client_)
                {
                    std::cerr << "Stopping client as instructed by the server." << std::endl;
                    close(sock);
                    return;
                }

                std::string query;
                std::cout << "Enter SQL command: ";
                std::getline(std::cin, query);

                SqlCommand command;
                command.query = query;

                send_message(sock, command);

                if (query == "exit")
                {
                    std::cout << "Exiting client." << std::endl;
                    close(sock);
                    return;
                }
            }
        }
        else
        {
            std::cerr << "Failed to connect to server. Retrying in " << config_.retry_interval << " seconds..."
                      << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(config_.retry_interval));
            retry_count++;
        }
    }

    if (!stop_client_) { std::cerr << "Reached maximum retry attempts. Exiting client." << std::endl; }
}

/**
 * @brief 发送消息到服务器
 *
 * @param sock 客户端socket文件描述符
 * @param command 要发送的SQL命令
 */
void Client::send_message(int& sock, const SqlCommand& command)
{
    std::ostringstream os;
    {
        cereal::JSONOutputArchive archive(os);
        std::unique_ptr<Message>  cmd = std::make_unique<SqlCommand>(command);
        archive(cereal::make_nvp("command", cmd));
    }

    std::string message = os.str();
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Message sent" << std::endl;

    std::vector<char> buffer(config_.buffer_size, 0);
    int               valread = read(sock, buffer.data(), config_.buffer_size);

    if (valread <= 0)
    {
        std::cerr << "Server disconnected or error occurred" << std::endl;
        close(sock);
        return;
    }

    buffer[valread] = '\0';
    std::string response_data(buffer.data(), valread);

    std::istringstream       is(response_data);
    cereal::JSONInputArchive archive(is);

    std::unique_ptr<SqlResult> result;
    try
    {
        archive(cereal::make_nvp("response", result));
    } catch (const std::exception& e)
    {
        std::cerr << "Error deserializing result: " << e.what() << std::endl;
        close(sock);
        return;
    }

    if (result)
    {
        std::cout << "Server response: " << response_data << std::endl;

        if (result->need_disconnect)
        {
            if (dynamic_cast<SqlExecuteResult*>(result.get())->extra_info == "Max connections reached")
            {
                std::cerr << "Max connections reached. Stopping client." << std::endl;
                stop_client_ = true;  // 设置标志位，指示客户端应该停止
                close(sock);
                return;
            }

            std::cout << "Server requested disconnect" << std::endl;
            close(sock);
        }
    }
    else { std::cerr << "Failed to deserialize server response" << std::endl; }
}

/**
 * @brief 连接到服务器
 *
 * @param sock 客户端socket文件描述符
 * @return 是否成功连接到服务器
 */
bool Client::connect_to_server(int& sock)
{
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(config_.port);

    if (inet_pton(AF_INET, config_.server_address.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    return true;
}
