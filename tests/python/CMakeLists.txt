
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  
  add_test (
    NAME test1
    COMMAND Python3::Interpreter -m pytest -v test1.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests/python/
    )

  set_tests_properties(test1
    PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../../src/:${PROJECT_SOURCE_DIR}/src/:$ENV{PYTHONPATH}")

endif()
