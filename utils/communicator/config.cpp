#include "config.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// ServerConfig

/**
 * @brief ServerConfig构造函数
 *
 * 从配置文件加载服务器配置
 *
 * @param file 配置文件名
 */
ServerConfig::ServerConfig(const char* file) { load(file); }

/**
 * @brief 从文件加载服务器配置
 *
 * @param filename 配置文件名
 */
void ServerConfig::load(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) { throw runtime_error("Could not open config file: " + filename); }
    try
    {
        cereal::JSONInputArchive archive(file);
        archive(cereal::make_nvp("server", *this));
        cout << "Loaded server config: server_address = " << server_address << ", port = " << port
             << ", buffer_size = " << buffer_size << ", max_clients = " << max_clients
             << ", bplus_tree_threads = " << bplus_tree_threads << endl;
    } catch (const cereal::Exception& e)
    {
        throw runtime_error("Failed to load config: " + string(e.what()));
    }
}

/**
 * @brief 保存服务器配置到文件
 *
 * @param filename 配置文件名
 */
void ServerConfig::save(const string& filename) const
{
    ofstream file(filename);
    if (!file.is_open()) { throw runtime_error("Could not open config file: " + filename); }
    try
    {
        cereal::JSONOutputArchive archive(file);
        archive(cereal::make_nvp("server", *this));
        cout << "Server config saved to " << filename << endl;
    } catch (const cereal::Exception& e)
    {
        throw std::runtime_error("Failed to save config: " + std::string(e.what()));
    }
}

// ClientConfig

/**
 * @brief ClientConfig构造函数
 *
 * 从配置文件加载客户端配置
 *
 * @param file 配置文件名
 */
ClientConfig::ClientConfig(const char* file) { load(file); }

/**
 * @brief 从文件加载客户端配置
 *
 * @param filename 配置文件名
 */
void ClientConfig::load(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) { throw runtime_error("Could not open config file: " + filename); }
    try
    {
        cereal::JSONInputArchive archive(file);
        archive(cereal::make_nvp("client", *this));
        cout << "Loaded client config: server_address = " << server_address << ", port = " << port
             << ", buffer_size = " << buffer_size << ", max_retry_attempts = " << max_retry_attempts
             << ", retry_interval = " << retry_interval << endl;
    } catch (const cereal::Exception& e)
    {
        throw runtime_error("Failed to load config: " + string(e.what()));
    }
}

/**
 * @brief 保存客户端配置到文件
 *
 * @param filename 配置文件名
 */
void ClientConfig::save(const string& filename) const
{
    ofstream file(filename);
    if (!file.is_open()) { throw runtime_error("Could not open config file: " + filename); }
    try
    {
        cereal::JSONOutputArchive archive(file);
        archive(cereal::make_nvp("client", *this));
        cout << "Client config saved to " << filename << endl;
    } catch (const cereal::Exception& e)
    {
        throw std::runtime_error("Failed to save config: " + std::string(e.what()));
    }
}