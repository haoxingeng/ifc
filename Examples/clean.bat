@echo off
echo Begin to clean...

del /f /s /q *.aps
del /f /s /q *.ncb
del /f /s /q /ah *.suo
del /f /s /q *.vcproj.*.user

for /f "tokens=*" %%i in ('dir /ad /s /b debug') do rd /s /q %%i
for /f "tokens=*" %%i in ('dir /ad /s /b release') do rd /s /q %%i

echo Finished!
echo. & pause 
