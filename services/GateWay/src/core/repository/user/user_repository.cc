#include "user_repository.hpp"

#include <core/domain/do/user/user_do.hpp>
#include <utils/pool/db_params.hpp>
#include <utils/pool/db_pool.hpp>

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

  // // 下面两个用于演示
  // [[nodiscard]] std::optional<UserVerifyCodeDO> find_by_email_and_purpose(const std::string& email,
  //                                                                         std::int8_t purpose) const
  // {
  //   void(this);

  //   auto conn = utils::DBPool::GetInstance().GetConnection();

  //   const char* sql =
  //       "SELECT id, email, code, purpose, created_at FROM user_verification_codes WHERE email = ? AND purpose = ? "
  //       "ORDER BY created_at DESC LIMIT 1";
  //   auto params = utils::MakeParams(email, purpose);

  //   std::int64_t db_id{};
  //   utils::StringBuffer<255> db_email;
  //   utils::StringBuffer<16> db_code;
  //   std::int8_t db_purpose{};
  //   utils::StringBuffer<255> db_created_at;

  //   auto results = utils::MakeResults(db_id, db_email, db_code, db_purpose, db_created_at);

  //   if (conn.QueryOne(sql, params, results))
  //   {
  //     return UserVerifyCodeDO{.id = db_id,
  //                             .email = db_email.str(),
  //                             .code = db_code.str(),
  //                             .purpose = db_purpose,
  //                             .created_at = std::chrono::system_clock::time_point{}};
  //   }

  //   return std::nullopt;
  // }

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

}  // namespace core