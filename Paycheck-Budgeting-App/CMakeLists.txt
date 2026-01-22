cmake_minimum_required(VERSION 3.10)
project(BudgetApp)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(OpenXLSX-master)

add_executable(budget_app main.cpp Budget.cpp)
target_link_libraries(budget_app OpenXLSX)