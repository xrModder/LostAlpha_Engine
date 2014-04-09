set MAINDIR=%~dp0
set GAMEDIR=%MAINDIR%\game_distrib_files
set OUTDIR=%MAINDIR%\output

mkdir %OUTDIR%\game

cd %GAMEDIR%\appdata
%MAINDIR%\7za a -phaslo1234 -r -y appdata.7z .
move /y appdata.7z %OUTDIR%\game
cd %GAMEDIR%\bins
%MAINDIR%\7za a -phaslo1234 -r -y bins.7z .
move /y bins.7z %OUTDIR%\game
cd %GAMEDIR%\gamedata
%MAINDIR%\7za a -phaslo1234 -r -y gamedata.7z .
move /y gamedata.7z %OUTDIR%\game
cd %GAMEDIR%
%MAINDIR%\7za a -phaslo1234 -r -y maindir.7z start.bat gamedata.db? fsgame.ltx
move /y maindir.7z %OUTDIR%\game

cd %MAINDIR%
