#pragma once

#include <sc-memory/sc_agent.hpp>
namespace interfaceModule
{
class CreateRelationAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;

private:
  void createAnswer(std::string message);
  void createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure);
  bool containsOnlyEnglish(std::string name);
};

}  // namespace interfaceModule
