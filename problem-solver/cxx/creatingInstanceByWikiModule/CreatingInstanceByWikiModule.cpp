#include "CreatingInstanceByWikiModule.hpp"

#include "agent/CreateInstanceByWikiAgent.hpp"

namespace creatingInstanceByWikiModule
{
SC_MODULE_REGISTER(CreatingInstanceByWikiModule)->Agent<CreateInstanceByWikiAgent>();
}  // namespace creatingInstanceByWikiModule