#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "keynodes/InterfaceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"

#include "CreateClassAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include <set>

using namespace interfaceModule;
using namespace dialogControlModule;
using namespace commonModule;

ScResult CreateClassAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
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

    //create result

    ScAddr const & _rrel_idtf = m_context.SearchElementBySystemIdentifier("rrel_system_idtf");
    ScAddr const & _rrel_ru = m_context.SearchElementBySystemIdentifier("rrel_ru");
    ScAddr const & _rrel_note = m_context.SearchElementBySystemIdentifier("rrel_note");
    ScAddr const & _rrel_super_class = m_context.SearchElementBySystemIdentifier("rrel_super_class");
    ScAddr const & _rrel_decomposition= m_context.SearchElementBySystemIdentifier("rrel_decomposition");

    //find nodes
    ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_idtf);
    ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_ru);
    ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_note);
    ScAddr const & _super_class = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, _rrel_super_class);

    //check if nodes exist

    std::string name;
    name = m_context.GetElementSystemIdentifier(_system_idtf);

    std::string ru;
    m_context.GetLinkContent(_system_ru_idtf, ru);

    std::string note;
    m_context.GetLinkContent(_note, note);
    
    bool s_class = true;
    if(m_context.GetElementSystemIdentifier(_super_class) == "concept_"){
        s_class = false;
    }
    
    if(!containsOnlyEnglish(name)){
        createAnswerMessageAndStructure("concept_errors_in_names", answerStructure);
        
        SC_LOG_DEBUG("finished with error: nodes exists");
        return action.FinishSuccessfully();
    }
    ScAddr new_concept;
    if(m_context.SearchElementBySystemIdentifier(name, new_concept))
    { 
        if(m_context.GetElementType(new_concept) != ScType::NodeVar){
            createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
            
            SC_LOG_DEBUG("finished with error: nodes exists");
            return action.FinishSuccessfully();
        }
    }
    //decomposition
    ScAddr const & rrel_decomposition = m_context.SearchElementBySystemIdentifier("rrel_decomposition");

    ScTemplate search_d;

    search_d.Quintuple(
        action,
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "d",
        ScType::EdgeAccessVarPosPerm,
        rrel_decomposition
    );

    ScTemplateSearchResult result_d;
    bool const is_success_d = m_context.SearchByTemplate(search_d, result_d);
    std::set<std::string>decomposition;
  
    if (is_success_d)
    {
      for (size_t i = 0; i < result_d.Size(); ++i)
      {
        std::string decomp = m_context.GetElementSystemIdentifier(result_d[i]["d"]);
        decomposition.insert(decomp);
      }
    }

    //create class

    ScTemplate result_struct;


    ScAddr const & system_idtf = m_context.GenerateNode(ScType::NodeConstClass);
    m_context.SetElementSystemIdentifier(name, system_idtf);
  
    ScAddr const& ru_idtf = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(ru_idtf, ru);
  
    ScAddr const& note_node = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(note_node, note);

    std::vector<ScAddr> decomposition_addr;
    for(std::string value : decomposition){
        ScAddr const& addr = m_context.SearchElementBySystemIdentifier(value);
        decomposition_addr.push_back(addr);
    }



    result_struct.Quintuple(
        system_idtf,
        ScType::EdgeDCommonVar,
        ru_idtf,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_main_idtf
    );
    result_struct.Triple(
        InterfaceKeynodes::lang_ru, 
        ScType::EdgeAccessVarPosPerm,
        ru_idtf
    );
    
    result_struct.Quintuple(
        system_idtf,
        ScType::EdgeDCommonVar,
        note_node,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_note
    );
    
    result_struct.Triple(
        InterfaceKeynodes::lang_ru, 
        ScType::EdgeAccessVarPosPerm,
        note_node
    );

    if(s_class){
        std::string name_super_class = m_context.GetElementSystemIdentifier(_super_class);
        ScAddr const& super_class = m_context.SearchElementBySystemIdentifier(name_super_class);
    
        result_struct.Quintuple(
            super_class,
            ScType::EdgeDCommonVar,
            system_idtf,
            ScType::EdgeAccessVarPosPerm,
            InterfaceKeynodes::nrel_inclusion
        );
    }

    for(int i = 0; i < decomposition_addr.size(); i++){
        ScAddr dec = m_context.SearchElementBySystemIdentifier(m_context.GetElementSystemIdentifier(decomposition_addr[i]));
        result_struct.Quintuple(
            system_idtf,
            ScType::EdgeDCommonVar,
            dec,
            ScType::EdgeAccessVarPosPerm,
            InterfaceKeynodes::nrel_subdividing
        );
    }
    m_context.EraseElement(_system_idtf);
    m_context.EraseElement(_system_ru_idtf);
    m_context.EraseElement(_note);
    m_context.EraseElement(_super_class);

    ScTemplateResultItem genClassConstruction;
    m_context.GenerateByTemplate(result_struct, genClassConstruction);

    createAnswerMessageAndStructure("concept_phrase_about_successful_creating_class", answerStructure);

    for (size_t i = 0; i < genClassConstruction.Size(); ++i)
    {
        try
        {
            m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, genClassConstruction[i]);
        }
        catch (utils::ScException & exception)
        {
            SC_AGENT_LOG_ERROR(exception.Description());
            SC_AGENT_LOG_DEBUG("do program error");
            return action.FinishUnsuccessfully();
        }
    }
  
    SC_AGENT_LOG_DEBUG("CreateClassAgent finished");
    return action.FinishSuccessfully();
}

ScAddr CreateClassAgent::GetActionClass() const
{
  return InterfaceKeynodes::action_create_class;
}

void CreateClassAgent::createAnswer(std::string message){
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
        messageConstructionGenerator.generateTextTranslationConstruction(replyAddr, InterfaceKeynodes::lang_ru, message);
        utils::GenerationUtils::generateRelationBetween(&m_context, replyAddr, author, InterfaceKeynodes::nrel_authors);
        m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, targAddr, replyAddr);
    }
    catch (utils::ScException & exception)
        {
            SC_AGENT_LOG_ERROR(exception.Description());
            SC_AGENT_LOG_DEBUG("create answer error");
        }

}

void CreateClassAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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


bool CreateClassAgent::containsOnlyEnglish(std::string name){
    for (char ch : name) {
        if (!std::isalpha(ch) && ch != '_') {
            return false;
        }
    }
    return true; 
}