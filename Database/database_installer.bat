@echo off

REM ############################################
REM ## You can change here your own DB params ##
REM ############################################
REM MYSQL BIN PATH
set mysqlBinPath=C:\Program Files\MySQL\MySQL Server 5.0\bin

set user=root
set pass=
set db=roseon
set host=localhost

REM ############################################

set mysqldumpPath="%mysqlBinPath%\mysqldump"
set mysqlPath="%mysqlBinPath%\mysql"







REM DONT TOUCH BELOW!
REM ############################################
REM ############################################

echo PLEASE EDIT THIS SCRIPT SO VALUES IN THE CONFIG SECTION MATCH YOUR DATABASE(S)
echo.
echo.
echo Making a backup of the original database.
%mysqldumpPath% --add-drop-table -h %host% -u %user% --password=%pass% %db% > osrose_backup.sql
echo.
echo WARNING: A full install (f) will destroy data in your
echo          accounts,characters,items,quest_data and storage tables.
echo          Choose upgrade (u) if you already have a running server

:ask
set askprompt=x
set /p askprompt=DB install type: (f) full or (u) upgrade or {s} skip or (q) quit? 
if /i %askprompt%==f goto install
if /i %askprompt%==u goto upgrade
if /i %askprompt%==q goto end
goto ask

:install
echo Deleting database tables for new content.
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < full_install.sql
echo creating accounts table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < accounts.sql
echo creating channels tables
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < channels.sql
echo creating characters table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < characters.sql
echo creating items table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < items.sql
echo creating list_clans table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_clan.sql
echo creating list_friend table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_friend.sql
echo creating list_quest_qsd table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_quest_qsd.sql
echo creating list_mobgroups table !!! THIS CAN BE VERY LONG !!!
echo Note: !!! THIS CAN BE VERY LONG !!!
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_mobgroups.sql
echo creating list_config table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% <list_config.sql
echo creating mail_list table
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% <mail_list.sql

:upgrade
echo Installing new database content.
echo updating list_npcs
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_npcs.sql
echo updating list_npcs_special
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_npcs_special.sql
echo updating list_respawnzones
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_respawnzones.sql
echo updating telegates
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_telegates.sql
echo updating storage
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < storage.sql
echo mileage
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < mileage.sql
echo item_drops
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < item_drops.sql
echo item_drops_and
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < item_drops_and.sql
echo ban_list
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < ban_list.sql
echo wishlist
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < wishlist.sql
echo GM Cart, CG list
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_cart_cg.sql
echo GM allskill
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_skills.sql


echo Those files are specific to PY custom events.
echo PY:: custom events
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_customevents.sql
echo PY:: custom gates
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_customgates.sql

echo Those files are specific to PY drop system ("old one")
echo PY:: extra stats
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_extra_stats.sql
echo PY:: skillbooks 
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_skillbooks.sql

echo Basic Events (originals)
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_events.sql



:end
echo.
echo Script complete.
pause