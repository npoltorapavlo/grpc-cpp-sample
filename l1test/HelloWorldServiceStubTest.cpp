#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "HelloWorldServiceMock.h"
#include "Server.h"
#include "helloworld.grpc.pb.h"
#include <grpcpp/create_channel.h>

using ::sample::HelloWorldRequest;
using ::sample::HelloWorldResponse;
using ::testing::_;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::IsTrue;
using ::testing::NiceMock;
using ::testing::Test;

const auto kUri = "0.0.0.0:50051";
const auto kMessage = "hello world";

class AHelloWorldServiceStub : public Test {
protected:
    NiceMock<HelloWorldServiceMock> service;
    Server server;
    ::sample::HelloWorldService::Stub stub;
    AHelloWorldServiceStub()
        : server(kUri, &service)
        , stub(grpc::CreateChannel(kUri, grpc::InsecureChannelCredentials()))
    {
    }
    /*
     * grpc::CreateChannel(kUri, grpc::CompositeChannelCredentials(
     *      grpc::SslCredentials(grpc::SslCredentialsOptions()),
     *      grpc::AccessTokenCredentials(token)));
     */
    /*
     * grpc::CreateChannel(kUri,
     *      grpc::SslCredentials(grpc::SslCredentialsOptions()));
     * grpc::ClientContext context;
     * context.AddMetadata("authorization", "Bearer " + token);
     */
    /*
     * grpc::ChannelArguments args;
     * args.SetInt(GRPC_ARG_CLIENT_IDLE_TIMEOUT_MS, 30000);
     * std::shared_ptr<grpc::ChannelCredentials> creds;
     * creds = grpc::InsecureChannelCredentials();
     * stub = ::sample::SampleService::NewStub(
     *      grpc::CreateCustomChannel(kUri, creds, args));
     */
};

TEST_F(AHelloWorldServiceStub, GetsHelloWorldResponse)
{
    HelloWorldRequest req;
    ON_CALL(service, HelloWorld(_, _, _))
        .WillByDefault(Invoke(
            [&](::grpc::ServerContext*, const HelloWorldRequest* request,
                HelloWorldResponse* response) {
                req = (*request);
                response->set_message(kMessage);
                return grpc::Status::OK;
            }));

    grpc::ClientContext context;
    HelloWorldRequest request;
    HelloWorldResponse response;
    auto status = stub.HelloWorld(&context, request, &response);
    ASSERT_THAT(status.ok(), IsTrue());
    EXPECT_THAT(response.message(), Eq(kMessage));
}
