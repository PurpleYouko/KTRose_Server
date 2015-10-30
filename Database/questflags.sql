-- phpMyAdmin SQL Dump
-- version 2.10.3
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: May 27, 2015 at 10:01 AM
-- Server version: 5.0.45
-- PHP Version: 5.2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

-- 
-- Database: `rev81`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `questflags`
-- 

CREATE TABLE `questflags` (
  `owner` int(11) NOT NULL,
  `flags` varchar(500) collate latin1_general_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- 
-- Dumping data for table `questflags`
-- 

