#include "logic.hpp"

#include <json/json.h>

#include <atomic>
#include <core/repository/user_repository.hpp>
#include <core/session/session.hpp>
#include <global/Global.hpp>
#include <global/SuperQueue.hpp>
#include <memory>
#include <optional>
#include <thread>
#include <tools/Logger.hpp>
#include <unordered_map>
#include <utils/common/code.hpp>
#include <utils/grpc/client/status_server_client.hpp>
#include <utils/pool/redis/redis_pool.hpp>

namespace core
{

struct LogicTask
{
  Session::Ptr session;
  std::shared_ptr<RecvNode> msg;
};

struct Logic::_impl
{
  std::atomic<bool> _running{true};
  std::atomic<bool> _has_task{false};

  global::SuperQueue<LogicTask, global::server::LOGIC_QUEUE_CAPACITY> _queue;
  std::jthread _thread;

  std::unordered_map<short, Logic::CallBack> _handlers;

  utils::StatusServerClinet& _status_server_client = utils::StatusServerClinet::GetInstance();

  void handle_message(const Session::Ptr& session, const std::shared_ptr<RecvNode>& msg)
  {
    auto msg_id = msg->GetMsgId();

    // 分发消息
    if (_handlers.contains(msg_id))
    {
      _handlers[msg_id](session, msg->GetData());
    }
    else
    {
      tools::Logger::getInstance().error("Unrecognized message id: {}", msg_id);
    }
  }

  void run()
  {
    tools::Logger::getInstance().info("Logic thread started");

    while (_running.load(std::memory_order_acquire))
    {
      LogicTask task;

      if (_queue.pop(task))
      {
        handle_message(task.session, task.msg);
        continue;
      }

      _has_task.store(false, std::memory_order_relaxed);
      _has_task.wait(false, std::memory_order_acquire);
    }

    tools::Logger::getInstance().info("Logic thread stopped");
  }

  static std::optional<Json::Value> parse_json(const std::string& json_str)
  {
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream stream(json_str);

    if (!Json::parseFromStream(builder, stream, &root, &errors))
    {
      return std::nullopt;
    }
    return root;
  }

  void init_handlers()
  {
    // 注册消息处理函数
    _handlers[utils::ID_LOGIN_CHAT] = [this](const Session::Ptr& session, const std::span<const char>& data)
    {
      Json::Value response;
      Json::StreamWriterBuilder writer;

      auto root = parse_json(std::string(data.data(), data.size()));
      if (!root)
      {
        tools::Logger::getInstance().error("Failed to parse JSON");
        response["code"] = utils::JSON_PARSE_ERROR;
        response["message"] = "Failed to parse JSON";
        session->Send(std::make_shared<SendNode>(utils::ID_LOGIN_CHAT_RESPONSE, Json::writeString(writer, response)));
        return;
      }

      // 调用 RPC 校验
      auto uuid = root.value()["uuid"].asString();
      auto token = root.value()["token"].asString();
      auto res = _status_server_client.VerifyLoginInfo(uuid, token);
      if (!res)
      {
        tools::Logger::getInstance().error("Login failed: {}", res.error().message);
        response["code"] = res.error().code;
        response["message"] = res.error().message;
        session->Send(std::make_shared<SendNode>(utils::ID_LOGIN_CHAT_RESPONSE, Json::writeString(writer, response)));
        return;
      }

      // 查询用户基本信息
      UserDO user = UserRepository::getUserById(uuid);

      Json::Value user_info;
      user_info["nickname"] = user.nickname;
      user_info["avatar"] = user.avatar;
      user_info["email"] = user.email;

      // 存入 redis，按照 prefix + uuid 作为 key
      auto redis_conn = utils::RedisPool::GetInstance().GetConnection();
      auto key = global::server::USER_INFO_PREFIX + uuid;
      auto pipeline = redis_conn.NewPipeLine();
      pipeline.Append("HSET %s nickname %s", key.c_str(), user.nickname.c_str())
          .Append("HSET %s avatar %s", key.c_str(), user.avatar.c_str())
          .Append("HSET %s email %s", key.c_str(), user.email.c_str())
          .Append("EXPIRE %s %d", key.c_str(), global::server::USER_INFO_EXPIRE_TIME_S);
      pipeline.Execute();

      // 返回响应
      response["code"] = utils::SUCCESS;
      response["message"] = "Login successful";
      response["data"] = user_info;
      session->Send(std::make_shared<SendNode>(utils::ID_LOGIN_CHAT_RESPONSE, Json::writeString(writer, response)));
    };
  }

  _impl() : _thread([this] { run(); })
  {
    init_handlers();
  }

  ~_impl()
  {
    _running.store(false, std::memory_order_release);
    _has_task.store(true, std::memory_order_release);
    _has_task.notify_one();
  }
};

Logic::Logic() : _pimpl(std::make_unique<_impl>())
{
}

Logic::~Logic() = default;

Logic& Logic::GetInstance()
{
  static Logic instance;
  return instance;
}

void Logic::PostToLogic(const std::shared_ptr<Session>& session, const std::shared_ptr<RecvNode>& msg)
{
  if (_pimpl->_queue.emplace(LogicTask{.session = session, .msg = msg}))
  {
    _pimpl->_has_task.store(true, std::memory_order_release);
    _pimpl->_has_task.notify_one();
  }
  else
  {
    tools::Logger::getInstance().error("Logic queue is full");
  }
}

}  // namespace core