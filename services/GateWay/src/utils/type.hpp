/******************************************************************************
 *
 * @file       type.hpp
 * @brief      类型定义
 *
 * @author     KBchulan
 * @date       2025/12/02
 * @history
 ******************************************************************************/

#ifndef TYPE_HPP
#define TYPE_HPP

#include <core/domain/vo/common_vo.hpp>
#include <expected>
#include <functional>
#include <string>

using RequestHandleResult = std::expected<std::string, std::string>;
using GetHandlers = std::function<void(const std::string&, core::CommonVO&)>;

#endif  // TYPE_HPP