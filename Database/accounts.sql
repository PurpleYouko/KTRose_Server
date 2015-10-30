-- ----------------------------
-- Table structure for accounts
-- ----------------------------
CREATE TABLE IF NOT EXISTS accounts (
	id int(11) NOT NULL auto_increment,
	username varchar(64) default NULL,
	password varchar(32) default NULL,
	accesslevel int(11) default 100,
	lastip varchar(15) default '0.0.0.0',
	lasttime int(11) default 0,
	lastsvr int(11) default 0,
	lastchar varchar(64) default NULL,
	email varchar(100) default NULL,
	nb_donation int(11) default 0,
	donation varchar(255) default '0',
	active int(11) default 1,
	active_key varchar(255) default NULL,
	zulystorage bigint(11) default 0,
	platinum tinyint(1) default 0,
	online tinyint(1) default 0,
	login_count int(11) default 0,
	isSiteLogged tinyint(1) default 0,
	  PRIMARY KEY  (id)
	) ENGINE=InnoDB AUTO_INCREMENT=78 DEFAULT CHARSET=latin1;
