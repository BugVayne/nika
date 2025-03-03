#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "keynodes/InterfaceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"

#include "CreateRelationAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include <set>

using namespace interfaceModule;
using namespace dialogControlModule;
using namespace commonModule;

ScResult CreateRelationAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
    ScAddr const & messageAddr = action.GetArgument(ScKeynodes::rrel_1);
    if (!messageAddr.IsValid()){
        SC_AGENT_LOG_ERROR("The message isn’t valid");
        return action.FinishUnsuccessfully();
    }

    
    ScAddr const & answerStructure = m_context.SearchElementBySystemIdentifier("answer_structure");
    
    ScTemplate findAnswerStructure;

    findAnswerStructure.Triple(
      answerStructure,
      ScType::EdgeAccessVarPosPerm >> "y",
      ScType::Unknown
    );

    ScTemplateSearchResult resultAnswerStructure;
    bool const& is_success = m_context.SearchByTemplate(findAnswerStructure, resultAnswerStructure);

    if (is_success)
    {
        for (size_t i = 0; i < resultAnswerStructure.Size(); i++)
        {
            m_context.EraseElement(resultAnswerStructure[i]["y"]);
        }   
    }
    
    ScAddr const& formLinkAddr = action.GetArgument(ScKeynodes::rrel_2);

    std::string formLinkContent;
    m_context.GetLinkContent(formLinkAddr, formLinkContent);
  
    m_context.EraseElement(formLinkAddr);

    //rrel nodes
    ScAddr const & _rrel_system_idtf = m_context.SearchElementBySystemIdentifier("rrel_system_idtf");
    ScAddr const & _rrel_ru = m_context.SearchElementBySystemIdentifier("rrel_ru");
    ScAddr const & _rrel_note = m_context.SearchElementBySystemIdentifier("rrel_note");
    ScAddr const & _rrel_first_domain = m_context.SearchElementBySystemIdentifier("rrel_first_domain");
    ScAddr const & _rrel_second_domain= m_context.SearchElementBySystemIdentifier("rrel_second_domain");
  
    ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_system_idtf);
    ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_ru);
    ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_note);
    ScAddr const & _first_domain = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_first_domain);
    ScAddr const & _second_domain = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_second_domain);

    std::string idtf, ru_idtf, note, first_domain, second_domain;

    idtf = m_context.GetElementSystemIdentifier(_system_idtf);
    m_context.GetLinkContent(_system_ru_idtf, ru_idtf);
    m_context.GetLinkContent(_note, note);
    first_domain = m_context.GetElementSystemIdentifier(_first_domain);
    second_domain = m_context.GetElementSystemIdentifier(_second_domain);

    if(!containsOnlyEnglish(idtf)){
      createAnswerMessageAndStructure("concept_message_about_error_in_system_idtf_non_latin_symbol", answerStructure);
      SC_AGENT_LOG_DEBUG("CreateRelationAgent finished with error: invalid idtf input");
      return action.FinishSuccessfully();
    }
    

    ScAddr new_concept;
    if(m_context.SearchElementBySystemIdentifier(idtf, new_concept)){
      if((m_context.GetElementType(new_concept) != ScType::NodeVar))
        { 
          createAnswerMessageAndStructure("concept_message_about_error_system_idtf_already_exists", answerStructure);
          SC_AGENT_LOG_DEBUG("CreateRelationAgent finished with error: idtf node already exists");
          return action.FinishSuccessfully();
        }
    }

    if(m_context.SearchElementBySystemIdentifier(first_domain, new_concept)){
      if(m_context.GetElementType(new_concept) == ScType::NodeVar)
        { 
          createAnswerMessageAndStructure("concept_message_about_error_first_domain_does_not_exist", answerStructure);
          SC_LOG_DEBUG("CreateRelationAgent finished with error: first domain does not exist");
          return action.FinishSuccessfully();
        }
    }

    if(m_context.SearchElementBySystemIdentifier(second_domain, new_concept)){
      if(m_context.GetElementType(new_concept) == ScType::NodeVar)
        { 
          createAnswerMessageAndStructure("concept_message_about_error_second_domain_does_not_exist", answerStructure);
          SC_LOG_DEBUG("CreateRelationAgent finished with error: second domain does not exist");
          return action.FinishSuccessfully();
        }
    }

    //relation classification list
    ScAddr const & _rrel_include= m_context.SearchElementBySystemIdentifier("rrel_include");

	  ScTemplate search_include;
    search_include.Quintuple(
      action,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> "x",
      ScType::EdgeAccessVarPosPerm,
      _rrel_include
    );

    ScTemplateSearchResult result_include;
    bool const& _is_success = m_context.SearchByTemplate(search_include, result_include);
    std::vector<std::string>classification;
    if(_is_success)
    {
      for (size_t i = 0; i < result_include.Size(); ++i)
      {
        std::string clas = m_context.GetElementSystemIdentifier(result_include[i]["x"]);
        classification.push_back(clas);
      }
    }

    //class creating
    ScTemplate result_struct;

    ScAddr const & system_idtf = m_context.GenerateNode(ScType::NodeConstNoRole);
    m_context.SetElementSystemIdentifier(idtf, system_idtf);

    ScAddr const& ru_idtf_ = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(ru_idtf_, ru_idtf);


    ScAddr const& first_domain_ =  m_context.SearchElementBySystemIdentifier(first_domain);
    ScAddr const& second_domain_ =  m_context.SearchElementBySystemIdentifier(second_domain);

    std::vector<ScAddr> classification_addr;
    for(std::string value : classification){
      ScAddr const& addr = m_context.SearchElementBySystemIdentifier(value);
      classification_addr.push_back(addr);
    }

    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar,
      ru_idtf_,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_main_idtf
    );

    result_struct.Triple(
      InterfaceKeynodes::lang_ru, 
      ScType::EdgeAccessVarPosPerm,
      ru_idtf_
    );

    if(note!="error_no_data")
    {
      ScAddr const& note_node = m_context.GenerateLink(ScType::LinkConst);
      m_context.SetLinkContent(note_node, note);
      result_struct.Quintuple(
        system_idtf,
        ScType::EdgeDCommonVar,
        note_node,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_note
      );
    }

    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar,
      first_domain_,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_first_domain
    );

    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar,
      second_domain_,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_second_domain
    );

    for(const auto& element : classification_addr){
      ScAddr dec = m_context.SearchElementBySystemIdentifier(m_context.GetElementSystemIdentifier(element));
      result_struct.Triple(
        dec, 
        ScType::EdgeAccessVarPosPerm,
        system_idtf
      );
    }

    ScTemplateResultItem genRelationConstruction;
    m_context.GenerateByTemplate(result_struct, genRelationConstruction);


    createAnswerMessageAndStructure("concept_phrase_about_successful_creating_relation", answerStructure);

    for (size_t i = 0; i < genRelationConstruction.Size(); ++i)
    {
        m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, genRelationConstruction[i]);
    }
    SC_AGENT_LOG_DEBUG("CreateRelationAgent finished ");
    return action.FinishSuccessfully();
}

ScAddr CreateRelationAgent::GetActionClass() const
{
  return InterfaceKeynodes::action_create_relation;
}

void CreateRelationAgent::createAnswer(std::string message)
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
  m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, MessageKeynodes::concept_message, replyAddr);
  messageConstructionGenerator.generateTextTranslationConstruction(replyAddr, InterfaceKeynodes::lang_ru, message);
  utils::GenerationUtils::generateRelationBetween(&m_context, replyAddr, author, InterfaceKeynodes::nrel_authors);
  m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, targAddr, replyAddr);
}

void CreateRelationAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
{
    ScAddr const & phrase = m_context.SearchElementBySystemIdentifier(conceptName);
    ScTemplate searchPhraseConstruction;
    searchPhraseConstruction.Quintuple(
        InterfaceKeynodes::concept_phrase,
        ScType::EdgeDCommonVar,
        phrase,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_inclusion
    );
    searchPhraseConstruction.Triple(
        phrase,
        ScType::EdgeAccessVarPosPerm,
        ScType::LinkVar >> "_link"
    );
    searchPhraseConstruction.Triple(
        InterfaceKeynodes::lang_ru,
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
        m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, phraseConstruction[0][i]);
    }
}

bool CreateRelationAgent::containsOnlyEnglish(std::string name){
  for (char ch : name) {
      if (!std::isalpha(ch) && ch != '_') {
          return false;
      }
  }
  return true; 
}
