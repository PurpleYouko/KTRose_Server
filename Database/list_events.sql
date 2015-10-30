 
SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_events
-- ----------------------------
CREATE TABLE `list_events` (
  `name` varchar(255) NOT NULL,
  `type` int(11) NOT NULL,
  `dialogid` int(11) NOT NULL default '0',
  `eventid` int(11) NOT NULL default '0',
  `Active` int(11) NOT NULL default '0',
  `activekeyword` varchar(20) NOT NULL,
  `eventdescription` varchar(50) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
 
-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `list_events` VALUES ('[Cherry Berry Pie Crush] Anya', '1176', '76', '3', '0', 'winter2010', 'Winter Event 2010');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '1', '0', '', 'Old event-New Year (Hanbok)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '2', '0', '', 'old event-February (sockets)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '3', '0', '', 'old event-Spring (Cherry Blossoms)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '22', '0', 'winter2010', 'Winter Event 2010');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '101', '0', '', 'old event-Summer 01 (trash)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '102', '0', '', 'old event-Summer 02 (ghost hunt)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '103', '0', '', 'old event-Summer 03 (fireworks)');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '304', '0', 'xmas2009', 'Christmas Event 2009');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '307', '0', 'easter2009', 'Easter 2009');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '308', '0', 'worldcup1996', 'World Cup 1996');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '310', '0', '', 'Raffle Event Coupon?');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '313', '0', 'newyears2007', 'New Years 2007 - Lucky Pig');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '314', '0', 'newyears2007', 'New Years 2007 - Lucky Pig');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '315', '0', 'valentines2010', 'Valentines Day');
INSERT INTO `list_events` VALUES ('[Event Info] Judy', '1201', '305', '317', '0', 'stpatricks2009', 'St. Patricks Day');
INSERT INTO `list_events` VALUES ('[Event Info] Felice Fete', '1202', '306', '300', '0', '', 'Grand Ballroom Event?');
INSERT INTO `list_events` VALUES ('[Event Info] Felice Fete', '1202', '306', '301', '0', '', 'Grand Ballroom Event?');
INSERT INTO `list_events` VALUES ('[Event Info] Felice Fete', '1202', '306', '302', '0', '', 'Grand Ballroom Event?');
INSERT INTO `list_events` VALUES ('[Event Info] Felice Fete', '1202', '306', '303', '0', '', 'Grand Ballroom Event?');
INSERT INTO `list_events` VALUES ('[Event Info] Lucielle Fete', '1203', '307', '303', '0', '', 'Grand Ballroom Event?');
INSERT INTO `list_events` VALUES ('[Event Info] Santa Claus', '1205', '309', '304', '0', '', 'Christmas');
