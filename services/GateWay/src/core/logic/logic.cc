#include "logic.hpp"

#include <core/controller/demo/demo_controller.hpp>
#include <core/domain/query/demo/demo_query.hpp>
#include <tools/Logger.hpp>
#include <unordered_map>
#include <utils/code.hpp>
#include <utils/query_parser.hpp>

namespace core
{

struct Logic::_impl
{
  std::unordered_map<std::string, GetHandlers> _get_handlers;

  RequestHandleResult handle_get_request(const std::string& target)
  {
    auto pos = target.find('?');
    std::string path = (pos == std::string::npos) ? target : target.substr(0, pos);
    core::CommonVO common_vo{};

    if (_get_handlers.contains(path))
    {
      _get_handlers[path](target, common_vo);
      return common_vo.ToString();
    }

    common_vo.code = utils::URL_NOT_FOUND;
    common_vo.message = "URL not found";
    common_vo.data = "";
    return std::unexpected(common_vo.ToString());
  }

  // RequestHandleResult handle_post_request(const std::string& target, const std::string& body);
  // RequestHandleResult handle_put_request(const std::string& target, const std::string& body);
  // RequestHandleResult handle_delete_request(const std::string& target);

  _impl()
  {
    _get_handlers["/demo/get-test"] = [](const std::string& target, core::CommonVO& common_vo)
    {
      auto query = utils::ParseQuery<GetTestQuery>(target);
      DemoController::GetInstance().HandleDemoGetRequest(query, common_vo);
    };
  }

  ~_impl() = default;
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

RequestHandleResult Logic::HandleGetRequest(const std::string& target)
{
  return _pimpl->handle_get_request(target);
}

// RequestHandleResult Logic::HandlePostRequest(const std::string& target, const std::string& body)
// {
//   return _pimpl->handle_post_request(target, body);
// }

// RequestHandleResult Logic::HandlePutRequest(const std::string& target, const std::string& body)
// {
//   return _pimpl->handle_put_request(target, body);
// }

// RequestHandleResult Logic::HandleDeleteRequest(const std::string& target)
// {
//   return _pimpl->handle_delete_request(target);
// }

}  // namespace core