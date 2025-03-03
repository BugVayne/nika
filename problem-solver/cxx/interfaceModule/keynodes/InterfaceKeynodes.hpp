#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace interfaceModule
{
class InterfaceKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_change_interface{"action_change_interface", ScType::NodeConstClass};

  static inline ScKeynode const concept_header{"concept_header", ScType::NodeConstClass};

  static inline ScKeynode const concept_main_part{"concept_main_part", ScType::NodeConstClass};

  static inline ScKeynode const concept_footer{"concept_footer", ScType::NodeConstClass};

  static inline ScKeynode const nrel_component_color{"nrel_component_color", ScType::NodeConstNoRole};

  static inline ScKeynode const rrel_entity{"rrel_entity", ScType::NodeConstRole};

  static inline ScKeynode const rrel_color{"rrel_color", ScType::NodeConstRole};

  static inline ScKeynode const action_create_message_class_and_phrase_template{"action_create_message_class_and_phrase_template", ScType::NodeConstClass};

  static inline ScKeynode const action_create_class{"action_create_class", ScType::NodeConstClass};

  static inline ScKeynode const action_create_class_instance{"action_create_class_instance", ScType::NodeConstClass};

  static inline ScKeynode const action_create_relation{"action_create_relation", ScType::NodeConstClass};

  static inline ScKeynode const concept_phrase{"concept_phrase", ScType::NodeConstClass};

  static inline ScKeynode const concept_intent_possible_class{"concept_intent_possible_class", ScType::NodeConstClass};

  static inline ScKeynode const concept_message{"concept_message", ScType::NodeConstClass};

  static inline ScKeynode const concept_answer_on_standard_message_rule_priority_1{"concept_answer_on_standard_message_rule_priority_1", ScType::NodeConstClass};

  static inline ScKeynode const concept_atomic_message{"concept_atomic_message", ScType::NodeConstClass};

  static inline ScKeynode const concept_wit_entity{"concept_wit_entity", ScType::NodeConstClass};

  static inline ScKeynode const nrel_inclusion{"nrel_inclusion", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_main_idtf{"nrel_main_idtf", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_answer{"nrel_answer", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_wit_ai_idtf{"nrel_wit_ai_idtf", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_reply{"nrel_reply", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_implication{"nrel_implication", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_answer_pattern{"nrel_answer_pattern", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_authors{"nrel_authors", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_note{"nrel_note", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_subdividing{"nrel_subdividing", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_definition{"nrel_definition", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_definitional_domain{"nrel_definitional_domain", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_combination{"nrel_combination", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_first_domain{"nrel_first_domain", ScType::NodeConstNoRole};

  static inline ScKeynode const nrel_second_domain{"nrel_second_domain", ScType::NodeConstNoRole};

  static inline ScKeynode const rrel_main_key_sc_element{"rrel_main_key_sc_element", ScType::NodeConstRole};

  static inline ScKeynode const lang_ru{"lang_ru", ScType::NodeConstClass};

  static inline ScKeynode const lang_en{"lang_en", ScType::NodeConstClass};

  static inline ScKeynode const atomic_logical_formula{"atomic_logical_formula", ScType::NodeConstClass};

  static inline ScKeynode const _message{"_message", ScType::Var};

  static inline ScKeynode const _reply_message{"_reply_message", ScType::Var};

  static inline ScKeynode const sc_node_norole_relation{"lang_en", ScType::Const};

  static inline ScKeynode const relation{"relation", ScType::NodeConstClass};
};

}  // namespace interfaceModule
