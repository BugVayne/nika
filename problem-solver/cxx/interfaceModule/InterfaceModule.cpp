#include "InterfaceModule.hpp"

#include "agent/ChangeInterfaceColorAgent.hpp"
#include "agent/CreateAnswerTemplateAgent.hpp"
#include "agent/CreateClassAgent.hpp"
#include "agent/CreateClassInstanceAgent.hpp"
#include "agent/CreateRelationAgent.hpp"

using namespace interfaceModule;

SC_MODULE_REGISTER(InterfaceModule)->Agent<ChangeInterfaceColorAgent>()
                                    ->Agent<CreateClassAgent>()
                                    ->Agent<CreateAnswerTemplateAgent>()
                                    ->Agent<CreateClassInstanceAgent>()
                                    ->Agent<CreateRelationAgent>();
