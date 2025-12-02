#include "demo_controller.hpp"

#include <json/value.h>

#include <utils/code.hpp>
#include <utils/type.hpp>

namespace core
{

struct DemoController::_impl
{
  void handle_demo_get_request(const GetTestQuery& query, core::CommonVO& common_vo) const
  {
    (void)this;

    Json::Value data;
    data["name"] = query.name;
    data["age"] = query.age;

    common_vo.code = utils::SUCCESS;
    common_vo.message = "Success";
    common_vo.data = data;
  }

  _impl() = default;
  ~_impl() = default;
};

DemoController::DemoController() : _pimpl(std::make_unique<_impl>())
{
}

DemoController::~DemoController() = default;

DemoController& DemoController::GetInstance()
{
  static DemoController instance;
  return instance;
}

void DemoController::HandleDemoGetRequest(const GetTestQuery& query, core::CommonVO& common_vo) const
{
  _pimpl->handle_demo_get_request(query, common_vo);
}

}  // namespace core