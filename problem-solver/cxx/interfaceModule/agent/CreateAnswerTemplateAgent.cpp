//#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
//#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "keynodes/InterfaceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"

#include "CreateAnswerTemplateAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include <set>

using namespace interfaceModule;
//using namespace scAgentsCommon;
using namespace dialogControlModule;
using namespace commonModule;

ScResult CreateAnswerTemplateAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
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

    //user close
    if(formLinkContent == "0")
    {
        ScAddr const & phrase = m_context.SearchElementBySystemIdentifier("concept_phrase_about_user_close");
        ScTemplate searchPhraseConstruction;
        searchPhraseConstruction.Quintuple(
            InterfaceKeynodes::concept_phrase,
            ScType::EdgeDCommonVar >> "edge",
            phrase,
            ScType::EdgeAccessVarPosPerm >> "type",
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

        SC_AGENT_LOG_DEBUG("user close");
        return action.FinishSuccessfully();
    }

    //create nodes

    ScAddr const& rrel_phrase = m_context.SearchElementBySystemIdentifier("rrel_phrase");
    ScAddr const& rrel_phrase_ru = m_context.SearchElementBySystemIdentifier("rrel_phrase_ru");
    ScAddr const& rrel_answer = m_context.SearchElementBySystemIdentifier("rrel_answer");
    ScAddr const& rrel_answer_ru = m_context.SearchElementBySystemIdentifier("rrel_answer_ru");
    ScAddr const& rrel_wit = m_context.SearchElementBySystemIdentifier("rrel_wit");

    ScAddr const& _phrase = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, rrel_phrase);
    ScAddr const& _phrase_ru = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, rrel_phrase_ru);
    ScAddr const& _answer = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, rrel_answer);
    ScAddr const& _answer_ru = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, rrel_answer_ru);
    ScAddr const& _wit = utils::IteratorUtils::getAnyByOutRelation(&m_context, action, rrel_wit);

    std::string phrase_name = m_context.GetElementSystemIdentifier(_phrase);
    std::string phrase_ru;
    m_context.GetLinkContent(_phrase_ru, phrase_ru);
    std::string answer_name = m_context.GetElementSystemIdentifier(_answer);
    std::string answer_ru;
    m_context.GetLinkContent(_answer_ru, answer_ru);
    std::string wit;
    m_context.GetLinkContent(_wit, wit);


    if(!containsOnlyEnglish(phrase_name)){
        createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
        return action.FinishSuccessfully();
    }

    if(!containsOnlyEnglish(answer_name)){
        createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
        return action.FinishSuccessfully();
    }

    if(!containsOnlyEnglish(wit)){
        createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
        return action.FinishSuccessfully();
    }

    ScAddr const& rrel_answers = m_context.SearchElementBySystemIdentifier("rrel_answers");
    ScTemplate search_answers;

    search_answers.Quintuple(
        action, 
        ScType::EdgeAccessVarPosPerm,
        ScType::Unknown >> "a",
        ScType::EdgeAccessVarPosPerm,
        rrel_answers
    );

    ScTemplateSearchResult results;
    bool const success = m_context.SearchByTemplate(search_answers, results);
    std::set<std::string> answers;
    if(success){
        for(int i = 0; i < results.Size(); i++){
            std::string ans;
            m_context.GetLinkContent(results[i]["a"], ans);
            answers.insert(ans);
        }
    } 

    if(answers.size() == 1){
        for(std::string value : answers){
            if(value == "none"){
            createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
            
            SC_AGENT_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
            return action.FinishSuccessfully();
            }
        }
    }

    ScAddr new_concept;
    if(m_context.SearchElementBySystemIdentifier(phrase_name, new_concept))
    { 
        if(m_context.GetElementType(new_concept) != ScType::NodeVar){
        createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
        return action.FinishSuccessfully();
        }
    }
    ScAddr new_concept1;
    if(m_context.SearchElementBySystemIdentifier(answer_name, new_concept1))
    { 
        if(m_context.GetElementType(new_concept1) != ScType::NodeVar){
        createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
        return action.FinishSuccessfully();
        }
    }
    ScAddr new_concept2;
    if(m_context.SearchElementBySystemIdentifier(wit, new_concept2))
    { 
        if(m_context.GetElementType(new_concept1) != ScType::NodeVar){
        createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
        
        SC_AGENT_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
        return action.FinishSuccessfully();
        }
    }

    //create result
    ScAddr const& classNode = m_context.GenerateNode(ScType::NodeConstClass);
    m_context.SetElementSystemIdentifier(phrase_name, classNode);

    ScAddr const& classMainIdtf = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(classMainIdtf, phrase_ru);

    ScAddr const& witIntent = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(witIntent, wit);

    ScTemplate messageClassConstruction;

    messageClassConstruction.Triple(
        InterfaceKeynodes::concept_intent_possible_class,
        ScType::EdgeAccessVarPosPerm,
        classNode
    );

    messageClassConstruction.Quintuple(
        classNode,
        ScType::EdgeDCommonVar,
        classMainIdtf,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_main_idtf
    );

    messageClassConstruction.Triple(
        InterfaceKeynodes::lang_ru,
        ScType::EdgeAccessVarPosPerm,
        classMainIdtf
    );

    messageClassConstruction.Quintuple(
        classNode,
        ScType::EdgeDCommonVar,
        witIntent,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_wit_ai_idtf
    );

    messageClassConstruction.Triple(
        InterfaceKeynodes::lang_ru,
        ScType::EdgeAccessVarPosPerm,
        witIntent
    );

    ScTemplateResultItem classConstruction;
    m_context.GenerateByTemplate(messageClassConstruction, classConstruction);

    //create phrases

    ScAddr const& phraseTemplateNode = m_context.GenerateNode(ScType::NodeConstClass);
    m_context.SetElementSystemIdentifier(answer_name, phraseTemplateNode);

    ScAddr const& phraseTemplateMainIdtf = m_context.GenerateLink(ScType::LinkConst);
    m_context.SetLinkContent(phraseTemplateMainIdtf, answer_ru);

    ScTemplate phraseTemplate;

    phraseTemplate.Quintuple(
        InterfaceKeynodes::concept_phrase,
        ScType::EdgeDCommonVar,
        phraseTemplateNode,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_inclusion
    );

    phraseTemplate.Quintuple(
        phraseTemplateNode,
        ScType::EdgeDCommonVar,
        phraseTemplateMainIdtf,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_main_idtf
    );

    phraseTemplate.Triple(
        InterfaceKeynodes::lang_ru,
        ScType::EdgeAccessVarPosPerm,
        phraseTemplateMainIdtf
    );

    for(std::string phrase : answers){
        ScAddr const& phraseLink = m_context.GenerateLink(ScType::LinkConst);
        m_context.SetLinkContent(phraseLink, phrase);

        phraseTemplate.Triple(
            phraseTemplateNode,
            ScType::EdgeAccessVarPosPerm,
            phraseLink
        );

        phraseTemplate.Triple(
            InterfaceKeynodes::lang_ru,
            ScType::EdgeAccessVarPosPerm,
            phraseLink
        );
    }

    ScTemplateResultItem genPhrase;
    m_context.GenerateByTemplate(phraseTemplate, genPhrase);

    //create logic tule

    //first

    ScTemplate logicRule;
    ScAddr const & edgeFromConceptMessageToMessageVar = m_context.GenerateConnector(ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::concept_message, InterfaceKeynodes::_message);

    ScAddr const & edgeFromConceptMessageClassToMessageVar = m_context.GenerateConnector(ScType::EdgeAccessVarPosPerm,
        classNode, InterfaceKeynodes::_message);

    ScAddr const & structUp = m_context.GenerateNode(ScType::NodeConstStruct);

    logicRule.Triple(
        structUp,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::concept_message
    );
    
    logicRule.Triple(
        structUp,
        ScType::EdgeAccessVarPosPerm,
        edgeFromConceptMessageToMessageVar
    );
    
    logicRule.Triple(
        structUp,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::_message
    );
    
    logicRule.Triple(
        structUp,
        ScType::EdgeAccessVarPosPerm,
        classNode
    );
    
    logicRule.Triple(
        structUp,
        ScType::EdgeAccessVarPosPerm,
        edgeFromConceptMessageClassToMessageVar
    );
    
    //second
    ScAddr const & edgeFromConceptMessageClassToReplyMessage = m_context.GenerateConnector(ScType::EdgeAccessVarPosPerm,
        classNode, InterfaceKeynodes::_reply_message);

    ScAddr const & edgeFromConceptAtomicMessageToReplyMessage = m_context.GenerateConnector(ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::concept_atomic_message, InterfaceKeynodes::_reply_message);

    ScAddr const & edgeFromMessageToReplyMessage = m_context.GenerateConnector(ScType::EdgeDCommonVar,
        InterfaceKeynodes::_message, InterfaceKeynodes::_reply_message);

    ScAddr const & edgeFromNrelReplyToDCommonEdge = m_context.GenerateConnector(ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_reply, edgeFromMessageToReplyMessage);

    ScAddr const & structDown = m_context.GenerateNode(ScType::NodeConstStruct);
    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        classNode
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        edgeFromConceptMessageClassToReplyMessage
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::_reply_message
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::concept_atomic_message
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        edgeFromConceptAtomicMessageToReplyMessage
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::_message
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_reply
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        edgeFromMessageToReplyMessage
    );

    logicRule.Triple(
        structDown,
        ScType::EdgeAccessVarPosPerm,
        edgeFromNrelReplyToDCommonEdge
    );

    //create rule
    logicRule.Triple(
        InterfaceKeynodes::atomic_logical_formula,
        ScType::EdgeAccessVarPosPerm,
        structUp
    );
    
    logicRule.Triple(
        InterfaceKeynodes::atomic_logical_formula,
        ScType::EdgeAccessVarPosPerm,
        structDown
    );
    
    logicRule.Quintuple(
        structUp,
        ScType::EdgeDCommonVar >> "key_sc_element",
        structDown,
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_implication
    );
    ScAddr const & ruleNode = m_context.GenerateNode(ScType::NodeConst);

    std::string replace = "concept";
    size_t pos = phrase_name.find(replace);
        if (pos != std::string::npos) {
            phrase_name.replace(pos, replace.length(), "lr");
    }

    m_context.SetElementSystemIdentifier(phrase_name, ruleNode);

    logicRule.Quintuple(
        ruleNode,
        ScType::EdgeAccessVarPosPerm,
        "key_sc_element",
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::rrel_main_key_sc_element
    );
    
    logicRule.Triple(
        InterfaceKeynodes::concept_answer_on_standard_message_rule_priority_1,
        ScType::EdgeAccessVarPosPerm,
        ruleNode
    );
    
    logicRule.Quintuple(
        ruleNode,
        ScType::EdgeDCommonVar,
        ScType::NodeVarTuple >> "tuple",
        ScType::EdgeAccessVarPosPerm,
        InterfaceKeynodes::nrel_answer_pattern
    );
    
    logicRule.Quintuple(
        "tuple",
        ScType::EdgeAccessVarPosPerm,
        phraseTemplateNode,
        ScType::EdgeAccessVarPosPerm,
        ScKeynodes::rrel_1
    );

    ScTemplateResultItem full_construction;
    m_context.GenerateByTemplate(logicRule, full_construction);


    createAnswerMessageAndStructure("concept_phrase_about_successful_creating", answerStructure);

    for (size_t i = 0; i < genPhrase.Size(); ++i)
    {
        m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, genPhrase[i]);
    }

    for (size_t i = 0; i < classConstruction.Size(); ++i)
    {
        if (std::find(genPhrase.begin(), genPhrase.end(), classConstruction[i]) == genPhrase.end())
        {
            m_context.GenerateConnector(ScType::EdgeAccessConstPosPerm, answerStructure, classConstruction[i]);
        }
    }

    SC_AGENT_LOG_DEBUG("CreateAnswerTemplateAgent finished");
    return action.FinishSuccessfully();
}

ScAddr CreateAnswerTemplateAgent::GetActionClass() const
{
  return InterfaceKeynodes::action_create_message_class_and_phrase_template;
}

void CreateAnswerTemplateAgent::createAnswer(std::string message){
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

void CreateAnswerTemplateAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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

bool CreateAnswerTemplateAgent::containsOnlyEnglish(std::string name){
    for (char ch : name) {
        if (!std::isalpha(ch) && ch != '_') {
            return false;
        }
    }
    return true; 
}