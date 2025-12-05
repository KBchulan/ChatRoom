#include "context.hpp"

#include <unordered_map>

namespace utils
{

struct Context::_impl
{
  std::unordered_map<std::string, std::any> _personal_val;
  ParsedUrl _url;
  std::string _body;

  _impl(ParsedUrl&& url, std::string&& body) : _url(std::move(url)), _body(std::move(body))
  {
  }
  ~_impl() = default;
};

Context::Context(ParsedUrl& url, std::string& body) : pimpl(std::make_unique<_impl>(std::move(url), std::move(body)))
{
}

Context::~Context() = default;

void Context::Set(const std::string& key, std::any value)
{
  pimpl->_personal_val[key] = std::move(value);
}

[[nodiscard]] std::any Context::Get(const std::string& key) const
{
  auto iter = pimpl->_personal_val.find(key);
  if (iter != pimpl->_personal_val.end())
  {
    return iter->second;
  }
  return std::any{};
}

[[nodiscard]] const ParsedUrl& Context::GetUrl() const
{
  return pimpl->_url;
}

[[nodiscard]] const std::string& Context::GetBody() const
{
  return pimpl->_body;
}

}  // namespace utils