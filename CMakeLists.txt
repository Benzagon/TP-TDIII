cmake_minimum_required (VERSION 3.11)

project(td3-blockchain
  LANGUAGES CXX
  VERSION 1.0.0
)

include(FetchContent)

# --- Fetch GoogleTest ---------------------------------------------------------

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

# --- Ejecutable: tests -------------------------------------------------

add_executable(tests tests/tests_blockchain.cpp tests/tests_billetera.cpp billetera.cpp blockchain.cpp calendario.cpp)

target_link_libraries(
  tests
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
