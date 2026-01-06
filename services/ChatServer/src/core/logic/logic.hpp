/******************************************************************************
 *
 * @file       logic.hpp
 * @brief      逻辑系统
 *
 * @author     KBchulan
 * @date       2025/12/14
 * @history
 ******************************************************************************/

#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <core/CoreExport.hpp>
#include <core/msg-node/msg-node.hpp>
#include <functional>
#include <memory>
#include <span>

namespace core
{

class Session;
class CORE_EXPORT Logic
{
public:
  using CallBack = std::function<void(const std::shared_ptr<Session>&, std::span<const char>)>;

  static Logic& GetInstance();

  void PostToLogic(const std::shared_ptr<Session>& session, const std::shared_ptr<RecvNode>& msg);

  Logic(const Logic&) = delete;
  Logic& operator=(const Logic&) = delete;
  Logic(Logic&&) = delete;
  Logic& operator=(Logic&&) = delete;

private:
  Logic();
  ~Logic();

  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

}  // namespace core

#endif  // LOGIC_HPP