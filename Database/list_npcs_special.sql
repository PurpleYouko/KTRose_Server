-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Mar 14, 2018 at 01:04 PM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `list_npcs_special`
-- 

DROP TABLE IF EXISTS `list_npcs_special`;
CREATE TABLE IF NOT EXISTS `list_npcs_special` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL,
  `type` int(11) NOT NULL,
  `map` int(11) NOT NULL,
  `dir` float NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `dialogid` int(11) NOT NULL default '0',
  `eventid` int(11) NOT NULL default '0',
  `tempdialogid` int(11) NOT NULL default '0',
  `isactive` int(11) NOT NULL,
  `whatisit` varchar(20) default NULL,
  `buffbot` tinyint(4) NOT NULL default '0',
  `buffpower` int(11) NOT NULL default '1000',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1047 ;

-- 
-- Dumping data for table `list_npcs_special`
-- 

INSERT INTO `list_npcs_special` (`id`, `name`, `type`, `map`, `dir`, `x`, `y`, `dialogid`, `eventid`, `tempdialogid`, `isactive`, `whatisit`, `buffbot`, `buffpower`) VALUES 
(1, 'Banner - Happy Halloween', 1198, 1, 182.002, 5232.64, 5309.68, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(2, 'Banner - Happy Halloween', 1198, 1, 0, 5265.23, 5069.55, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(3, '[Righteous Crusader] Norg', 3023, 2, 95.984, 5325.12, 5350, 228, 0, 0, 0, 'Halloween-2008', 0, 0),
(4, '[Knight of Junon Order] Sammodell', 3021, 2, 275.984, 5328.78, 5131.26, 226, 0, 0, 0, 'Halloween-2008', 0, 0),
(5, 'Banner - Happy Halloween', 1198, 2, 180.5, 5515.56, 5418.65, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(6, 'Banner - Happy Halloween', 1198, 2, 0, 5514.99, 5389.59, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(7, '[Ferrell Bank Guard] Arnold', 3024, 2, 264.016, 5721.48, 5364.74, 229, 0, 0, 0, 'Halloween-2008', 0, 0),
(8, 'Banner - Happy Halloween', 1198, 2, 1, 5706.4, 5264.86, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(9, '[Arumic Mercenary] Turenth', 3022, 2, 204.237, 5730.21, 5087.81, 227, 0, 0, 0, 'Halloween-2008', 0, 0),
(10, '[Mysterious Traveler] Edeline', 1200, 22, 160.035, 5175.09, 5020.77, 144, 0, 0, 0, 'Halloween-2008', 0, 0),
(11, 'Banner - Happy Halloween', 1198, 22, 148.307, 5280.66, 5160.09, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(12, 'Banner - Beware!', 1199, 25, 337.957, 5714.99, 5405.6, 0, 0, 0, 0, 'Halloween-2008', 0, 0),
(13, '[Ghost] Harry', 1157, 25, 342.011, 5506.64, 5554.8, 257, 0, 0, 0, 'Halloween-2008', 0, 0),
(14, '[Teleporter] Selladin', 1017, 1, 162.011, 5247.24, 5222.58, 0, 0, 0, 0, 'Pre-Evo-Teleporter', 0, 0),
(15, '[Teleporter] Idiosel', 1119, 2, 270, 5343.67, 5177.36, 0, 0, 0, 0, 'Pre-Evo-Teleporter', 0, 0),
(16, '[Evil Reindeer] Bob', 1490, 38, 97.961, 5065.61, 5206.88, 318, 0, 0, 0, 'Christmas-2008', 0, 0),
(17, '[Traveler] Garlen', 1491, 2, 153.435, 5463, 5167.57, 325, 0, 0, 0, 'Special Skill Reset ', 0, 0),
(18, 'NPC1', 3040, 2, 0, 4829.88, 4645.83, 306, 0, 0, 0, 'Christmas-2008-Bob-P', 0, 0),
(19, 'NPC2', 3041, 2, 0, 4833.84, 4645.52, 325, 0, 0, 0, 'Christmas-2008-Bob-P', 0, 0),
(20, '[Kuro-Tejina Staff] Buff Fairy', 1046, 1, 275.986, 5246, 5283, 0, 0, 0, 1, 'Buff Bot', 1, 3000);
