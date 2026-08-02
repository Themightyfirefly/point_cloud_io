cmake_minimum_required(VERSION 3.8)
project(point_cloud_io)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options.
option(BUILD_WITH_VTK_SUPPORT "Build package with support for VTK files." OFF)
if(BUILD_WITH_VTK_SUPPORT)
  message("point_cloud_io will be built with support for VTK files.")
  add_definitions(-DHAVE_VTK)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(include)

###########
## Build ##
###########
add_executable(read
  src/read_node.cpp
  src/Read.cpp
)
target_include_directories(read PUBLIC include)
ament_target_dependencies(read
  pcl_ros
  sensor_msgs
)

#add_executable(write
#  src/write_node.cpp
#  src/Write.cpp
#)
#target_include_directories(write PUBLIC include)
#ament_target_dependencies(write
#  pcl_ros
#  sensor_msgs
#)

#############
## Install ##
#############
install(
  TARGETS
    read
    #write
  DESTINATION lib/${PROJECT_NAME}
)
#install(
#  DIRECTORY
#    include/${PROJECT_NAME}/
#  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#)
install(
  DIRECTORY
    examples
    launch
  DESTINATION share/${PROJECT_NAME}
)

#find_package(cmake_clang_tools QUIET)
#if(cmake_clang_tools_FOUND)
#  add_default_clang_tooling(
#    TARGETS
#      read
#      write
#  )
#endif(cmake_clang_tools_FOUND)

##############
##   Test   ##
##############
#if(CATKIN_ENABLE_TESTING)
#  catkin_add_gtest(test_${PROJECT_NAME}
#    test/empty_test.cpp
#  )
#  add_dependencies(test_${PROJECT_NAME}
#    read
#    write
#  )
#  target_include_directories(test_${PROJECT_NAME} PRIVATE
#    include
#  )
#  target_include_directories(test_${PROJECT_NAME} SYSTEM PUBLIC
#    ${catkin_INCLUDE_DIRS}
#  )
#  target_link_libraries(test_${PROJECT_NAME}
#    gtest_main
#    ${catkin_LIBRARIES}
#  )

  ###################
  ## Code_coverage ##
  ###################
#  find_package(cmake_code_coverage QUIET)
#  if(cmake_code_coverage_FOUND)
#    add_gtest_coverage(
#      TEST_BUILD_TARGETS
#        test_${PROJECT_NAME}
#    )
#  endif()
#endif()

ament_package()