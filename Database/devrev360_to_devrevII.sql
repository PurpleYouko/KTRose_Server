
-- Update script to update from "dev rev 360" to "dev rev II" --
-- version 1.0.2 --
-- osRose dev team. --
-- http://forum.dev-osrose.com --


-- Basic skills --
ALTER TABLE `characters` CHANGE `basic_skills` `basic_skills` VARCHAR( 255 ) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '11,12,13,14,15,16,17,18,19,20,25,26,27,28,29,30,31,32,33,50,101';
ALTER TABLE `characters` ADD `bonusddrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --
ALTER TABLE `characters` ADD `timerddrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --
ALTER TABLE `characters` ADD `bonusstatdrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --
ALTER TABLE `characters` ADD `timerstatdrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --
ALTER TABLE `characters` ADD `bonusgraydrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --
ALTER TABLE `characters` ADD `timergraydrop` INT( 11 ) NOT NULL DEFAULT '0';	-- used for medals --


-- List Config --
ALTER TABLE `list_config` ADD `pc_drop_zuly` INT( 11 ) NOT NULL DEFAULT '30';	-- % Zuly drop --
ALTER TABLE `list_config` ADD `drop_rev` INT( 11 ) NOT NULL DEFAULT '1';	-- New AND drop system --


-- List Npcs --
ALTER TABLE `list_npcs` ADD `extra_param` INT( 11 ) NOT NULL DEFAULT '0';	-- used for NPCs, events and so on... --


-- Wishlist --
ALTER TABLE `wishlist` CHANGE `itemowner` `itemowner` INT( 11 ) NULL DEFAULT '0';
ALTER TABLE `wishlist` CHANGE `itemhead` `itemhead` BIGINT( 11 ) NULL DEFAULT '0';
ALTER TABLE `wishlist` CHANGE `itemdata` `itemhead` BIGINT( 11 ) NULL DEFAULT '0';
ALTER TABLE `wishlist` ADD UNIQUE `ow_slot` ( `itemowner` , `slot` );


-- Mail list --
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
