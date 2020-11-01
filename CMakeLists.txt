cmake_minimum_required(VERSION 3.10.2)
project(synchronizer) 

# Using C++17
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()


################
# Dependencies #
################

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(mpi_cmake_modules REQUIRED)
find_package(shared_memory REQUIRED)
find_package(real_time_tools REQUIRED)
find_package(pybind11_vendor REQUIRED)
find_package(pybind11 REQUIRED)
find_package(cereal REQUIRED)
ament_export_dependencies(
  mpi_cmake_modules
  pybind11
  shared_memory
  real_time_tools
  cereal)


# prepare to export all needed targets
set(all_targets)
set(all_target_exports)


###########
# Library #
###########

add_library( ${PROJECT_NAME} SHARED
  src/follower_sync.cpp
  src/leader_sync.cpp
  src/follower.cpp
  src/leader.cpp )
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(${PROJECT_NAME} cereal)
target_link_libraries(${PROJECT_NAME} shared_memory::shared_memory)
target_link_libraries(${PROJECT_NAME} real_time_tools::real_time_tools)
ament_export_interfaces(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
list(APPEND all_targets ${PROJECT_NAME})
list(APPEND all_target_exports export_${PROJECT_NAME})


#########
# demos #
#########

add_executable(demo_follower
  demos/demo_follower.cpp)
target_include_directories(
  demo_follower
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)
target_link_libraries(demo_follower ${PROJECT_NAME})
set(all_targets ${all_targets} demo_follower)
install(TARGETS demo_follower RUNTIME DESTINATION lib/${PROJECT_NAME})

add_executable(demo_leader
  demos/demo_leader.cpp)
target_include_directories(
  demo_leader
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)
target_link_libraries(demo_leader ${PROJECT_NAME})
set(all_targets ${all_targets} demo_leader)
install(TARGETS demo_leader RUNTIME DESTINATION lib/${PROJECT_NAME})


###################
# Python wrappers #
###################

add_library(${PROJECT_NAME}_cpp_bindings MODULE srcpy/wrappers.cpp)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE pybind11::module)
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_cpp_bindings PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_cpp_bindings
  PROPERTIES PREFIX "" SUFFIX "${PYTHON_MODULE_EXTENSION}"
  OUTPUT_NAME ${PROJECT_NAME})
target_include_directories(
  ${PROJECT_NAME}_cpp_bindings
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include> SYSTEM
  PUBLIC ${PYTHON_INCLUDE_DIRS})
_ament_cmake_python_get_python_install_dir()
install(TARGETS ${PROJECT_NAME}_cpp_bindings DESTINATION ${PYTHON_INSTALL_DIR})


#################
# documentation #
#################

add_documentation()


######################
# Install and export #
######################\

install(DIRECTORY include/ DESTINATION include)
install(
  TARGETS ${all_targets}
  EXPORT ${all_target_exports}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include)
ament_package()


##############
# unit tests #
##############

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_${PROJECT_NAME} tests/main.cpp tests/unit_tests.cpp)
  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})
endif()

      



