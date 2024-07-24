#include <sstream>
#include <cereal/archives/json.hpp>
#include "communicator/message.h"

int main()
{
    std::string input = "{\"value0\":{\"base\":{\"polymorphic_id\":2147483649,\"polymorphic_name\":\"SqlCommand\","
                        "\"ptr_wrapper\":{\"id\":1,\"data\":{\"query\":\"SELECT * FROM users\"}}}}}";
    std::stringstream ss(input);
    {
        cereal::JSONInputArchive iarchive(ss);
        std::shared_ptr<Message> message;
        iarchive(message);
        auto command = std::dynamic_pointer_cast<SqlCommand>(message);
        if (command) { std::cout << "Deserialized SqlCommand query: " << command->query << std::endl; }
    }
    return 0;
}
