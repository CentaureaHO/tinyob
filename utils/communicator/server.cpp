#include "server.h"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>

/**
 * @brief 服务器构造函数
 *
 * @param config 服务器配置
 */
Server::Server(const ServerConfig& config) : config_(config), thread_pool_(config.max_clients), current_connections_(0)
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    bind_and_listen();
}

/**
 * @brief 运行服务器
 *
 * 开始接受和处理客户端连接。
 */
void Server::run()
{
    while (true)
    {
        FD_ZERO(&readfds_);
        FD_SET(server_fd_, &readfds_);
        int max_sd = server_fd_;

        int activity = select(max_sd + 1, &readfds_, nullptr, nullptr, nullptr);
        if ((activity < 0) && (errno != EINTR)) { std::cerr << "select error\n"; }

        if (FD_ISSET(server_fd_, &readfds_)) { accept_new_connection(); }
    }
}

/**
 * @brief 处理客户端连接
 *
 * @param client_socket 客户端socket文件描述符
 */
void Server::handle_client(int client_socket)
{
    sockaddr_in client_addr;
    socklen_t   addr_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len);

    while (true)
    {
        std::vector<char> buffer(config_.buffer_size);
        int               valread = read(client_socket, buffer.data(), config_.buffer_size);

        if (valread == 0)
        {
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(connections_mutex_);
                current_connections_--;
            }
            print_client_info(client_addr, false);
            break;
        }
        else if (valread < 0)
        {
            std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(connections_mutex_);
                current_connections_--;
            }
            print_client_info(client_addr, false);
            break;
        }

        buffer[valread] = '\0';
        std::string received_data(buffer.data(), valread);

        std::stringstream is;
        is << received_data;
        cereal::JSONInputArchive archive(is);

        std::unique_ptr<Message> command;
        try
        {
            archive(cereal::make_nvp("command", command));
        } catch (const std::exception& e)
        {
            std::cerr << "Error deserializing command: " << e.what() << std::endl;
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(connections_mutex_);
                current_connections_--;
            }
            print_client_info(client_addr, false);
            break;
        }

        if (SqlCommand* sqlCmd = dynamic_cast<SqlCommand*>(command.get()))
        {
            std::unique_ptr<SqlResult> result(handle_sql_command(*sqlCmd));
            std::ostringstream         os;
            {
                cereal::JSONOutputArchive output_archive(os);
                output_archive(cereal::make_nvp("response", result));
            }

            std::string response = os.str();
            send(client_socket, response.c_str(), response.length(), 0);

            if (dynamic_cast<SqlExecuteResult*>(result.get()))
                ;
            else if (dynamic_cast<SqlQueryResult*>(result.get()))
                ;
            else
            {
                std::cerr << "Received an unknown result type\n";
                close(client_socket);
                {
                    std::lock_guard<std::mutex> lock(connections_mutex_);
                    current_connections_--;
                }
                print_client_info(client_addr, false);
                break;
            }

            if (result->need_disconnect)
            {
                close(client_socket);
                {
                    std::lock_guard<std::mutex> lock(connections_mutex_);
                    current_connections_--;
                }
                print_client_info(client_addr, false);
                break;
            }
        }
        else
        {
            std::cerr << "Received an unknown command type\n";
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(connections_mutex_);
                current_connections_--;
            }
            print_client_info(client_addr, false);
            break;
        }
    }
}

/**
 * @brief 绑定并监听
 *
 * 绑定服务器地址并开始监听连接。
 */
void Server::bind_and_listen()
{
    struct sockaddr_in address;
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(config_.server_address.c_str());
    int  port               = config_.port;
    bool bound              = false;

    while (!bound)
    {
        address.sin_port = htons(port);
        if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0)
        {
            std::cerr << "bind failed on port " << port << ": " << strerror(errno) << std::endl;
            port++;
            if (port > static_cast<int>(config_.port + 10))
            {
                std::cerr << "All ports in range are unavailable" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            bound = true;
            std::cout << "Server is listening on port " << port << std::endl;
        }
    }

    if (listen(server_fd_, config_.max_clients) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief 接受新连接
 *
 * 接受新的客户端连接并将其分配给线程池。
 */
void Server::accept_new_connection()
{
    struct sockaddr_in address;
    socklen_t          addrlen    = sizeof(address);
    int                new_socket = accept(server_fd_, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        if (current_connections_ >= config_.max_clients)
        {
            std::cerr << "Max client connections reached, rejecting new connection" << std::endl;
            reject_new_connection(new_socket);
            return;
        }

        current_connections_++;
    }

    std::cout << "New connection, socket fd is " << new_socket << ", ip is: " << inet_ntoa(address.sin_addr)
              << ", port: " << ntohs(address.sin_port) << std::endl;

    thread_pool_.EnQueue(&Server::handle_client, this, new_socket);
}

/**
 * @brief 拒绝新连接
 *
 * 当达到最大连接数时拒绝新的客户端连接。
 *
 * @param client_socket 客户端socket文件描述符
 */
void Server::reject_new_connection(int client_socket)
{
    std::ostringstream os;
    {
        cereal::JSONOutputArchive archive(os);
        std::unique_ptr<Message>  cmd                               = std::make_unique<SqlExecuteResult>();
        dynamic_cast<SqlExecuteResult*>(cmd.get())->extra_info      = "Max connections reached";
        dynamic_cast<SqlExecuteResult*>(cmd.get())->need_disconnect = true;
        archive(cereal::make_nvp("response", cmd));
    }

    std::string response = os.str();
    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
}

/**
 * @brief 处理SQL命令
 *
 * @param command SQL命令
 * @return SqlResult* SQL命令的结果
 */
SqlResult* Server::handle_sql_command(const SqlCommand& command)
{
    if (command.query == "execute")
    {
        SqlExecuteResult* execute_result = new SqlExecuteResult();
        execute_result->extra_info       = "Execution successful";
        execute_result->need_disconnect  = 0;
        return execute_result;
    }
    else if (command.query == "exit")
    {
        SqlExecuteResult* execute_result = new SqlExecuteResult();
        execute_result->extra_info       = "Exiting";
        execute_result->need_disconnect  = 1;
        return execute_result;
    }
    else
    {
        SqlQueryResult* query_result  = new SqlQueryResult();
        query_result->results         = {{"Result1", "Result2"}, {"Row2Col1", "Row2Col2"}};
        query_result->need_disconnect = 0;
        return query_result;
    }
}

/**
 * @brief 打印客户端信息
 *
 * @param address 客户端地址
 * @param connected 是否连接
 */
void Server::print_client_info(const sockaddr_in& address, bool connected)
{
    std::cout << "Client " << (connected ? "connected" : "disconnected") << ". IP: " << inet_ntoa(address.sin_addr)
              << ", Port: " << ntohs(address.sin_port) << std::endl;
}