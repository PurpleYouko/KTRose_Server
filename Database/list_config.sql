-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Nov 05, 2015 at 11:10 AM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `list_config`
-- 

DROP TABLE IF EXISTS `list_config`;
CREATE TABLE IF NOT EXISTS `list_config` (
  `id` int(10) NOT NULL auto_increment,
  `exp_rate` int(11) NOT NULL,
  `kill_on_fail` tinyint(1) NOT NULL default '0',
  `drop_rate` int(11) NOT NULL,
  `zuly_rate` int(11) NOT NULL,
  `blue_chance` int(11) NOT NULL,
  `slot_chance` int(11) NOT NULL,
  `stat_chance` int(11) NOT NULL,
  `refine_chance` int(11) NOT NULL,
  `rare_refine` int(11) NOT NULL default '0',
  `conf` varchar(50) NOT NULL,
  `player_damage` int(11) NOT NULL,
  `monster_damage` int(11) NOT NULL,
  `player_acc` int(11) NOT NULL,
  `monster_acc` int(11) NOT NULL,
  `pvp_acc` int(11) NOT NULL,
  `skill_damage` int(11) NOT NULL,
  `maxlevel` int(11) NOT NULL,
  `drop_type` int(11) NOT NULL,
  `savetime` int(11) NOT NULL,
  `partygap` int(11) NOT NULL,
  `maxstat` int(11) NOT NULL,
  `cfmode` int(11) NOT NULL,
  `autosave` int(11) NOT NULL,
  `mapdelay` int(11) NOT NULL,
  `visualdelay` int(11) NOT NULL,
  `worlddelay` int(11) NOT NULL,
  `fairymode` int(11) NOT NULL,
  `fairystay` int(11) NOT NULL,
  `fairywait` int(11) NOT NULL,
  `fairytestmode` int(11) NOT NULL,
  `osRoseVer` varchar(11) NOT NULL,
  `testgrid` int(11) NOT NULL,
  `jrose` int(11) NOT NULL,
  `is_pegasus` int(11) NOT NULL,
  `monmax` int(11) NOT NULL default '10',
  `massexport` int(11) NOT NULL default '0',
  `uwnbplayers` int(11) NOT NULL default '0',
  `uwside` int(11) NOT NULL default '0',
  `pc_drop_zuly` int(11) NOT NULL default '30',
  `drop_rev` int(11) NOT NULL default '1',
  `deathdelay` int(11) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

-- 
-- Dumping data for table `list_config`
-- 

INSERT INTO `list_config` (`id`, `exp_rate`, `kill_on_fail`, `drop_rate`, `zuly_rate`, `blue_chance`, `slot_chance`, `stat_chance`, `refine_chance`, `rare_refine`, `conf`, `player_damage`, `monster_damage`, `player_acc`, `monster_acc`, `pvp_acc`, `skill_damage`, `maxlevel`, `drop_type`, `savetime`, `partygap`, `maxstat`, `cfmode`, `autosave`, `mapdelay`, `visualdelay`, `worlddelay`, `fairymode`, `fairystay`, `fairywait`, `fairytestmode`, `osRoseVer`, `testgrid`, `jrose`, `is_pegasus`, `monmax`, `massexport`, `uwnbplayers`, `uwside`, `pc_drop_zuly`, `drop_rev`, `deathdelay`) VALUES 
(1, 1, 0, 2, 3, 5, 5, 5, 5, 0, 'default', 100, 100, 100, 100, 100, 100, 215, 2, 500, 10, 400, 0, 1, 10, 200, 700, 1, 45, 30, 0, '81.361', 1, 0, 0, 10, 0, 0, 0, 30, 1, 500);
