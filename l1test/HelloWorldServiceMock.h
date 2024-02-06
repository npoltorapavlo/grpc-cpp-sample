#pragma once

#include "helloworld.grpc.pb.h"
#include <gmock/gmock.h>

class HelloWorldServiceMock : public ::sample::HelloWorldService::Service {
public:
    ~HelloWorldServiceMock() override = default;
    MOCK_METHOD(::grpc::Status, HelloWorld,
        (::grpc::ServerContext * context,
            const ::sample::HelloWorldRequest* request,
            ::sample::HelloWorldResponse* response),
        (override));
};
