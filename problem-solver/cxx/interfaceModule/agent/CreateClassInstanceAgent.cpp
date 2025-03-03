#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "keynodes/InterfaceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"

#include "CreateClassInstanceAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include <set>
#include <utility>
#include <algorithm>

using namespace interfaceModule;
using namespace dialogControlModule;
using namespace commonModule;

ScResult CreateClassInstanceAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
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
  bool const is_success = m_context.SearchByTemplate(findAnswerStructure, resultAnswerStructure);
  
  if (is_success)
  {
      for (size_t i = 0; i < resultAnswerStructure.Size(); i++)
      {
          m_context.EraseElement(resultAnswerStructure[i]["y"]);
      }   
  }

  ScAddr const & formLinkAddr = action.GetArgument(ScKeynodes::rrel_2);

  std::string formLinkContent;
  m_context.GetLinkContent(formLinkAddr, formLinkContent);

  m_context.EraseElement(formLinkAddr);

  ScAddr const & _rrel_system_idtf = m_context.SearchElementBySystemIdentifier("rrel_system_idtf");
  ScAddr const & _rrel_ru = m_context.SearchElementBySystemIdentifier("rrel_ru");
  ScAddr const & _rrel_note = m_context.SearchElementBySystemIdentifier("rrel_note");
  ScAddr const & _rrel_clas = m_context.SearchElementBySystemIdentifier("rrel_class");
  
  ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_system_idtf);
  ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_ru);
  ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_note);
  ScAddr const & _clas = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_clas);

  std::string idtf, ru_idtf, note, clas;

  idtf = m_context.GetElementSystemIdentifier(_system_idtf);
  m_context.GetLinkContent(_system_ru_idtf, ru_idtf);
  m_context.GetLinkContent(_note, note);
  clas = m_context.GetElementSystemIdentifier(_clas);

  //relation list 
  ScAddr const & _rrel_relation= m_context.SearchElementBySystemIdentifier("rrel_relation");

	ScTemplate search_relation;
	
	search_relation.Quintuple(
    action,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "x",
    ScType::EdgeAccessVarPosPerm,
    _rrel_relation
  );

  ScTemplateSearchResult result_relation;
  bool _is_success = m_context.SearchByTemplate(search_relation, result_relation);
	
  std::vector<std::pair<std::string, std::string>> relation_list;

  if(_is_success)
	{
		for (size_t i = 0; i < result_relation.Size(); ++i)
		{
      ScAddr const & _rrel_concept_relation= m_context.SearchElementBySystemIdentifier("rrel_concept_relation");
      ScAddr const & _rrel_nrel_relation= m_context.SearchElementBySystemIdentifier("rrel_nrel_relation");


      ScTemplate search_concept_and_nrel_relation;
	
      search_concept_and_nrel_relation.Quintuple(
        result_relation[i]["x"],
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "c",
        ScType::EdgeAccessVarPosPerm,
        _rrel_concept_relation
      );

      search_concept_and_nrel_relation.Quintuple(
        result_relation[i]["x"],
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "n",
        ScType::EdgeAccessVarPosPerm,
        _rrel_nrel_relation
      );


      ScTemplateSearchResult result_concept_and_nrel_relation;
      bool _is_success_find = m_context.SearchByTemplate(search_concept_and_nrel_relation, result_concept_and_nrel_relation);


      if(_is_success_find)
      {
        for (size_t j = 0; j < result_concept_and_nrel_relation.Size(); j++)
        {
          std::string rel = m_context.GetElementSystemIdentifier(result_concept_and_nrel_relation[j]["n"]);
          std::string conc = m_context.GetElementSystemIdentifier(result_concept_and_nrel_relation[j]["c"]);

          relation_list.push_back(make_pair(rel, conc));
        }
          
      }
		}
	}

  // if idtf contains non-english symbols
  if(!containsOnlyEnglish(idtf)){
    createAnswerMessageAndStructure("concept_message_about_error_in_system_idtf_non_latin_symbol", answerStructure);
    SC_AGENT_LOG_DEBUG("CreateClassInstanceAgent finished with error: invalid idtf inputs");
    return action.FinishSuccessfully();
  }

  // if class contains non-english symbols
  if(!containsOnlyEnglish(clas)){
    createAnswerMessageAndStructure("concept_message_about_error_in_class_non_latin_symbol", answerStructure);
    SC_AGENT_LOG_DEBUG("CreateClassInstanceAgent finished with error: invalid class inputs");
    return action.FinishSuccessfully();
  }

  //if idtf already exists
  ScAddr new_concept;
  if(m_context.SearchElementBySystemIdentifier(idtf, new_concept)){
		if((m_context.GetElementType(new_concept) != ScType::NodeVar))
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_system_idtf_already_exists", answerStructure);
			SC_AGENT_LOG_DEBUG("CreateClassInstanceAgent finished with error: idtf node already exists");
			return action.FinishSuccessfully();
  		}
	}

  // class should already exist
  if(m_context.SearchElementBySystemIdentifier(clas, new_concept)){
		if(m_context.GetElementType(new_concept) == ScType::NodeVar)
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_class_idtf_does_not_exist", answerStructure);
			SC_AGENT_LOG_DEBUG("CreateRelationAgent finished with error: class idtf does not exist");
			return action.FinishSuccessfully();
  		}
	}

  for(const auto& element : relation_list)
  {
    if(clas == element.second)
    {
      createAnswerMessageAndStructure("concept_message_about_error_relation_second_domain_is_class", answerStructure);
			SC_AGENT_LOG_DEBUG("CreateRelationAgent finished with error: added relation can not connect instance and its class ");
			return action.FinishSuccessfully();
    }
  }

  //creating structure

  ScTemplate result_struct;

  ScAddr const & system_idtf = m_context.GenerateNode(ScType::NodeConst);
  m_context.SetElementSystemIdentifier(idtf, system_idtf);

  ScAddr const& ru_idtf_ = m_context.GenerateLink(ScType::LinkConst);
  m_context.SetLinkContent(ru_idtf_, ru_idtf);

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

  ScAddr const & clas_ = m_context.SearchElementBySystemIdentifier(clas);

  for (const auto& p : relation_list)
  {
    ScAddr rel_addr = m_context.SearchElementBySystemIdentifier(p.first);
    ScAddr class_addr =m_context.SearchElementBySystemIdentifier(p.second);
    

    result_struct.Quintuple(
      system_idtf,
      ScType::EdgeDCommonVar,
      class_addr,
      ScType::EdgeAccessVarPosPerm,
      rel_addr
    );
  }

  result_struct.Quintuple(
    system_idtf,
    ScType::EdgeDCommonVar,
    ru_idtf_,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  result_struct.Triple(
    clas_, 
    ScType::EdgeAccessVarPosPerm,
    system_idtf
  );

  result_struct.Triple(
    InterfaceKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm,
    ru_idtf_
  );

  ScTemplateResultItem genInstanceConstruction;
  m_context.GenerateByTemplate(result_struct, genInstanceConstruction);

  createAnswerMessageAndStructure("concept_phrase_about_success_creating_class_instance", answerStructure);

  for (size_t i = 0; i < genInstanceConstruction.Size(); ++i)
  {
    m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, genInstanceConstruction[i]);
  }

  SC_AGENT_LOG_DEBUG("CreateClassInstanceAgent finished ");
  return action.FinishSuccessfully();
}

ScAddr CreateClassInstanceAgent::GetActionClass() const
{
  return InterfaceKeynodes::action_create_class_instance;
}

void CreateClassInstanceAgent::createAnswer(std::string message){
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

void CreateClassInstanceAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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

bool CreateClassInstanceAgent::containsOnlyEnglish(std::string name){
  for (char ch : name) {
      if (!std::isalpha(ch) && ch != '_') {
          return false;
      }
  }
  return true; 
}