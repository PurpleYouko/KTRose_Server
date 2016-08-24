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




