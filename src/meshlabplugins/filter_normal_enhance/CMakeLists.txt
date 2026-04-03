# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES filter_normal_enhance.cpp)

set(HEADERS filter_normal_enhance.h)

add_library(filter_normal_enhance MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_normal_enhance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_normal_enhance PUBLIC common)

set_property(TARGET filter_normal_enhance PROPERTY FOLDER Plugins)

set_property(TARGET filter_normal_enhance PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_normal_enhance PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                             ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_normal_enhance
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)