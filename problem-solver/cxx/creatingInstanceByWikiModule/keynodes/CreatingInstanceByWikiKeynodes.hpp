#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace creatingInstanceByWikiModule
{
class CreatingInstanceByWikiKeynodes : public ScKeynodes
{
public:
    static inline ScKeynode const action_creating_new_instance{
      "action_creating_new_instance",
      ScType::NodeConstClass};
  
    static inline ScKeynode const new_instance_by_wiki{
        "new_instance_by_wiki",
        ScType::NodeConstClass};
  
        static inline ScKeynode const concept_creating_new_instance{
            "concept_creating_new_instance",
            ScType::NodeConstClass};
    static inline ScKeynode const rrel_entity{
      "rrel_entity"
    };
    static inline ScKeynode const lang_en{
      "lang_en"
    };

    static inline ScKeynode const lang_ru{"lang_ru", ScType::NodeConstClass};

    static inline ScKeynode const nrel_main_idtf{"nrel_main_idtf", ScType::NodeConstNoRole};

    static inline ScKeynode const nrel_note{"nrel_note", ScType::NodeConstNoRole};

    static inline ScKeynode const nrel_subdividing{"nrel_subdividing", ScType::NodeConstNoRole};

    static inline ScKeynode const concept_phrase{"concept_phrase", ScType::NodeConstClass};

    static inline ScKeynode const nrel_authors{"nrel_authors", ScType::NodeConstNoRole};

    static inline ScKeynode const concept_message{"concept_message", ScType::NodeConstClass};
};

}  // namespace creatingInstanceByWikiModule