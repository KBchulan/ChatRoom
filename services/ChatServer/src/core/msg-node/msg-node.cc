#include "msg-node.hpp"

#include <boost/asio/detail/socket_ops.hpp>
#include <cstddef>
#include <global/Global.hpp>

namespace core
{

using namespace global::server;

MsgNode::MsgNode(short capacity) : _cur_len(0), _data(static_cast<std::size_t>(capacity))
{
}

void MsgNode::Clear() noexcept
{
  _cur_len = 0;
}

short MsgNode::GetMsgId() const noexcept
{
  return -1;
}

short MsgNode::GetSize() const noexcept
{
  return _cur_len;
}

std::span<const char> MsgNode::GetData() const noexcept
{
  return {_data.data(), static_cast<size_t>(_cur_len)};
}

RecvNode::RecvNode(short msg_id, std::span<const char> body) : MsgNode(static_cast<short>(body.size())), _msg_id(msg_id)
{
  std::memcpy(_data.data(), body.data(), body.size());
  _cur_len = static_cast<short>(body.size());
}

short RecvNode::GetMsgId() const noexcept
{
  return _msg_id;
}

SendNode::SendNode(short msg_id, std::span<const char> body)
    : MsgNode(static_cast<short>(MSG_HEAD_TOTAL_LEN + body.size())), _msg_id(msg_id)
{
  // 写入消息
  auto net_id = boost::asio::detail::socket_ops::host_to_network_short(static_cast<uint16_t>(msg_id));
  std::memcpy(_data.data(), &net_id, MSG_TYPE_LENGTH);

  // 写入消息长度
  auto net_len = boost::asio::detail::socket_ops::host_to_network_short(static_cast<uint16_t>(body.size()));
  std::memcpy(_data.data() + MSG_TYPE_LENGTH, &net_len, MSG_LEN_LENGTH);

  // 写入消息体
  if (!body.empty())
  {
    std::memcpy(_data.data() + MSG_HEAD_TOTAL_LEN, body.data(), body.size());
  }

  // 记录总长度
  _cur_len = static_cast<short>(MSG_HEAD_TOTAL_LEN + body.size());
}

short SendNode::GetMsgId() const noexcept
{
  return _msg_id;
}

}  // namespace core