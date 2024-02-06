cmake_minimum_required(VERSION 3.14)

project(l1test)

set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME}
        HelloWorldServiceStubTest.cpp
)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
FetchContent_MakeAvailable(googletest)
target_link_libraries(${PROJECT_NAME} PRIVATE gmock_main)

find_package(Protobuf REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Protobuf_LIBRARIES})

add_custom_target(protoc
        ${Protobuf_PROTOC_EXECUTABLE}
        --cpp_out ${CMAKE_CURRENT_BINARY_DIR}
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        helloworld.proto
)
add_dependencies(${PROJECT_NAME} protoc)

find_package(gRPC CONFIG REQUIRED)
set(GRPC_LIBS gRPC::grpc++)
set(GRPC_CPP_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GRPC_LIBS})

add_custom_target(protoc-gen-grpc
        ${Protobuf_PROTOC_EXECUTABLE}
        --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        helloworld.proto
)
add_dependencies(${PROJECT_NAME} protoc-gen-grpc)

set(PROTO_SRCS helloworld.pb.cc helloworld.grpc.pb.cc)
target_sources(${PROJECT_NAME} PRIVATE ${PROTO_SRCS})
set_property(SOURCE ${PROTO_SRCS} PROPERTY GENERATED 1)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
