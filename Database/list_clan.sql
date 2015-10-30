/*
MySQL Data Transfer
Source Host: 192.168.1.100
Source Database: osrose
Target Host: 192.168.1.100
Target Database: osrose
Date: 11/15/2007 7:54:14 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_clan
-- ----------------------------
CREATE TABLE `list_clan` (
  `id` int(11) NOT NULL auto_increment,
  `logo` int(11) default NULL,
  `back` int(11) default NULL,
  `name` varchar(50) default NULL,
  `cp` int(11) default NULL,
  `grade` int(11) default NULL,
  `slogan` varchar(100) default NULL,
  `news` varchar(200) default NULL,
  `rankingpoints` int(11) default NULL,
  `siglogo` int(11) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=99 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
