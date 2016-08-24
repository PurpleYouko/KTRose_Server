-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Aug 08, 2016 at 11:21 AM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `osrose`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `qflags`
-- 

DROP TABLE IF EXISTS `qflags`;
CREATE TABLE IF NOT EXISTS `qflags` (
  `owner` int(11) NOT NULL,
  `switches` varchar(1024) collate latin1_general_ci NOT NULL default '0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;


