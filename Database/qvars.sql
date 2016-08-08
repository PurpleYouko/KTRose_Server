-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Aug 08, 2016 at 11:18 AM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `qvars`
-- 

DROP TABLE IF EXISTS `qvars`;
CREATE TABLE IF NOT EXISTS `qvars` (
  `owner` int(11) NOT NULL,
  `episode` varchar(64) collate latin1_general_ci NOT NULL default '0,0,0,0,0',
  `job` varchar(32) collate latin1_general_ci NOT NULL default '0,0,0',
  `planet` varchar(32) collate latin1_general_ci NOT NULL default '0,0,0,0,0,0,0',
  `unionvar` varchar(64) collate latin1_general_ci NOT NULL default '0,0,0,0,0,0,0,0,0,0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- 
-- Dumping data for table `qvars`
-- 

INSERT INTO `qvars` (`owner`, `episode`, `job`, `planet`, `unionvar`) VALUES 
(0, '0,1,0,0,0', '0,0,0', '0,0,0,0,0,0,0', '0,0,0,0,0,0,0,0,0,0'),
(482, '0,1,0,0,0', '0,0,0', '0,0,0,0,0,0,0', '0,0,0,0,0,0,0,0,0,0'),
(485, '0,1,0,0,0', '0,0,0', '0,0,0,0,0,0,0', '0,0,0,0,0,0,0,0,0,0');
