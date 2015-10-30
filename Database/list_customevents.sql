/*
MySQL Data Transfer
Source Host: 192.168.1.100
Source Database: osrose
Target Host: 192.168.1.100
Target Database: osrose
Date: 1/13/2008 8:28:04 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_customevents
-- ----------------------------
CREATE TABLE `list_customevents` (
  `id` int(11) NOT NULL auto_increment,
  `eventtype` int(11) NOT NULL default '0',
  `npcname` varchar(50) collate latin1_general_ci NOT NULL,
  `x` int(11) NOT NULL default '5000',
  `y` int(11) NOT NULL default '5000',
  `map` int(11) NOT NULL default '1',
  `radius` int(11) NOT NULL default '2',
  `active` tinyint(4) NOT NULL default '0',
  `pc1` int(11) NOT NULL default '0',
  `pc2` int(11) NOT NULL default '0',
  `pc3` int(11) NOT NULL default '0',
  `pc4` int(11) NOT NULL default '0',
  `pc5` int(11) NOT NULL default '0',
  `pc6` int(11) NOT NULL default '0',
  `pc7` int(11) NOT NULL default '0',
  `pc8` int(11) NOT NULL default '0',
  `pc9` int(11) NOT NULL default '0',
  `pc10` int(11) NOT NULL default '0',
  `pt1` int(11) NOT NULL default '0',
  `pt2` int(11) NOT NULL default '0',
  `pt3` int(11) NOT NULL default '0',
  `pt4` int(11) NOT NULL default '0',
  `pt5` int(11) NOT NULL default '0',
  `pt6` int(11) NOT NULL default '0',
  `pt7` int(11) NOT NULL default '0',
  `pt8` int(11) NOT NULL default '0',
  `pt9` int(11) NOT NULL default '0',
  `pt10` int(11) NOT NULL default '0',
  `pi1` int(11) NOT NULL default '0',
  `pi2` int(11) NOT NULL default '0',
  `pi3` int(11) NOT NULL default '0',
  `pi4` int(11) NOT NULL default '0',
  `pi5` int(11) NOT NULL default '0',
  `pi6` int(11) NOT NULL default '0',
  `pi7` int(11) NOT NULL default '0',
  `pi8` int(11) NOT NULL default '0',
  `pi9` int(11) NOT NULL default '0',
  `pi10` int(11) NOT NULL default '0',
  `pn1` varchar(50) collate latin1_general_ci NOT NULL,
  `pn2` varchar(50) collate latin1_general_ci NOT NULL,
  `pn3` varchar(50) collate latin1_general_ci NOT NULL,
  `pn4` varchar(50) collate latin1_general_ci NOT NULL,
  `pn5` varchar(50) collate latin1_general_ci NOT NULL,
  `pn6` varchar(50) collate latin1_general_ci NOT NULL,
  `pn7` varchar(50) collate latin1_general_ci NOT NULL,
  `pn8` varchar(50) collate latin1_general_ci NOT NULL,
  `pn9` varchar(50) collate latin1_general_ci NOT NULL,
  `pn10` varchar(50) collate latin1_general_ci NOT NULL,
  `script1` varchar(250) collate latin1_general_ci NOT NULL,
  `script2` varchar(250) collate latin1_general_ci NOT NULL,
  `script3` varchar(200) collate latin1_general_ci NOT NULL,
  `script4` varchar(200) collate latin1_general_ci NOT NULL,
  `itemname` varchar(50) collate latin1_general_ci NOT NULL,
  `collecttype` int(11) NOT NULL default '0',
  `collectnum` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `list_customevents` VALUES ('1', '0', 'Judith', '5112', '5322', '37', '3', '0', '150', '250', '350', '450', '550', '650', '750', '900', '0', '0', '5', '2', '2', '3', '3', '3', '1', '9', '0', '0', '157', '176', '156', '155', '156', '157', '36', '35', '0', '0', 'Flip Flops', 'Red Swimming Cap', 'Snorkel', 'Swim Suit', 'Bikini', 'Beachwear', 'Swimming Goggles', 'Surfboard', '', '', 'Trash monsters have invaded our beaches. Please help me to clean up by collecting the trash that they drop.', 'Choose one of these items as your prize. type /prize X (where X is the iem number in the list)', '', '', 'trash', '12', '407', '0');
INSERT INTO `list_customevents` VALUES ('3', '0', 'Judy', '5262', '5212', '1', '2', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '', '', '', '', '', '', '', '', 'A new portal has been discovered. Please go there and help Judith. The entrance can be found in a waterfall just outside of Adventurer Plains village', '', '', '', '', '0', '0', '0');
INSERT INTO `list_customevents` VALUES ('4', '0', 'Santa', '5073', '5362', '38', '2', '1', '100', '150', '200', '250', '300', '350', '400', '450', '0', '0', '2', '3', '4', '5', '2', '3', '4', '5', '0', '0', '152', '152', '152', '152', '168', '164', '164', '164', '0', '0', 'Santa Hat', 'Santa Sweater', 'Santa Gloves', 'Santa Boots', 'White Santa Hat', 'White Santa Sweater', 'White Santa Gloves', 'White Santa Boots', '', '', 'Please help me. The Rudolph Santas have stolen all the Santa Lottery cards. Without it i can\'t run my Christmas Raffle. Help me get them back please.', 'Choose one of these items as your prize. type /prize X (where X is the item number in the list)', '', '', 'lottery tickets', '12', '403', '0');
INSERT INTO `list_customevents` VALUES ('5', '0', 'Judy', '5262', '5212', '1', '2', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '', '', '', '', '', '', '', '', 'Santa is in trouble!! BlackSheep has pursuaded the Rudolph Santas to steal all the Lottery Tickets. Please help. There is a portal behind Trytehs shop.', '', '', '', '', '0', '0', '0');
