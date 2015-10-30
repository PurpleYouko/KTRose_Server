--
-- Table structure for table `wishlist`
--

CREATE TABLE `wishlist` (
  `itemowner` int(11) NOT NULL default '0',
  `itemhead` bigint(11) NOT NULL default '0',
  `itemdata` bigint(11) NOT NULL default '0',
  `slot` int(11) NOT NULL default '0',
  UNIQUE KEY `ow_slot` (`itemowner`,`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


