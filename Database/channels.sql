-- ----------------------------
-- Table structure for channels
-- ----------------------------
CREATE TABLE IF NOT EXISTS channels (
	id int(11) NOT NULL,
	type int(11) NOT NULL,
	name varchar(255) default NULL,
	host varchar(255) default NULL,
	port int(11) default NULL,
	lansubmask varchar(255) default NULL,
	lanip varchar(255) default NULL,
	connected int(11) default NULL,
	maxconnections int(11) default NULL,
	owner int(11) default NULL
	)ENGINE=InnoDB DEFAULT CHARSET=latin1;
