/*
 Navicat MySQL Dump SQL

 Source Server         : mine
 Source Server Type    : MySQL
 Source Server Version : 80044 (8.0.44)
 Source Host           : localhost:3306
 Source Schema         : banksystem

 Target Server Type    : MySQL
 Target Server Version : 80044 (8.0.44)
 File Encoding         : 65001

 Date: 08/12/2025 20:28:27
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for accounts
-- ----------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts`  (
  `account_id` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `user_id` int NOT NULL,
  `account_type` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '储蓄账户',
  `balance` decimal(15, 2) NULL DEFAULT 0.00,
  `status` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '正常',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`account_id`) USING BTREE,
  INDEX `idx_accounts_user_id`(`user_id` ASC) USING BTREE,
  CONSTRAINT `accounts_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of accounts
-- ----------------------------
INSERT INTO `accounts` VALUES ('621420230101000001', 1, '储蓄账户', 9769809.00, '正常', '2025-12-06 17:52:09');
INSERT INTO `accounts` VALUES ('621420230101000002', 2, '储蓄账户', 244191.00, '冻结', '2025-12-06 17:52:09');
INSERT INTO `accounts` VALUES ('621420230101000003', 3, '储蓄账户', 3000.00, '正常', '2025-12-06 17:52:09');
INSERT INTO `accounts` VALUES ('6214202512071526506', 1, '活期账户', 1000.00, '正常', '2025-12-07 15:26:26');
INSERT INTO `accounts` VALUES ('6214202512071526691', 1, '定期账户', 0.00, '正常', '2025-12-07 15:26:34');
INSERT INTO `accounts` VALUES ('6214202512071606905', 6, '储蓄账户', 100000.00, '正常', '2025-12-07 16:06:43');
INSERT INTO `accounts` VALUES ('6214202512071642726', 7, '储蓄账户', 0.00, '正常', '2025-12-07 16:42:34');

-- ----------------------------
-- Table structure for transactions
-- ----------------------------
DROP TABLE IF EXISTS `transactions`;
CREATE TABLE `transactions`  (
  `transaction_id` int NOT NULL AUTO_INCREMENT,
  `account_id` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `transaction_type` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `amount` decimal(15, 2) NOT NULL,
  `target_account` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `description` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `transaction_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`transaction_id`) USING BTREE,
  INDEX `idx_transactions_account_id`(`account_id` ASC) USING BTREE,
  INDEX `idx_transactions_time`(`transaction_time` DESC) USING BTREE,
  CONSTRAINT `transactions_ibfk_1` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`account_id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 15 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of transactions
-- ----------------------------
INSERT INTO `transactions` VALUES (1, '621420230101000001', '存款', 100000.00, NULL, '存款操作', '2025-12-06 20:15:17');
INSERT INTO `transactions` VALUES (2, '621420230101000001', '转账', 100000.00, '621420230101000002', '转账支出', '2025-12-07 00:45:18');
INSERT INTO `transactions` VALUES (3, '621420230101000002', '收款', 100000.00, '621420230101000001', '转账收入', '2025-12-07 00:45:18');
INSERT INTO `transactions` VALUES (4, '621420230101000001', '存款', 10000000.00, NULL, '存款操作', '2025-12-07 13:14:09');
INSERT INTO `transactions` VALUES (5, '621420230101000001', '转账', 114514.00, '621420230101000002', '转账支出', '2025-12-07 13:15:15');
INSERT INTO `transactions` VALUES (6, '621420230101000002', '收款', 114514.00, '621420230101000001', '转账收入', '2025-12-07 13:15:15');
INSERT INTO `transactions` VALUES (7, '621420230101000001', '转账', 12332.00, '621420230101000002', '转账支出', '2025-12-07 15:27:45');
INSERT INTO `transactions` VALUES (8, '621420230101000002', '收款', 12332.00, '621420230101000001', '转账收入', '2025-12-07 15:27:45');
INSERT INTO `transactions` VALUES (9, '621420230101000001', '转账', 12345.00, '621420230101000002', '转账支出', '2025-12-07 15:27:56');
INSERT INTO `transactions` VALUES (10, '621420230101000002', '收款', 12345.00, '621420230101000001', '转账收入', '2025-12-07 15:27:56');
INSERT INTO `transactions` VALUES (11, '621420230101000001', '转账', 1000.00, '6214202512071526506', '转账支出', '2025-12-07 15:29:09');
INSERT INTO `transactions` VALUES (12, '6214202512071526506', '收款', 1000.00, '621420230101000001', '转账收入', '2025-12-07 15:29:09');
INSERT INTO `transactions` VALUES (13, '621420230101000001', '转账', 100000.00, '6214202512071606905', '转账支出', '2025-12-07 16:07:40');
INSERT INTO `transactions` VALUES (14, '6214202512071606905', '收款', 100000.00, '621420230101000001', '转账收入', '2025-12-07 16:07:40');

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users`  (
  `user_id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `password` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `full_name` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `id_card` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `phone` varchar(15) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `email` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`user_id`) USING BTREE,
  UNIQUE INDEX `username`(`username` ASC) USING BTREE,
  UNIQUE INDEX `id_card`(`id_card` ASC) USING BTREE,
  INDEX `idx_users_username`(`username` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 8 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of users
-- ----------------------------
INSERT INTO `users` VALUES (1, 'admin', '123456789', '系统管理员', '110101199001011234', '13800138000', 'admin@bank.com', '2025-12-06 17:51:44');
INSERT INTO `users` VALUES (2, 'zhangsan', '123456', '张三', '110101199001011235', '13800138001', 'zhangsan@email.com', '2025-12-06 17:51:44');
INSERT INTO `users` VALUES (3, 'lisi', '123456', '李四', '110101199001011236', '13800138002', 'lisi@email.com', '2025-12-06 17:51:44');
INSERT INTO `users` VALUES (4, 'lehee', '123456', 'ikun', '12345678890098', '13867462365', 'lehe@bank.com', '2025-12-07 13:17:32');
INSERT INTO `users` VALUES (5, 'eternal', 'mimaismima', 'hope', '12323245546778964568', '17627349859095', 'imissyou@email.com', '2025-12-07 16:02:26');
INSERT INTO `users` VALUES (6, 'jjs', '123456789', 'jjs', '12334567894141241513', '1241524642646', 'fafee@email.com', '2025-12-07 16:06:16');
INSERT INTO `users` VALUES (7, 'jiji', '114514', 'tp', '12343243567890877656', '1223456789087', 'gggd@out.com', '2025-12-07 16:42:19');

SET FOREIGN_KEY_CHECKS = 1;
