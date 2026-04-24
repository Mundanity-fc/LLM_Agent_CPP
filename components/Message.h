#include <string>
#include <boost/json.hpp>

class Message {
public:
    Message(){}
    void addSystemPrompt(const std::string& newPrompt);
    void addUserPrompt(const std::string& newPrompt);
    void addAssistantPrompt(const std::string& newPrompt);

    [[nodiscard]] boost::json::array getJsonPrompt() const;
    [[nodiscard]] std::string getStringPrompt() const;
private:
    std::vector<boost::json::object> history;
    int totalHistory = 0;
    int totalUserHistory = 0;
    int totalAssistantHistory = 0;
};

