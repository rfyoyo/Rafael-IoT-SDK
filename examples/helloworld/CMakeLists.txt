cmake_minimum_required(VERSION 3.15)
include(proj.conf)
find_package(rafael_iot_sdk REQUIRED HINTS $ENV{RAFAEL_SDK_BASE})
project(helloworld LANGUAGES C CXX ASM)

sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/helloworld/Include)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/helloworld/app.c
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/helloworld/main.c)

setup_project(helloworld)