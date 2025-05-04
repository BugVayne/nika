set(INFERENCE_MODULE_SRC "${SUBSYSTEMS_PATH}/scl-machine/problem-solver/cxx/inferenceModule")

set(SOURCES
	"CreatingInstanceByWikiModule.cpp"
	"agent/CreateInstanceByWikiAgent.cpp"
)

set(HEADERS
	"CreatingInstanceByWikiModule.hpp"
	"keynodes/CreatingInstanceByWikiKeynodes.hpp"
	"agent/CreateInstanceByWikiAgent.hpp"
)

include_directories(
	${CMAKE_CURRENT_LIST_DIR}
	${SC_MEMORY_SRC}
	${SC_KPM_SRC}
	${DIALOG_CONTROL_MODULE_SRC}
	${SC_COMMON_MODULE_SRC}
	${INFERENCE_MODULE_SRC}
   	${DIALOG_CONTROL_MODULE_SRC}
)

add_library(creatingInstanceByWikiModule SHARED ${SOURCES} ${HEADERS})

target_link_libraries(creatingInstanceByWikiModule sc-memory sc-agents-common common dialogControlModule inferenceModule)

if(${SC_CLANG_FORMAT_CODE})
	target_clangformat_setup(creatingInstanceByWikiModule)
endif()
