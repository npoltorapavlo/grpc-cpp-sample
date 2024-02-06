#pragma once

#include "sample.grpc.pb.h"
#include <gmock/gmock.h>

class SampleServiceMock : public ::sample::SampleService::Service {
public:
    ~SampleServiceMock() override = default;
    MOCK_METHOD(::grpc::Status, GetValue, (::grpc::ServerContext * context, const ::sample::GetValueRequest* request, ::sample::GetValueResponse* response), (override));
    MOCK_METHOD(::grpc::Status, UpdateValue, (::grpc::ServerContext * context, const ::sample::UpdateValueRequest* request, ::sample::UpdateValueResponse* response), (override));
    MOCK_METHOD(::grpc::Status, DeleteValue, (::grpc::ServerContext * context, const ::sample::DeleteValueRequest* request, ::sample::DeleteValueResponse* response), (override));
};
