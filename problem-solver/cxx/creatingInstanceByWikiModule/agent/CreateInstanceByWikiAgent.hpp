#pragma once

#include <sc-memory/sc_agent.hpp>

#include "searcher/MessageSearcher.hpp"

namespace creatingInstanceByWikiModule
{
class CreateInstanceByWikiAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

private:
  void createAnswer(std::string message) const;

  std::string getMessageText(ScAddr const & messageAddr) const;

  std::unique_ptr<class dialogControlModule::MessageSearcher> messageSearcher;

  std::string trim(const std::string& str);

  void parseJSON(const std::string& json, std::map<std::string, std::string>& fields, std::map<std::string, std::vector<std::string>>& arrayFields);

  void printParsedJSON(const std::map<std::string, std::string>& fields, const std::map<std::string, std::vector<std::string>>& arrayFields);

  void createMessageAndStructure(std::string conceptName, ScAddr const & answerStructure);
};
}  // namespace creatingInstanceByWikiModule