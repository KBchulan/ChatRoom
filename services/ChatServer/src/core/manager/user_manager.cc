#include "user_manager.hpp"

namespace core
{

struct UserManager::_impl
{
  // TODO: 后续实现用户管理的具体逻辑
};

UserManager::UserManager() : _pimpl(std::make_unique<_impl>())
{
}

UserManager::~UserManager() = default;

UserManager& UserManager::GetInstance()
{
  static UserManager instance;
  return instance;
}

}  // namespace core