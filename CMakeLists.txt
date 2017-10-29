cmake_minimum_required(VERSION 3.7)
project(SimpleHttp)

set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(Boost_USE_STATIC_LIBS   ON)
endif()

find_package(Boost COMPONENTS system date_time regex filesystem REQUIRED)
find_package(Threads REQUIRED)

file(GLOB HTTP_LIB_FILES	httpd/*.hh httpd/*.cc BrightFuture/executor/BoostAsioExecutor.cc)

include_directories(
    ${Boost_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/BrightFuture
)

add_library(httpdlib
    ${HTTP_LIB_FILES}
    )

target_link_libraries(httpdlib
    Threads::Threads
    ${Boost_LIBRARIES}
)

set(SOURCE_FILES main.cc)
add_executable(SimpleHttp ${SOURCE_FILES})
target_link_libraries(SimpleHttp httpdlib)

add_subdirectory(BrightFuture)
