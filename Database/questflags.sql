-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Feb 16, 2016 at 12:36 PM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `questflags`
-- 

DROP TABLE IF EXISTS `questflags`;
CREATE TABLE IF NOT EXISTS `questflags` (
  `owner` int(11) NOT NULL,
  `flags` varchar(500) collate latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- 
-- Dumping data for table `questflags`
-- 

INSERT INTO `questflags` (`owner`, `flags`) VALUES 
(486, '15'),
(486, '6'),
(483, '15'),
(487, '75'),
(487, '74'),
(487, '73'),
(487, '72'),
(487, '69'),
(487, '68'),
(487, '67'),
(487, '66'),
(487, '64'),
(487, '15'),
(488, '78'),
(488, '77'),
(488, '64'),
(488, '15'),
(481, '79'),
(481, '78'),
(488, '13'),
(481, '77'),
(481, '64'),
(481, '15'),
(490, '77'),
(490, '76'),
(490, '75'),
(490, '74'),
(490, '73'),
(490, '72'),
(490, '71'),
(490, '70'),
(490, '69'),
(490, '68'),
(490, '67'),
(490, '66'),
(490, '64'),
(490, '15'),
(490, '12'),
(489, '69'),
(489, '68'),
(489, '67'),
(489, '66'),
(489, '64'),
(489, '15');
