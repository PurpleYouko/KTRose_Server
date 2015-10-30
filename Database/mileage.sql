-- ----------------------------
-- Table structure for mileage
-- ----------------------------
CREATE TABLE `mileage` (
  `owner` int(11) NOT NULL default '0',
  `itemnum` int(11) NOT NULL default '0',
  `itemtype` int(11) NOT NULL default '0',
  `refine` int(11) NOT NULL default '0',
  `durability` int(11) NOT NULL default '40',
  `lifespan` int(11) NOT NULL default '100',
  `slotnum` int(11) NOT NULL default '0',
  `count` int(11) NOT NULL default '1',
  `stats` int(11) NOT NULL default '0',
  `socketed` int(11) NOT NULL default '0',
  `appraised` int(11) NOT NULL default '0',
  `gem` int(11) NOT NULL default '0',
  `sp_value` int(11) NOT NULL default '0',
  UNIQUE KEY `ow_slot` (`owner`,`slotnum`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
