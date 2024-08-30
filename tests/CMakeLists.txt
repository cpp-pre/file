# To modify this, edit the template:
# - .tipi/CMakeLists.subdir-tests.txt.tpl
# - tests/CMakeLists.subdir-tests.txt.tpl
#
# The templates can be generated using the 'tipi cmaketpl' command


  # hash
  add_executable(hash hash.cpp )
  set_target_properties(hash PROPERTIES OUTPUT_NAME hash)
  target_link_libraries(hash cpp-pre_file::file)

  if (DEFINED EMSCRIPTEN)
    add_test(NAME hash COMMAND node --experimental-wasm-threads --experimental-wasm-bulk-memory $<TARGET_FILE:hash>)
  else() 
    add_test(NAME hash COMMAND "$<TARGET_FILE:hash>")
  endif()


