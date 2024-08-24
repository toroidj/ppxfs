@echo off
set RETAIL=1
rem *** set value ***
set arcname=ppxfs0.zip
set readme=ppxfs.txt
set srcname=ppxfssrc.7z
set exename=ppxfs

rem *** main ***
del /q *.zip 2> NUL
del /q *.map 2> NUL

del /q *.obj 2> NUL
del /q *.res 2> NUL
make -DRELEASE
call m64 -DRELEASE

tfilesign sp %exename%.dll %exename%.dll
tfilesign sp %exename%64.dll %exename%64.dll
tfilesign sp %exename%64A.dll %exename%64A.dll

rem *** Source Archive ***
if %RETAIL%==0 goto :skipsource

for %%i in (*) do CT %readme% %%i
ppb /c %%u/7-zip32.dll,a %srcname% -hide -mx=9 MAKE* GNUmake* *.BAT *.C *.CPP *.H *.HPJ *.pl *.RC *.RH *.ICO *.DEF pplib*.lib
CT %readme% %srcname%
:skipsource

for %%i in (*.exe) do CT %readme% %%i
for %%i in (*.dll) do CT %readme% %%i
ppb /c %%u/7-ZIP32.DLL,a -tzip -hide -mx=7 %arcname% %readme% *.exe *.dll %srcname%
tfilesign s %arcname% %arcname%
CT %readme% %arcname%
del /q %srcname% 2> NUL
