#include <string>
#include <boost/json.hpp>

class Message {
public:
    Message(){}
    void editSystemPrompt(const std::string& newPrompt);
    void editUserPrompt(const std::string& newPrompt);

    [[nodiscard]] boost::json::array getJsonPrompt() const;
    [[nodiscard]] std::string getStringPrompt() const;
private:
    std::string systemPrompt;
    std::string userPrompt;
};

