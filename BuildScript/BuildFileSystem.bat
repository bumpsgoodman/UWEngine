@echo off

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ..\Project\UWEngineFileSystem\UWEngineFileSystem\UWEngineFileSystem.vcxproj /t:build /p:configuration=debug /p:platform=x64
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ..\Project\UWEngineFileSystem\UWEngineFileSystem\UWEngineFileSystem.vcxproj /t:build /p:configuration=debug /p:platform=Win32

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ..\Project\UWEngineFileSystem\UWEngineFileSystem\UWEngineFileSystem.vcxproj /t:build /p:configuration=release /p:platform=x64
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ..\Project\UWEngineFileSystem\UWEngineFileSystem\UWEngineFileSystem.vcxproj /t:build /p:configuration=release /p:platform=Win32

echo.
del /s /q "..\Output\LIB\*.exp"
echo.
echo Complate
