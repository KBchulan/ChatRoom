#include "user_repository.hpp"

#include <core/domain/do/user/user_do.hpp>
#include <utils/pool/mariadb/db_pool.hpp>

namespace core
{

struct UserRepository::_impl
{
  utils::DBPool& _db_pool = utils::DBPool::GetInstance();

  [[nodiscard]] bool insert_verify_code(const UserVerifyCodeDO& dto) const
  {
    auto conn = _db_pool.GetConnection();

    const char* sql = "INSERT INTO user_verification_codes (email, code, purpose) VALUES (?, ?, ?)";
    auto params = utils::MakeParams(dto.email, dto.code, dto.purpose);

    return conn.Execute(sql, params);
  }

  [[nodiscard]] std::expected<bool, std::string> check_user_exists(const std::string& email,
                                                                   const std::string& nickname) const
  {
    auto conn = _db_pool.GetConnection();

    std::int8_t count = 0;
    auto params = utils::MakeParams(nickname, email);
    auto results = utils::MakeResults(count);

    const char* sql = "SELECT COUNT(*) FROM users WHERE nickname = ? OR email = ?";
    if (!conn.QueryOne(sql, params, results))
    {
      return std::unexpected("Failed to execute check_user_exists query");
    }

    return count > 0;
  }

  [[nodiscard]] bool insert_user(const UserDO& user_do) const
  {
    auto conn = _db_pool.GetConnection();

    const char* sql = "INSERT INTO users (uuid, nickname, avatar, email, password_hash) VALUES (?, ?, ?, ?, ?)";
    auto params =
        utils::MakeParams(user_do.uuid, user_do.nickname, user_do.avatar, user_do.email, user_do.password_hash);

    return conn.Execute(sql, params);
  }

  [[nodiscard]] bool update_user_password(const std::string& email, const std::string& password_hash) const
  {
    auto conn = _db_pool.GetConnection();

    const char* sql = "UPDATE users SET password_hash = ? WHERE email = ?";
    auto params = utils::MakeParams(password_hash, email);

    return conn.Execute(sql, params);
  }

  [[nodiscard]] std::pair<std::string, std::string> get_uid_pass_by_user(const std::string& user, bool is_email) const
  {
    auto conn = _db_pool.GetConnection();

    std::string sql;
    if (is_email)
    {
      sql = "SELECT uuid, password_hash FROM users WHERE email = ?";
    }
    else
    {
      sql = "SELECT uuid, password_hash FROM users WHERE nickname = ?";
    }

    auto params = utils::MakeParams(user);

    utils::StringBuffer<36> uuid;
    utils::StringBuffer<255> password_hash;
    auto results = utils::MakeResults(uuid, password_hash);

    if (!conn.QueryOne(sql.c_str(), params, results))
    {
      return {};
    }

    return {uuid.str(), password_hash.str()};
  }

  // [[nodiscard]] std::vector<UserVerifyCodeDO> FindAllByEmail(const std::string& email)
  // {
  //   void(this);

  //   auto conn = utils::DBPool::GetInstance().GetConnection();

  //   const char* sql = "SELECT id, email, code, purpose, created_at FROM user_verification_codes WHERE email = ?";
  //   auto params = utils::MakeParams(email);

  //   std::int64_t db_id{};
  //   utils::StringBuffer<256> db_email_buf;
  //   utils::StringBuffer<32> db_code_buf;
  //   std::int8_t db_purpose{};
  //   utils::StringBuffer<256> db_created_at;

  //   auto results = utils::MakeResults(db_id, db_email_buf, db_code_buf, db_purpose, db_created_at);

  //   std::vector<UserVerifyCodeDO> list;
  //   conn.QueryMany(sql, params, results,
  //                  [&]()
  //                  {
  //                    list.push_back({.id = db_id,
  //                                    .email = db_email_buf.str(),
  //                                    .code = db_code_buf.str(),
  //                                    .purpose = db_purpose,
  //                                    .created_at = std::chrono::system_clock::time_point{}});
  //                  });

  //   return list;
  // }

  _impl() = default;
  ~_impl() = default;
};

UserRepository::UserRepository() : _pimpl(std::make_unique<_impl>())
{
}

UserRepository::~UserRepository() = default;

UserRepository& UserRepository::GetInstance()
{
  static UserRepository instance;
  return instance;
}

bool UserRepository::InsertVerifyCode(const UserVerifyCodeDO& user_verify_code_do) const
{
  return _pimpl->insert_verify_code(user_verify_code_do);
}

std::expected<bool, std::string> UserRepository::CheckUserExists(const std::string& email,
                                                                 const std::string& nickname) const
{
  return _pimpl->check_user_exists(email, nickname);
}

bool UserRepository::InsertUser(const UserDO& user_do) const
{
  return _pimpl->insert_user(user_do);
}

bool UserRepository::UpdateUserPassword(const std::string& email, const std::string& password_hash) const
{
  return _pimpl->update_user_password(email, password_hash);
}

std::pair<std::string, std::string> UserRepository::GetUidPassByUser(const std::string& user, bool is_email) const
{
  return _pimpl->get_uid_pass_by_user(user, is_email);
}

}  // namespace core