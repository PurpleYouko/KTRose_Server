--
-- Tabellenstruktur für Tabelle `list_quest_new`
--
 
CREATE TABLE IF NOT EXISTS `list_quest_new` (
  `owner` int(11) NOT NULL,
  `quests` blob,
  `questflags` blob
) ENGINE=MyISAM DEFAULT CHARSET=latin1;