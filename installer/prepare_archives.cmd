set MAINDIR=%~dp0
set GAMEDIR=%MAINDIR%\game_distrib_files
set OUTDIR=%MAINDIR%\output
set PASSWD=sdu28042elmd
set VOLUMELIMIT=1500m

mkdir %OUTDIR%\game

cd %GAMEDIR%\appdata
%MAINDIR%\7za a -p%PASSWD% -r -y -v%VOLUMELIMIT% appdata.7z .
move /y appdata.7z* %OUTDIR%\game
cd %GAMEDIR%\bins
%MAINDIR%\7za a -p%PASSWD% -r -y -v%VOLUMELIMIT% bins.7z .
move /y bins.7z* %OUTDIR%\game
cd %GAMEDIR%\gamedata
%MAINDIR%\7za a -p%PASSWD% -r -y -v%VOLUMELIMIT% gamedata.7z .
move /y gamedata.7z* %OUTDIR%\game
cd %GAMEDIR%
%MAINDIR%\7za a -p%PASSWD% -r -y -v%VOLUMELIMIT% maindir.7z start.bat gamedata.db* fsgame.ltx
move /y maindir.7z* %OUTDIR%\game

cd %MAINDIR%
