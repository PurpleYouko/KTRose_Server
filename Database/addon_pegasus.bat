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
echo creating list_mobgroups table !!! THIS CAN BE VERY LONG !!!
echo Note: !!! THIS CAN BE VERY LONG !!!
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_mobgroups_pegasus.sql
echo Installing new database content.
echo updating list_npcs
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_npcs_pegasus.sql
echo updating telegates
%mysqlPath% -h %host% -u %user% --password=%pass% -D %db% < list_telegates_pegasus.sql


:end
echo.
echo Script complete.
pause