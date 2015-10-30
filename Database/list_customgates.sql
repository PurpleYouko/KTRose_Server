/*
MySQL Data Transfer
Source Host: osrose
Source Database: osrose
Target Host: osrose
Target Database: osrose
Date: 3/13/2008 8:13:28 AM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_customgates
-- ----------------------------
CREATE TABLE `list_customgates` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(255) collate latin1_general_ci NOT NULL,
  `sourcemap` int(11) NOT NULL,
  `sourcex` float NOT NULL,
  `sourcey` float NOT NULL,
  `destmap` int(11) NOT NULL,
  `destx` float NOT NULL,
  `desty` float NOT NULL,
  `radius` int(11) NOT NULL default '2',
  `active` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `list_customgates` VALUES ('1', 'Goblin cave B1 TP zone to Sunshine Coast', '31', '5027', '5509', '37', '5117', '5303', '3', '0');
INSERT INTO `list_customgates` VALUES ('3', 'Zant to Fall Ball in Gorge of Silence', '1', '5196', '5091', '28', '5570', '4798', '2', '0');
INSERT INTO `list_customgates` VALUES ('2', 'AP Waterfall to Sunshine Coast', '22', '5022', '5182', '37', '5117', '5303', '2', '0');
INSERT INTO `list_customgates` VALUES ('4', 'Zant to Santas Planetoid', '1', '5329', '5162', '38', '5072', '5345', '3', '1');
/*
INSERT INTO `list_customgates` VALUES ('5', 'Crystal Snowfields to Luna Clan Field', '54', '5097', '4229', '59', '5095', '5128', '2', '0');
*/
INSERT INTO `list_customgates` VALUES ('6', 'JP to Grand Ballroom', '2', '5566', '5179', '40', '5184', '5211', '2', '1');
