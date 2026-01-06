/******************************************************************************
 *
 * @file       grpc_error.hpp
 * @brief      grpc错误格式
 *
 * @author     KBchulan
 * @date       2025/12/15
 * @history
 ******************************************************************************/

#ifndef GRPC_ERROR_HPP
#define GRPC_ERROR_HPP

#include <grpcpp/support/status.h>

#include <utils/UtilsExport.hpp>

namespace utils
{

struct UTILS_EXPORT GrpcError
{
  grpc::StatusCode code;
  std::string message;
};

}  // namespace utils

#endif  // GRPC_ERROR_HPP