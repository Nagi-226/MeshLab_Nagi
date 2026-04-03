# Copyright 2026
# SPDX-License-Identifier: BSL-1.0

set(SOURCES filter_custom.cpp)

set(HEADERS filter_custom.h)

add_meshlab_plugin(filter_custom ${SOURCES} ${HEADERS})
