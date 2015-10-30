/*
MySQL Data Transfer
Source Host: localhost
Source Database: osrose
Target Host: localhost
Target Database: osrose
Date: 4/7/2009 1:35:54 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for mail_list
-- ----------------------------
CREATE TABLE `mail_list` (
  `id` int(11) NOT NULL auto_increment,
  `mailfromname` varchar(20) NOT NULL,
  `mailfromcharid` int(11) NOT NULL,
  `sendtoname` varchar(20) NOT NULL,
  `sendtocharid` int(11) NOT NULL,
  `message` text NOT NULL,
  `dhsent` decimal(20,0) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

