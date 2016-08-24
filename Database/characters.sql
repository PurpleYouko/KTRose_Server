-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Feb 16, 2016 at 12:35 PM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `characters`
-- 

CREATE TABLE `characters` (
  `account_name` varchar(64) NOT NULL,
  `id` int(11) NOT NULL auto_increment,
  `char_name` varchar(64) NOT NULL,
  `zuly` bigint(11) NOT NULL default '10000',
  `level` decimal(11,0) NOT NULL default '1',
  `curHp` decimal(18,0) NOT NULL default '50',
  `curMp` decimal(18,0) NOT NULL default '18',
  `str` decimal(11,0) NOT NULL default '15',
  `con` decimal(11,0) NOT NULL default '15',
  `dex` decimal(11,0) NOT NULL default '15',
  `_int` decimal(11,0) NOT NULL default '15',
  `cha` decimal(11,0) NOT NULL default '10',
  `sen` decimal(11,0) NOT NULL default '10',
  `face` decimal(11,0) NOT NULL default '1',
  `hairStyle` decimal(11,0) NOT NULL default '1',
  `sex` decimal(11,0) NOT NULL default '0',
  `exp` decimal(11,0) NOT NULL default '0',
  `skillp` decimal(11,0) NOT NULL default '0',
  `statp` decimal(11,0) NOT NULL default '0',
  `clanid` decimal(11,0) NOT NULL default '0',
  `classid` decimal(11,0) NOT NULL default '0',
  `deletetime` decimal(20,0) NOT NULL default '0',
  `clan_rank` int(11) NOT NULL default '0',
  `stamina` int(11) NOT NULL default '5000',
  `quickbar` varchar(255) NOT NULL default '0',
  `basic_skills` varchar(255) NOT NULL default '11,12,13,14,15,16,17,18,19,20,25,26,27,28,29,30,31,32,33,50,101',
  `class_skills` varchar(500) NOT NULL default '0',
  `class_skills_level` varchar(500) NOT NULL default '1',
  `respawnid` int(11) NOT NULL default '56',
  `townid` int(11) NOT NULL default '56',
  `rewardpoints` int(11) NOT NULL default '0',
  `unionid` int(11) NOT NULL default '0',
  `union1points` int(11) NOT NULL default '0',
  `union2points` int(11) NOT NULL default '0',
  `union3points` int(11) NOT NULL default '0',
  `union4points` int(11) NOT NULL default '0',
  `union5points` int(11) NOT NULL default '0',
  `union6points` int(11) NOT NULL default '0',
  `union7points` int(11) NOT NULL default '0',
  `union8points` int(11) NOT NULL default '0',
  `union9points` int(11) NOT NULL default '0',
  `union10points` int(11) NOT NULL default '0',
  `unionfame` int(11) NOT NULL default '0',
  `union01` int(11) NOT NULL default '0',
  `union02` int(11) NOT NULL default '0',
  `union03` int(11) NOT NULL default '0',
  `union04` int(11) NOT NULL default '0',
  `union05` int(11) NOT NULL default '0',
  `bonusxp` int(11) NOT NULL default '0',
  `timerxp` int(11) NOT NULL default '0',
  `shoptype` int(11) NOT NULL default '0',
  `timershop` int(11) NOT NULL default '0',
  `isgm` tinyint(1) NOT NULL default '0',
  `isdev` tinyint(1) NOT NULL default '0',
  `unique_skills` varchar(1024) NOT NULL default '0',
  `unique_skills_level` varchar(255) NOT NULL default '1',
  `mileage_skills` varchar(1024) NOT NULL default '0',
  `mileage_skills_level` varchar(1024) NOT NULL default '1',
  `driving_skills` varchar(255) NOT NULL default '0',
  `bonusddrop` int(11) NOT NULL default '0',
  `timerddrop` int(11) NOT NULL default '0',
  `bonusstatdrop` int(11) NOT NULL default '0',
  `timerstatdrop` int(11) NOT NULL default '0',
  `bonusgraydrop` int(11) NOT NULL default '0',
  `timergraydrop` int(11) NOT NULL default '0',
  `highestoverkill` float NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=491 ;



