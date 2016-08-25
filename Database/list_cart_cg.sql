/*
MySQL Data Transfer
Source Host: localhost
Source Database: osrose
Target Host: localhost
Target Database: osrose
Date: 2/25/2009 8:11:45 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for list_cart_cg
-- ----------------------------
CREATE TABLE `list_cart_cg` (
  `id` int(11) NOT NULL auto_increment,
  `itemnumber` int(11) NOT NULL,
  `itemtype` int(11) NOT NULL,
  `Cart_CG` varchar(4) NOT NULL,
  `isactive` int(11) NOT NULL default '1',
  `description` varchar(50) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=172 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `list_cart_cg` VALUES ('1', '1', '14', 'Cart', '0', 'Wooden Frame');
INSERT INTO `list_cart_cg` VALUES ('2', '2', '14', 'Cart', '0', 'Wooden Carbon Frame');
INSERT INTO `list_cart_cg` VALUES ('3', '3', '14', 'Cart', '0', 'Colmandy Frame');
INSERT INTO `list_cart_cg` VALUES ('4', '4', '14', 'Cart', '0', 'Colmanju Frame');
INSERT INTO `list_cart_cg` VALUES ('5', '5', '14', 'Cart', '1', 'Sporty Frame');
INSERT INTO `list_cart_cg` VALUES ('6', '6', '14', 'Cart', '0', 'Sporty R Frame');
INSERT INTO `list_cart_cg` VALUES ('7', '7', '14', 'Cart', '0', 'Racing Frame Gray');
INSERT INTO `list_cart_cg` VALUES ('8', '8', '14', 'Cart', '0', 'Racing Frame Orange');
INSERT INTO `list_cart_cg` VALUES ('9', '9', '14', 'Cart', '0', 'Racing Frame Red');
INSERT INTO `list_cart_cg` VALUES ('10', '10', '14', 'Cart', '0', 'Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('11', '21', '14', 'Cart', '0', 'P.I.G. Frame');
INSERT INTO `list_cart_cg` VALUES ('12', '31', '14', 'CG', '0', 'Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('13', '32', '14', 'CG', '1', 'Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('14', '33', '14', 'CG', '0', 'Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('15', '34', '14', 'CG', '0', 'Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('16', '35', '14', 'CG', '0', 'Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('17', '41', '14', 'CG', '0', 'Black Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('18', '42', '14', 'CG', '0', 'Blue Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('19', '43', '14', 'CG', '0', 'Green Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('20', '44', '14', 'CG', '0', 'Pink Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('21', '45', '14', 'CG', '0', 'Red Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('22', '51', '14', 'CG', '0', 'Black Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('23', '52', '14', 'CG', '0', 'Blue Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('24', '53', '14', 'CG', '0', 'Green Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('25', '54', '14', 'CG', '0', 'Pink Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('26', '55', '14', 'CG', '0', 'Red Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('27', '61', '14', 'CG', '0', 'Black Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('28', '62', '14', 'CG', '0', 'Blue Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('29', '63', '14', 'CG', '0', 'Green Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('30', '64', '14', 'CG', '0', 'Pink Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('31', '65', '14', 'CG', '0', 'Red Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('32', '71', '14', 'CG', '0', 'Black Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('33', '72', '14', 'CG', '0', 'Blue Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('34', '73', '14', 'CG', '0', 'Green Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('35', '74', '14', 'CG', '0', 'Pink Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('36', '75', '14', 'CG', '0', 'Red Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('37', '81', '14', 'CG', '0', 'Black Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('38', '82', '14', 'CG', '0', 'Blue Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('39', '83', '14', 'CG', '0', 'Green Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('40', '84', '14', 'CG', '0', 'Pink Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('41', '85', '14', 'CG', '0', 'Red Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('42', '91', '14', 'Cart', '0', 'GM Cart');
INSERT INTO `list_cart_cg` VALUES ('43', '101', '14', 'Cart', '0', 'First Engine');
INSERT INTO `list_cart_cg` VALUES ('44', '102', '14', 'Cart', '0', 'Second Engine');
INSERT INTO `list_cart_cg` VALUES ('45', '103', '14', 'Cart', '1', 'Dural Engine');
INSERT INTO `list_cart_cg` VALUES ('46', '104', '14', 'Cart', '0', 'Chase Engine');
INSERT INTO `list_cart_cg` VALUES ('47', '105', '14', 'Cart', '0', 'Tricky Engine');
INSERT INTO `list_cart_cg` VALUES ('48', '106', '14', 'Cart', '0', 'Jet Engine');
INSERT INTO `list_cart_cg` VALUES ('49', '121', '14', 'Cart', '0', 'P.I.G. Engine');
INSERT INTO `list_cart_cg` VALUES ('50', '131', '14', 'CG', '0', 'Fornef Engine');
INSERT INTO `list_cart_cg` VALUES ('51', '132', '14', 'CG', '1', 'Sheft Engine');
INSERT INTO `list_cart_cg` VALUES ('52', '133', '14', 'Cart', '0', 'Luxem Engine');
INSERT INTO `list_cart_cg` VALUES ('53', '201', '14', 'Cart', '0', 'Wooden Wheels');
INSERT INTO `list_cart_cg` VALUES ('54', '202', '14', 'Cart', '0', 'Carbon Wheels');
INSERT INTO `list_cart_cg` VALUES ('55', '203', '14', 'Cart', '1', 'Rapid Wheels');
INSERT INTO `list_cart_cg` VALUES ('56', '221', '14', 'Cart', '0', 'P.I.G. Wheels');
INSERT INTO `list_cart_cg` VALUES ('57', '231', '14', 'CG', '0', 'Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('58', '232', '14', 'CG', '1', 'Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('59', '233', '14', 'CG', '0', 'Prime Leg');
INSERT INTO `list_cart_cg` VALUES ('60', '241', '14', 'CG', '0', 'Black Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('61', '242', '14', 'CG', '0', 'Blue Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('62', '243', '14', 'CG', '0', 'Green Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('63', '244', '14', 'CG', '0', 'Pink Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('64', '245', '14', 'CG', '0', 'Red Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('65', '251', '14', 'CG', '0', 'Black Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('66', '252', '14', 'CG', '0', 'Blue Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('67', '253', '14', 'CG', '0', 'Green Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('68', '254', '14', 'CG', '0', 'Pink Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('69', '255', '14', 'CG', '0', 'Red Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('70', '301', '14', 'Cart', '0', 'Spare Tire B');
INSERT INTO `list_cart_cg` VALUES ('71', '302', '14', 'Cart', '0', 'Small Rear Carrier');
INSERT INTO `list_cart_cg` VALUES ('72', '303', '14', 'Cart', '0', 'Add On Chair');
INSERT INTO `list_cart_cg` VALUES ('73', '311', '14', 'Cart', '0', 'Battle Machine Gun');
INSERT INTO `list_cart_cg` VALUES ('74', '312', '14', 'Cart', '0', 'Battle Launcher');
INSERT INTO `list_cart_cg` VALUES ('75', '321', '14', 'Cart', '0', 'P.I.G Balloon');
INSERT INTO `list_cart_cg` VALUES ('76', '331', '14', 'CG', '0', 'Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('77', '332', '14', 'CG', '0', 'Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('78', '333', '14', 'CG', '0', 'Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('79', '334', '14', 'CG', '1', 'Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('80', '335', '14', 'CG', '0', 'Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('81', '336', '14', 'CG', '0', 'Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('82', '401', '14', 'Cart', '0', 'Spike Wheel');
INSERT INTO `list_cart_cg` VALUES ('83', '402', '14', 'Cart', '0', 'Speed Wing');
INSERT INTO `list_cart_cg` VALUES ('84', '403', '14', 'Cart', '0', 'Wind Booster');
INSERT INTO `list_cart_cg` VALUES ('85', '404', '14', 'Cart', '1', 'Jet Booster');
INSERT INTO `list_cart_cg` VALUES ('86', '405', '14', 'Cart', '0', 'Sun Roof');
INSERT INTO `list_cart_cg` VALUES ('87', '406', '14', 'Cart', '0', 'Coal Mine Moldie Sun Roof');
INSERT INTO `list_cart_cg` VALUES ('88', '407', '14', 'Cart', '0', 'Coal Mine Moldie Sun Roof P');
INSERT INTO `list_cart_cg` VALUES ('89', '420', '14', 'Cart', '0', 'Original Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('90', '421', '14', 'Cart', '0', 'White Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('91', '422', '14', 'Cart', '0', 'Red Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('92', '423', '14', 'Cart', '0', 'Black Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('93', '424', '14', 'Cart', '0', 'Yellow Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('94', '425', '14', 'Cart', '0', 'Pink Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('95', '426', '14', 'Cart', '0', 'Blue Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('96', '427', '14', 'Cart', '0', 'Gray Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('97', '428', '14', 'Cart', '0', 'Green Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('98', '429', '14', 'Cart', '0', 'Flame Meister Frame');
INSERT INTO `list_cart_cg` VALUES ('99', '451', '14', 'Cart', '0', 'Chaser Engine');
INSERT INTO `list_cart_cg` VALUES ('100', '452', '14', 'Cart', '0', 'Tuned Chaser Engine');
INSERT INTO `list_cart_cg` VALUES ('101', '471', '14', 'Cart', '0', 'Meister Wheels');
INSERT INTO `list_cart_cg` VALUES ('102', '500', '14', 'CG', '0', 'Wooden Stock Core');
INSERT INTO `list_cart_cg` VALUES ('103', '501', '14', 'CG', '0', 'Reinforced Stock Core');
INSERT INTO `list_cart_cg` VALUES ('104', '502', '14', 'CG', '0', 'Vajalak Core');
INSERT INTO `list_cart_cg` VALUES ('105', '503', '14', 'CG', '0', 'Kilenju Core');
INSERT INTO `list_cart_cg` VALUES ('106', '504', '14', 'CG', '0', 'Saintner Core');
INSERT INTO `list_cart_cg` VALUES ('107', '510', '14', 'CG', '0', 'Stand Leg');
INSERT INTO `list_cart_cg` VALUES ('108', '511', '14', 'CG', '0', 'Runihol Leg');
INSERT INTO `list_cart_cg` VALUES ('109', '512', '14', 'CG', '0', 'Prime Leg');
INSERT INTO `list_cart_cg` VALUES ('110', '520', '14', 'CG', '0', 'Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('111', '521', '14', 'CG', '0', 'Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('112', '522', '14', 'CG', '0', 'Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('113', '523', '14', 'CG', '0', 'Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('114', '524', '14', 'CG', '0', 'Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('115', '525', '14', 'CG', '0', 'Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('116', '600', '14', 'Cart', '0', 'Wooden Backseat');
INSERT INTO `list_cart_cg` VALUES ('117', '601', '14', 'Cart', '1', 'Steel Backseat');
INSERT INTO `list_cart_cg` VALUES ('118', '700', '14', 'Cart', '0', 'Stone Hammer');
INSERT INTO `list_cart_cg` VALUES ('119', '701', '14', 'Cart', '0', 'Doom Hammer');
INSERT INTO `list_cart_cg` VALUES ('120', '702', '14', 'Cart', '0', 'Battle Bow');
INSERT INTO `list_cart_cg` VALUES ('121', '703', '14', 'Cart', '0', 'Tarkion Bow');
INSERT INTO `list_cart_cg` VALUES ('122', '704', '14', 'Cart', '0', 'Battle Katar');
INSERT INTO `list_cart_cg` VALUES ('123', '705', '14', 'Cart', '0', 'Battle Spear');
INSERT INTO `list_cart_cg` VALUES ('124', '706', '14', 'Cart', '0', 'Battle Mirror');
INSERT INTO `list_cart_cg` VALUES ('125', '707', '14', 'Cart', '0', 'Battle Crystal');
INSERT INTO `list_cart_cg` VALUES ('126', '708', '14', 'Cart', '0', 'Battle Machine Gun');
INSERT INTO `list_cart_cg` VALUES ('127', '709', '14', 'Cart', '0', 'Battle Launcher');
INSERT INTO `list_cart_cg` VALUES ('128', '800', '14', 'Cart', '0', 'Experimental Stone Hammer');
INSERT INTO `list_cart_cg` VALUES ('129', '801', '14', 'Cart', '0', 'Experimental Doom Hammer');
INSERT INTO `list_cart_cg` VALUES ('130', '802', '14', 'Cart', '0', 'Experimental Battle Bow');
INSERT INTO `list_cart_cg` VALUES ('131', '803', '14', 'Cart', '0', 'Experimental Tarkion Bow');
INSERT INTO `list_cart_cg` VALUES ('132', '804', '14', 'Cart', '0', 'Experimental Battle Katar');
INSERT INTO `list_cart_cg` VALUES ('133', '805', '14', 'Cart', '0', 'Experimental Battle Spear');
INSERT INTO `list_cart_cg` VALUES ('134', '806', '14', 'Cart', '0', 'Experimental Battle Mirror');
INSERT INTO `list_cart_cg` VALUES ('135', '807', '14', 'Cart', '0', 'Experimental Battle Crystal');
INSERT INTO `list_cart_cg` VALUES ('136', '808', '14', 'Cart', '0', 'Experimental Battle Machine Gun');
INSERT INTO `list_cart_cg` VALUES ('137', '809', '14', 'Cart', '0', 'Experimental Battle Launcher');
INSERT INTO `list_cart_cg` VALUES ('138', '811', '14', 'CG', '0', 'Black Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('139', '812', '14', 'CG', '0', 'Blue Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('140', '813', '14', 'CG', '0', 'Green Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('141', '814', '14', 'CG', '0', 'Pink Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('142', '815', '14', 'CG', '0', 'Red Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('143', '821', '14', 'CG', '0', 'Black Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('144', '822', '14', 'CG', '0', 'Blue Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('145', '823', '14', 'CG', '0', 'Green Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('146', '824', '14', 'CG', '0', 'Pink Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('147', '825', '14', 'CG', '0', 'Red Reinforced Punch Arms');
INSERT INTO `list_cart_cg` VALUES ('148', '831', '14', 'CG', '0', 'Black Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('149', '832', '14', 'CG', '0', 'Blue Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('150', '833', '14', 'CG', '0', 'Green Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('151', '834', '14', 'CG', '0', 'Pink Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('152', '835', '14', 'CG', '0', 'Red Drill Arms');
INSERT INTO `list_cart_cg` VALUES ('153', '841', '14', 'CG', '0', 'Black Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('154', '842', '14', 'CG', '0', 'Blue Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('155', '843', '14', 'CG', '0', 'Green Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('156', '844', '14', 'CG', '0', 'Pink Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('157', '845', '14', 'CG', '0', 'Red Sword Wheel Arms');
INSERT INTO `list_cart_cg` VALUES ('158', '851', '14', 'CG', '0', 'Black Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('159', '852', '14', 'CG', '0', 'Blue Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('160', '853', '14', 'CG', '0', 'Green Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('161', '854', '14', 'CG', '0', 'Pink Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('162', '855', '14', 'CG', '0', 'Red Cannon Arms');
INSERT INTO `list_cart_cg` VALUES ('163', '861', '14', 'CG', '0', 'Black Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('164', '862', '14', 'CG', '0', 'Blue Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('165', '863', '14', 'CG', '0', 'Green Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('166', '864', '14', 'CG', '0', 'Pink Spear Arms');
INSERT INTO `list_cart_cg` VALUES ('167', '865', '14', 'CG', '0', 'Red Spear Arms');
