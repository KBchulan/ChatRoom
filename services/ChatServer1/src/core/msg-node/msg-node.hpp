/******************************************************************************
 *
 * @file       msg-node.hpp
 * @brief      消息节点的统一定义
 *
 * @author     KBchulan
 * @date       2025/12/13
 * @history
 ******************************************************************************/

#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <core/CoreExport.hpp>
#include <span>
#include <vector>

namespace core
{

class CORE_EXPORT MsgNode
{
public:
  explicit MsgNode(short capacity);
  virtual ~MsgNode() = default;

  void Clear() noexcept;

  [[nodiscard]] virtual short GetMsgId() const noexcept;
  [[nodiscard]] short GetSize() const noexcept;
  [[nodiscard]] std::span<const char> GetData() const noexcept;

  MsgNode(const MsgNode&) = delete;
  MsgNode(MsgNode&&) = delete;
  MsgNode& operator=(const MsgNode&) = delete;
  MsgNode& operator=(MsgNode&&) = delete;

protected:
  short _cur_len;
  std::vector<char> _data;
};

class CORE_EXPORT RecvNode : public MsgNode
{
public:
  RecvNode(short msg_id, std::span<const char> body);

  [[nodiscard]] short GetMsgId() const noexcept override;

private:
  short _msg_id;
};

class CORE_EXPORT SendNode : public MsgNode
{
public:
  SendNode(short msg_id, std::span<const char> body);

  [[nodiscard]] short GetMsgId() const noexcept override;

private:
  short _msg_id;
};

}  // namespace core

#endif  // MSGNODE_HPP