@echo off

echo.
echo. create result directory
mkdir _bin

echo.
echo. delete old files
del _bin\* /Q

echo.
echo. copy new files
copy _release\KenanAgent32.exe _bin\
copy _release\KenanAgent64.exe _bin\
copy _release\KenanLauncher64.exe _bin\
copy _release\KenanSecuSvc64.exe _bin\

copy _release\KenanClp32.dll _bin\
copy _release\KenanClp64.dll _bin\
copy _release\KenanSecu32.dll _bin\
copy _release\KenanSecu64.dll _bin\
copy _release\KenanVDMgr64.dll _bin\

copy _release\v.exe _bin\
copy _release\vf.exe _bin\
copy _release\veracrypt.sys _bin\
copy _release\veracrypt-arm64.sys _bin\
copy _release\veracrypt-x64.sys _bin\

del ..\nsis\image\* /Q
copy _bin\* ..\nsis\image\

pause