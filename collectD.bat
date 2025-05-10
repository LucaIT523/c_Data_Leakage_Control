@echo off

echo.
echo. create result directory
mkdir _binD

echo.
echo. delete old files
del _binD\* /Q

echo.
echo. copy new files
copy _debug\KenanAgent32.exe _binD\
copy _debug\KenanAgent64.exe _binD\
copy _debug\KenanLauncher64.exe _binD\
copy _debug\KenanSecuSvc64.exe _binD\

copy _debug\KenanClp32.dll _binD\
copy _debug\KenanClp64.dll _binD\
copy _debug\KenanSecu32.dll _binD\
copy _debug\KenanSecu64.dll _binD\
copy _debug\KenanVDMgr64.dll _binD\

copy _release\v.exe _binD\
copy _release\vf.exe _binD\
copy _release\veracrypt.sys _binD\
copy _release\veracrypt-arm64.sys _binD\
copy _release\veracrypt-x64.sys _binD\

pause