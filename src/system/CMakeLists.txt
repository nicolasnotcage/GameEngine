set(LIB_NAME cge_system)
set(SUB_LIB_LIST "${SUB_LIB_LIST}" ${LIB_NAME} PARENT_SCOPE)
file(GLOB SRC_FILES *.c *.cpp)
file(GLOB HDR_FILES *.h *.hpp)
add_library(${LIB_NAME})
target_sources(${LIB_NAME} PRIVATE ${SRC_FILES} PUBLIC ${HDR_FILES})
target_compile_definitions(${LIB_NAME} PUBLIC)
