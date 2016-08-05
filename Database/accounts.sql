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
-- Table structure for table `accounts`
-- 

DROP TABLE IF EXISTS `accounts`;
CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(11) NOT NULL auto_increment,
  `username` varchar(64) default NULL,
  `password` varchar(32) default NULL,
  `accesslevel` int(11) default '100',
  `lastip` varchar(15) default '0.0.0.0',
  `lasttime` int(11) default '0',
  `lastsvr` int(11) default '0',
  `lastchar` varchar(64) default NULL,
  `email` varchar(100) default NULL,
  `nb_donation` int(11) default '0',
  `donation` varchar(255) default '0',
  `active` int(11) default '1',
  `active_key` varchar(255) default NULL,
  `zulystorage` bigint(11) default '0',
  `platinum` tinyint(1) default '0',
  `online` tinyint(1) default '0',
  `login_count` int(11) default '0',
  `isSiteLogged` tinyint(1) default '0',
  `ktpoints` int(11) NOT NULL default '0',
  `newpoints` int(11) NOT NULL default '0',
  `logtime` int(11) NOT NULL default '0',
  `totlogtime` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=82 ;

-- 
-- Dumping data for table `accounts`
-- 

INSERT INTO `accounts` (`id`, `username`, `password`, `accesslevel`, `lastip`, `lasttime`, `lastsvr`, `lastchar`, `email`, `nb_donation`, `donation`, `active`, `active_key`, `zulystorage`, `platinum`, `online`, `login_count`, `isSiteLogged`, `ktpoints`, `newpoints`, `logtime`, `totlogtime`) VALUES 
(78, 'purpleyouko', '80f2ef8d121d9b6dcb5019a5a375db92', 900, '173.26.216.173', 1455489904, 1, 'PurpleYouko', NULL, 0, '0', 1, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(79, 'Katy', '0571749e2ac330a7455809c6b0e7af90', 900, '65.188.225.108', 1455510537, 1, 'KatysMule', NULL, 0, '0', 1, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(81, 'crystalkuwin', '0ecb88c066fa4cdae8b800f9f217b5c9', 100, '76.4.160.61', 1455506086, 1, 'Saphira', NULL, 0, '0', 1, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(80, 'silveryouko', 'c91cf770bfef6d1160215934427f26d5', 900, '76.4.160.61', 1455489896, 1, 'SilverYouko', NULL, 0, '0', 1, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0);
