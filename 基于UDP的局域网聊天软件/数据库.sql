/*
Navicat MySQL Data Transfer

Source Server         : sss
Source Server Version : 50643
Source Host           : localhost:3306
Source Database       : jiwangkeshe

Target Server Type    : MYSQL
Target Server Version : 50643
File Encoding         : 65001

Date: 2019-05-10 03:37:28
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for address
-- ----------------------------
DROP TABLE IF EXISTS `address`;
CREATE TABLE `address` (
  `ip` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of address
-- ----------------------------
INSERT INTO `address` VALUES ('192.168.1.158');
INSERT INTO `address` VALUES ('192.168.2.2');
INSERT INTO `address` VALUES ('192.168.2.1');

-- ----------------------------
-- Table structure for information
-- ----------------------------
DROP TABLE IF EXISTS `information`;
CREATE TABLE `information` (
  `username` varchar(255) NOT NULL,
  `password` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of information
-- ----------------------------
INSERT INTO `information` VALUES ('zzz', 'zzz');
INSERT INTO `information` VALUES ('qqq', 'qqq');
