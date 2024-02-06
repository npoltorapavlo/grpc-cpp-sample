cmake_minimum_required(VERSION 3.14)

project(l0test)

set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME}
        StubTest.cpp
)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)
target_link_libraries(${PROJECT_NAME} PRIVATE gmock_main)

# if dlopen libs that use libprotobuf,
# you can get "File already exists in database"

find_package(Protobuf REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Protobuf_LIBRARIES})

# if protobuf_generate doesn't work, call protoc directly

add_custom_target(protoc
        ${Protobuf_PROTOC_EXECUTABLE} --cpp_out ${CMAKE_CURRENT_BINARY_DIR} -I ${CMAKE_CURRENT_SOURCE_DIR}/../sample ${CMAKE_CURRENT_SOURCE_DIR}/../sample/sample.proto
)
add_dependencies(${PROJECT_NAME} protoc)

# if cross compiling, get libs from recipe-sysroot
# and grpc_cpp_plugin from recipe-sysroot-native,
# ex. target_link_libraries(... grpc++) and find_program(... grpc_cpp_plugin)

find_package(gRPC CONFIG)
if (gRPC_FOUND)
    set(GRPC_LIBS gRPC::grpc++)
    set(GRPC_CPP_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
else ()
    set(GRPC_LIBS grpc++)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
endif ()
target_link_libraries(${PROJECT_NAME} PRIVATE ${GRPC_LIBS})

add_custom_target(protoc-gen-grpc
        ${Protobuf_PROTOC_EXECUTABLE} --grpc_out ${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} -I ${CMAKE_CURRENT_SOURCE_DIR}/../sample ${CMAKE_CURRENT_SOURCE_DIR}/../sample/sample.proto
)
add_dependencies(${PROJECT_NAME} protoc-gen-grpc)

set(PROTO_SRCS sample.pb.cc sample.grpc.pb.cc)
target_sources(${PROJECT_NAME} PRIVATE ${PROTO_SRCS})
set_property(SOURCE ${PROTO_SRCS} PROPERTY GENERATED 1)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
