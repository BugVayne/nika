#include "CreateInstanceByWikiAgent.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"
#include "keynodes/CreatingInstanceByWikiKeynodes.hpp"

#include "generator/MessageConstructionsGenerator.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

using namespace std;
using namespace dialogControlModule;
using namespace commonModule;


using namespace utils;

namespace creatingInstanceByWikiModule
{

ScResult CreateInstanceByWikiAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  ScAddr const & messageAddr = action.GetArgument(ScKeynodes::rrel_1);
  if (!messageAddr.IsValid())
  {
    SC_AGENT_LOG_ERROR("The message isn’t valid");
    return action.FinishUnsuccessfully();
  }

  if (!m_context.CheckConnector(
          CreatingInstanceByWikiKeynodes::concept_creating_new_instance,
          messageAddr,
          ScType::EdgeAccessConstPosPerm))
  {
    SC_AGENT_LOG_DEBUG("The message isn’t about letter search");
    return action.FinishUnsuccessfully();
  }
  
  ScAddr const & entityAddr =
        utils::IteratorUtils::getAnyByOutRelation(&m_context, messageAddr, CreatingInstanceByWikiKeynodes::rrel_entity);
  std::string word;
  m_context.GetLinkContent(entityAddr, word);
  SC_LOG_DEBUG(word);
  
  std::string command = "./wikipedia_fetcher \"" + word + "\"";
  int ret = system(command.c_str());
  SC_LOG_DEBUG(ret);
  if (ret != 0) {
    SC_LOG_DEBUG("Ошибка при запуске исполняемого файла");
  }
  else {
  SC_LOG_DEBUG("Исполняемый файл выполнен, данные сохранены в wiki_result.json");
  }
  
  const std::string filename = "wiki_result.json";

  std::ifstream file(filename);
  if (!file.is_open()) {
    SC_LOG_DEBUG("Ошибка: не удалось открыть файл");
  }

  std::string json, line;
  while (std::getline(file, line)) {
    json += line;
  }
  file.close();
  
  std::map<std::string, std::string> fields;
  std::map<std::string, std::vector<std::string>> arrayFields;

  parseJSON(json, fields, arrayFields);
  
  printParsedJSON(fields, arrayFields);

  ScAddr const & system_idtf = m_context.GenerateNode(ScType::NodeConstClass);
  m_context.SetElementSystemIdentifier(fields["Название на английском"], system_idtf);
  
  ScAddr const & ru_idtf = m_context.GenerateLink(ScType::LinkConst);
  m_context.SetLinkContent(ru_idtf, fields["Главное слово"]);
  
  std::string note = fields["Определение"];
  ScAddr const & note_node = m_context.GenerateLink(ScType::LinkConst);
  m_context.SetLinkContent(note_node, note);
  
  std::vector<ScAddr> decomposition;
  if (arrayFields["Виды"].size() == 0){
    SC_LOG_DEBUG("Нет информации о видах");
  }
  else {
    for (int i=0; i < arrayFields["Виды"].size(); i++){
      ScAddr const & link = m_context.GenerateLink(ScType::LinkConst);
      m_context.SetLinkContent(link, arrayFields["Виды"][i]);
      decomposition.push_back(link);
    }
  }

  std::vector<ScAddr> classification;
  if(arrayFields["Классификация"].size() == 0){
    SC_LOG_DEBUG("Нет информации о классицикации");
  }
  else{
    for(int i=0; i < arrayFields["Классификация"].size(); i++){
      ScAddr const & link = m_context.GenerateLink(ScType::LinkConst);
      m_context.SetLinkContent(link, arrayFields["Классификация"][i]);
      classification.push_back(link);
    }
  }
  
  ScTemplate result_struct;

  result_struct.Quintuple(
    system_idtf,
    ScType::EdgeDCommonVar, 
    ru_idtf,
    ScType::EdgeAccessVarPosPerm,
    CreatingInstanceByWikiKeynodes::nrel_main_idtf
  );

  result_struct.Triple(
    CreatingInstanceByWikiKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm, 
    ru_idtf
  );

  result_struct.Quintuple(
    system_idtf,
    ScType::EdgeDCommonVar, 
    note_node,
    ScType::EdgeAccessVarPosPerm, 
    CreatingInstanceByWikiKeynodes::nrel_note
  );

  result_struct.Triple(
    CreatingInstanceByWikiKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm, 
    note_node
  );

  
  for (int i = 0; i < decomposition.size(); i++){
    ScAddr const & decomp = m_context.GenerateNode(ScType::NodeConstClass);
    std::string name = "decomposition" + std::to_string(i);
    m_context.SetElementSystemIdentifier(name, decomp);
    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar, 
      decomp,
      ScType::EdgeAccessVarPosPerm, 
      CreatingInstanceByWikiKeynodes::nrel_subdividing
    );
    ScTemplate struct_decomp;
    struct_decomp.Quintuple(
      decomp,
      ScType::EdgeDCommonVar, 
      decomposition[i],
      ScType::EdgeAccessVarPosPerm,
      CreatingInstanceByWikiKeynodes::nrel_main_idtf
    );
    struct_decomp.Triple(
      CreatingInstanceByWikiKeynodes::lang_ru, 
      ScType::EdgeAccessVarPosPerm, 
      decomposition[i]
    );
    ScTemplateResultItem generation;
    m_context.GenerateByTemplate(struct_decomp, generation);
  }

  for (int i = 0; i < classification.size(); i++){
    ScAddr const & classif = m_context.GenerateNode(ScType::NodeConstClass);
    std::string name = "classification" + std::to_string(i);
    m_context.SetElementSystemIdentifier(name, classif);
    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar, 
      classif,
      ScType::EdgeAccessVarPosPerm, 
      CreatingInstanceByWikiKeynodes::nrel_subdividing
    );
    ScTemplate struct_class;
    struct_class.Quintuple(
      classif,
      ScType::EdgeDCommonVar, 
      classification[i],
      ScType::EdgeAccessVarPosPerm,
      CreatingInstanceByWikiKeynodes::nrel_main_idtf
    );
    struct_class.Triple(
      CreatingInstanceByWikiKeynodes::lang_ru, 
      ScType::EdgeAccessVarPosPerm, 
      classification[i]
    );
    ScTemplateResultItem generation_class;
    m_context.GenerateByTemplate(struct_class, generation_class);
  }

  ScTemplateResultItem genConstruction;
  m_context.GenerateByTemplate(result_struct, genConstruction);

  ScAddr const & answerStructure = m_context.SearchElementBySystemIdentifier("answer_structure");

  createMessageAndStructure("concept_message_about_successfulle_creating_instance_by_wiki", answerStructure);
 
  for (size_t i = 0; i < genConstruction.Size(); ++i)
    {
        try
        {
            m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, genConstruction[i]);
        }
        catch (utils::ScException & exception)
        {
            SC_AGENT_LOG_ERROR(exception.Description());
            SC_AGENT_LOG_DEBUG("do program error");
            return action.FinishUnsuccessfully();
        }
    }

  
  return action.FinishSuccessfully();
}



std::string CreateInstanceByWikiAgent::trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\n\r");
  size_t end = str.find_last_not_of(" \t\n\r");
  return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

void CreateInstanceByWikiAgent::createMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
{
  ScAddr const & phrase = m_context.SearchElementBySystemIdentifier(conceptName);
  ScTemplate searchPhraseConstruction;
  searchPhraseConstruction.Quintuple(
      CreatingInstanceByWikiKeynodes::concept_phrase,
      ScType::EdgeDCommonVar,
      phrase,
      ScType::EdgeAccessVarPosPerm,
      CreatingInstanceByWikiKeynodes::nrel_inclusion
  );
  searchPhraseConstruction.Triple(
      phrase,
      ScType::EdgeAccessVarPosPerm,
      ScType::LinkVar >> "_link"
  );
  searchPhraseConstruction.Triple(
      CreatingInstanceByWikiKeynodes::lang_ru,
      ScType::EdgeAccessVarPosPerm,
      "_link"
  );

  ScTemplateSearchResult phraseConstruction;
    m_context.SearchByTemplate(searchPhraseConstruction, phraseConstruction);
    std::string answer;
    m_context.GetLinkContent(phraseConstruction[0]["_link"], answer);
    createAnswer(answer);

    for (size_t i = 0; i < phraseConstruction[0].Size(); ++i)
    {
        try{
            m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, phraseConstruction[0][i]);
        }
        catch (utils::ScException & exception)
        {
            SC_AGENT_LOG_ERROR(exception.Description());
            SC_AGENT_LOG_DEBUG("create answer message error");
        }
    }
}

void CreateInstanceByWikiAgent::parseJSON(const std::string& json, std::map<std::string, std::string>& fields, std::map<std::string, std::vector<std::string>>& arrayFields) {
  size_t pos = 0;
  size_t length = json.size();

  while (pos < length) {
      // Пропускаем пробелы
      if (std::isspace(json[pos])) {
          ++pos;
          continue;
      }

      // Ищем ключ
      if (json[pos] == '"') {
          size_t keyStart = ++pos; // Пропускаем первую кавычку
          size_t keyEnd = json.find('"', keyStart);
          if (keyEnd == std::string::npos) {
              throw std::runtime_error("Ошибка парсинга: не закрыта кавычка в ключе.");
          }
          std::string key = json.substr(keyStart, keyEnd - keyStart);
          pos = keyEnd + 1;

          // Пропускаем пробелы и символ ':'
          while (pos < length && (std::isspace(json[pos]) || json[pos] == ':')) {
              ++pos;
          }

          // Проверяем значение
          if (json[pos] == '"') {
              // Значение - строка
              size_t valueStart = ++pos; // Пропускаем первую кавычку
              size_t valueEnd = json.find('"', valueStart);
              if (valueEnd == std::string::npos) {
                  throw std::runtime_error("Ошибка парсинга: не закрыта кавычка в значении.");
              }
              std::string value = json.substr(valueStart, valueEnd - valueStart);
              fields[key] = value;
              pos = valueEnd + 1;
          } else if (json[pos] == '[') {
              // Значение - массив
              ++pos; // Пропускаем '['
              std::vector<std::string> arrayValues;

              while (pos < length) {
                  // Пропускаем пробелы
                  while (std::isspace(json[pos])) {
                      ++pos;
                  }

                  if (json[pos] == '"') {
                      size_t valueStart = ++pos; // Пропускаем первую кавычку
                      size_t valueEnd = json.find('"', valueStart);
                      if (valueEnd == std::string::npos) {
                          throw std::runtime_error("Ошибка парсинга: не закрыта кавычка в массиве.");
                      }
                      std::string value = json.substr(valueStart, valueEnd - valueStart);
                      arrayValues.push_back(value);
                      pos = valueEnd + 1;
                  }

                  // Пропускаем пробелы и запятые
                  while (pos < length && (std::isspace(json[pos]) || json[pos] == ',')) {
                      ++pos;
                  }

                  if (json[pos] == ']') {
                      ++pos; // Пропускаем ']'
                      break;
                  }
              }

              arrayFields[key] = arrayValues;
          } else {
              throw std::runtime_error("Ошибка парсинга: неизвестный формат значения.");
          }
      } else {
          ++pos; // Пропускаем символы до начала ключа
      }
  }
}

void CreateInstanceByWikiAgent::printParsedJSON(const std::map<std::string, std::string>& fields, const std::map<std::string, std::vector<std::string>>& arrayFields) {
  for (const auto& field : fields) {
      SC_LOG_DEBUG(field.first + ": " + field.second);
  }

  for (const auto& arrayField : arrayFields) {
      SC_LOG_DEBUG(arrayField.first + ": ");
      for (const auto& value : arrayField.second) {
          SC_LOG_DEBUG(value);
      }
  }
}

ScAddr CreateInstanceByWikiAgent::GetActionClass() const
{
  return CreatingInstanceByWikiKeynodes::action_creating_new_instance;
}

void CreateInstanceByWikiAgent::createAnswer(std::string message) const
{
  dialogControlModule::MessageConstructionsGenerator messageConstructionGenerator = MessageConstructionsGenerator(&m_context);
  ScAddr const & author = m_context.SearchElementBySystemIdentifier("myself");
  ScAddr const & dialogue = m_context.SearchElementBySystemIdentifier("concept_dialogue");
  ScTemplate findTarg;
  findTarg.Triple(
      dialogue,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> "target"
  );
  ScTemplateSearchResult result;
  bool const isFoundByTemplate = m_context.SearchByTemplate(findTarg, result);
  ScAddr const & targAddr = result[0]["target"];
  
  ScAddr const &replyAddr = m_context.GenerateNode(ScType::NodeConst);
  try
  {
      m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, MessageKeynodes::concept_message, replyAddr);
      messageConstructionGenerator.generateTextTranslationConstruction(replyAddr, CreatingInstanceByWikiKeynodes::lang_ru, message);
      utils::GenerationUtils::generateRelationBetween(&m_context, replyAddr, author, CreatingInstanceByWikiKeynodes  ::nrel_authors);
      m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, targAddr, replyAddr);
  }
  catch (utils::ScException & exception)
      {
          SC_AGENT_LOG_ERROR(exception.Description());
          SC_AGENT_LOG_DEBUG("create answer error");
      }
}

std::string CreateInstanceByWikiAgent::getMessageText(ScAddr const & messageAddr) const
{
  ScAddr const & messageLink = messageSearcher->getMessageLink(messageAddr);
  std::string linkContent;
  if (!messageLink.IsValid())
  {
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Message link is not found.");
  }
  m_context.GetLinkContent(messageLink, linkContent);
  return linkContent;
}

}  // namespace creatingInstanceByWikiModule