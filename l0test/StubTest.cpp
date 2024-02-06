#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SampleServiceMock.h"
#include "Server.h"
#include "sample.grpc.pb.h"
#include <grpcpp/create_channel.h>

using ::sample::DeleteValueRequest;
using ::sample::DeleteValueResponse;
using ::sample::GetValueRequest;
using ::sample::GetValueResponse;
using ::sample::Key;
using ::sample::UpdateValueRequest;
using ::sample::UpdateValueResponse;
using ::sample::Value;
using ::testing::_;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::IsTrue;
using ::testing::NiceMock;
using ::testing::Test;

const auto kUri = "0.0.0.0:50051";
const auto kValue = "value_1";
const auto kKey = "key_1";
const auto kTtl = 100;
const auto kExpireTime = 1707238543;

class AStub : public Test {
protected:
    NiceMock<SampleServiceMock> service;
    Server server;
    ::sample::SampleService::Stub stub;
    AStub()
        : server(kUri, &service)
        , stub(grpc::CreateChannel(kUri, grpc::InsecureChannelCredentials()))
    /*
     * grpc::CreateChannel(kUri, grpc::CompositeChannelCredentials(
     *      grpc::SslCredentials(grpc::SslCredentialsOptions()),
     *      grpc::AccessTokenCredentials(token)));
     */
    /*
     * grpc::CreateChannel(kUri, grpc::SslCredentials(grpc::SslCredentialsOptions()));
     * grpc::ClientContext context;
     * context.AddMetadata("authorization", "Bearer " + token);
     */
    {
    }
};

TEST_F(AStub, GetsValueWithTtl)
{
    GetValueRequest req;
    ON_CALL(service, GetValue(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const GetValueRequest* request, GetValueResponse* response) {
                req = (*request);
                auto v = new Value();
                v->set_value(kValue);
                auto t = new google::protobuf::Duration();
                t->set_seconds(kTtl);
                v->set_allocated_ttl(t);
                auto k = new Key();
                k->set_key(request->key().key());
                v->set_allocated_key(k);
                response->set_allocated_value(v);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    GetValueRequest request;
    auto k = new Key();
    k->set_key(kKey);
    request.set_allocated_key(k);
    GetValueResponse response;
    auto status = stub.GetValue(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    ASSERT_THAT(req.has_key(), IsTrue());
    EXPECT_THAT(req.key().key(), Eq(kKey));
    ASSERT_THAT(response.has_value(), IsTrue());
    EXPECT_THAT(response.value().value(), Eq(kValue));
    ASSERT_THAT(response.value().has_key(), IsTrue());
    EXPECT_THAT(response.value().key().key(), Eq(kKey));
    ASSERT_THAT(response.value().has_ttl(), IsTrue());
    EXPECT_THAT(response.value().ttl().seconds(), Eq(kTtl));
}

TEST_F(AStub, GetsValueWithExpireTime)
{
    GetValueRequest req;
    ON_CALL(service, GetValue(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const GetValueRequest* request, GetValueResponse* response) {
                req = (*request);
                auto v = new Value();
                v->set_value(kValue);
                auto t = new google::protobuf::Timestamp();
                t->set_seconds(kExpireTime);
                v->set_allocated_expire_time(t);
                auto k = new Key();
                k->set_key(request->key().key());
                v->set_allocated_key(k);
                response->set_allocated_value(v);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    GetValueRequest request;
    auto k = new Key();
    k->set_key(kKey);
    request.set_allocated_key(k);
    GetValueResponse response;
    auto status = stub.GetValue(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    ASSERT_THAT(req.has_key(), IsTrue());
    EXPECT_THAT(req.key().key(), Eq(kKey));
    ASSERT_THAT(response.has_value(), IsTrue());
    EXPECT_THAT(response.value().value(), Eq(kValue));
    ASSERT_THAT(response.value().has_key(), IsTrue());
    EXPECT_THAT(response.value().key().key(), Eq(kKey));
    ASSERT_THAT(response.value().has_expire_time(), IsTrue());
    EXPECT_THAT(response.value().expire_time().seconds(), Eq(kExpireTime));
}

TEST_F(AStub, UpdatesValueWithTtl)
{
    UpdateValueRequest req;
    ON_CALL(service, UpdateValue(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const UpdateValueRequest* request, UpdateValueResponse*) {
                req = (*request);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    UpdateValueRequest request;
    auto v = new Value();
    v->set_value(kValue);
    auto t = new google::protobuf::Duration();
    t->set_seconds(kTtl);
    v->set_allocated_ttl(t);
    auto k = new Key();
    k->set_key(kKey);
    v->set_allocated_key(k);
    request.set_allocated_value(v);
    UpdateValueResponse response;
    auto status = stub.UpdateValue(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    ASSERT_THAT(req.has_value(), IsTrue());
    EXPECT_THAT(req.value().value(), Eq(kValue));
    ASSERT_THAT(req.value().has_key(), IsTrue());
    EXPECT_THAT(req.value().key().key(), Eq(kKey));
    ASSERT_THAT(req.value().has_ttl(), IsTrue());
    EXPECT_THAT(req.value().ttl().seconds(), Eq(kTtl));
}

TEST_F(AStub, UpdatesValueWithExpireTime)
{
    UpdateValueRequest req;
    ON_CALL(service, UpdateValue(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const UpdateValueRequest* request, UpdateValueResponse*) {
                req = (*request);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    UpdateValueRequest request;
    auto v = new Value();
    v->set_value(kValue);
    auto t = new google::protobuf::Timestamp();
    t->set_seconds(kExpireTime);
    v->set_allocated_expire_time(t);
    auto k = new Key();
    k->set_key(kKey);
    v->set_allocated_key(k);
    request.set_allocated_value(v);
    UpdateValueResponse response;
    auto status = stub.UpdateValue(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    ASSERT_THAT(req.has_value(), IsTrue());
    EXPECT_THAT(req.value().value(), Eq(kValue));
    ASSERT_THAT(req.value().has_key(), IsTrue());
    EXPECT_THAT(req.value().key().key(), Eq(kKey));
    ASSERT_THAT(req.value().has_expire_time(), IsTrue());
    EXPECT_THAT(req.value().expire_time().seconds(), Eq(kExpireTime));
}

TEST_F(AStub, DeletesKey)
{
    DeleteValueRequest req;
    ON_CALL(service, DeleteValue(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const DeleteValueRequest* request, DeleteValueResponse*) {
                req = (*request);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    DeleteValueRequest request;
    auto k = new Key();
    k->set_key(kKey);
    request.set_allocated_key(k);
    DeleteValueResponse response;
    auto status = stub.DeleteValue(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    ASSERT_THAT(req.has_key(), IsTrue());
    EXPECT_THAT(req.key().key(), Eq(kKey));
}
