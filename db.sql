CREATE DATABASE IF NOT EXISTS chatroom CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

USE chatroom;

-- 验证码表
CREATE TABLE IF NOT EXISTS `user_verification_codes` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `email` VARCHAR(255) NOT NULL COMMENT '用户邮箱',
  `code` VARCHAR(16) NOT NULL COMMENT '验证码',
  `purpose` TINYINT NOT NULL COMMENT '1=register, 2=reset_password, 发送验证码目的',
  `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '验证码创建时间',

  PRIMARY KEY (`id`),
  INDEX `idx_email_purpose_created_at` (`email`, `purpose`, `created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 用户表
CREATE TABLE IF NOT EXISTS `users` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `uuid` CHAR(36) NOT NULL UNIQUE COMMENT '每个用户自带的唯一id',
  `nickname` VARCHAR(64) NOT NULL UNIQUE COMMENT '用户昵称',
  `avatar` VARCHAR(128) NOT NULL COMMENT '用户头像',
  `email` VARCHAR(255) NOT NULL UNIQUE COMMENT '用户邮箱',
  `password_hash` VARCHAR(255) NOT NULL COMMENT 'Argon2id加密密码',
  `last_login` DATETIME DEFAULT NULL COMMENT '上次登录的时间',
  `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '账号创建时间',
  `updated_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '信息更新时间, 如修改密码等',

  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;