#include "user_repository.hpp"

#include <utils/pool/mariadb/db_pool.hpp>

namespace core
{

UserDO UserRepository::getUserById(const std::string& userId)
{
  auto conn = utils::DBPool::GetInstance().GetConnection();

  const char* sql = "SELECT nickname, avatar, email FROM users WHERE uuid = ?";

  utils::StringBuffer<64> nickname;
  utils::StringBuffer<128> avatar;
  utils::StringBuffer<255> email;

  auto params = utils::MakeParams(userId);
  auto results = utils::MakeResults(nickname, avatar, email);

  if (conn.QueryOne(sql, params, results))
  {
    return {.id = 0,
            .uuid = {},
            .nickname = nickname.str(),
            .avatar = avatar.str(),
            .email = email.str(),
            .password_hash = {},
            .last_login = {},
            .created_at = {},
            .updated_at = {}};
  }
  return {};
}

}  // namespace core