--
-- Table structure for table `ban_list`
--

DROP TABLE IF EXISTS `ban_list`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ban_list` (
  `ip` varchar(256) NOT NULL,
  `startban` decimal(20,0) NOT NULL,
  `bantime` decimal(20,0) default '0',
  `bannedby` varchar(256) NOT NULL,
  `reason` varchar(256) NOT NULL,
  `id` int(11) NOT NULL auto_increment,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ban_list`
--

LOCK TABLES `ban_list` WRITE;
/*!40000 ALTER TABLE `ban_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `ban_list` ENABLE KEYS */;
UNLOCK TABLES;
