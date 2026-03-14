/******************************************************************************
 *
 * @file       chat_service.hpp
 * @brief      用于 RPC 通信的聊天服务端
 *
 * @author     KBchulan
 * @date       2026/03/02
 * @history
 ******************************************************************************/

#ifndef CHAT_SERVICE_HPP
#define CHAT_SERVICE_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include <utils/grpc/chat_server/chat_server.grpc.pb.h>
#include <utils/grpc/chat_server/chat_server.pb.h>
#pragma GCC diagnostic pop

#include <utils/UtilsExport.hpp>

namespace utils
{

using namespace KBchulan::ChatRoom::ChatServer;

class UTILS_EXPORT ChatServiceImpl final : public ChatService::Service
{
public:
  ChatServiceImpl() = default;
  ~ChatServiceImpl() override = default;
};

}  // namespace utils

#endif  // CHAT_SERVICE_HPP