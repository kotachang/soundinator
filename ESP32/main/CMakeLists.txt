set(srcs "src/soundinator_main.cpp" "src/nimbler.cpp" "src/player.cpp" "src/test_note.cpp")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "inc")
target_compile_definitions(${COMPONENT_LIB} PUBLIC CONFIG_TEST_NOTE) 