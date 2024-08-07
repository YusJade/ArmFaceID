// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: face_network.proto

#include "face_network.pb.h"
#include "face_network.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace arm_face_id {

static const char* RpcManager_method_names[] = {
  "/arm_face_id.RpcManager/RecognizeFace",
};

std::unique_ptr< RpcManager::Stub> RpcManager::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< RpcManager::Stub> stub(new RpcManager::Stub(channel, options));
  return stub;
}

RpcManager::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_RecognizeFace_(RpcManager_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status RpcManager::Stub::RecognizeFace(::grpc::ClientContext* context, const ::arm_face_id::RecognizeRequest& request, ::arm_face_id::RecognizeResult* response) {
  return ::grpc::internal::BlockingUnaryCall< ::arm_face_id::RecognizeRequest, ::arm_face_id::RecognizeResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_RecognizeFace_, context, request, response);
}

void RpcManager::Stub::async::RecognizeFace(::grpc::ClientContext* context, const ::arm_face_id::RecognizeRequest* request, ::arm_face_id::RecognizeResult* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::arm_face_id::RecognizeRequest, ::arm_face_id::RecognizeResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RecognizeFace_, context, request, response, std::move(f));
}

void RpcManager::Stub::async::RecognizeFace(::grpc::ClientContext* context, const ::arm_face_id::RecognizeRequest* request, ::arm_face_id::RecognizeResult* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RecognizeFace_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::arm_face_id::RecognizeResult>* RpcManager::Stub::PrepareAsyncRecognizeFaceRaw(::grpc::ClientContext* context, const ::arm_face_id::RecognizeRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::arm_face_id::RecognizeResult, ::arm_face_id::RecognizeRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_RecognizeFace_, context, request);
}

::grpc::ClientAsyncResponseReader< ::arm_face_id::RecognizeResult>* RpcManager::Stub::AsyncRecognizeFaceRaw(::grpc::ClientContext* context, const ::arm_face_id::RecognizeRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncRecognizeFaceRaw(context, request, cq);
  result->StartCall();
  return result;
}

RpcManager::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RpcManager_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RpcManager::Service, ::arm_face_id::RecognizeRequest, ::arm_face_id::RecognizeResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](RpcManager::Service* service,
             ::grpc::ServerContext* ctx,
             const ::arm_face_id::RecognizeRequest* req,
             ::arm_face_id::RecognizeResult* resp) {
               return service->RecognizeFace(ctx, req, resp);
             }, this)));
}

RpcManager::Service::~Service() {
}

::grpc::Status RpcManager::Service::RecognizeFace(::grpc::ServerContext* context, const ::arm_face_id::RecognizeRequest* request, ::arm_face_id::RecognizeResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace arm_face_id

