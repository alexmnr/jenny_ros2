cmake_minimum_required(VERSION 3.22)
project(jonny_robot_description)

find_package(ament_cmake REQUIRED)

install(DIRECTORY urdf DESTINATION share/${PROJECT_NAME})
install(DIRECTORY meshes DESTINATION share/${PROJECT_NAME})
ament_package()
