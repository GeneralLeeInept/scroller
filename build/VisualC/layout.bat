:: Scroller layout batch file.
:: Copies build outputs to layout for packaging into setup by WiX.
:: Usage:
::      layout.bat [debug]

@echo off

goto main

:copy_file
robocopy "%1" "%2" "%3"
exit /b

:main
setlocal enabledelayedexpansion

pushd "%~dp0"

if /i "%1"=="debug" (
    shift
    echo on
)

:: Make layout folder
set _project_dir=%~dp0
set _build_dir=%_project_dir%..
set _root_dir=%_build_dir%\..
set _deps_dir=%_root_dir%\deps
set _target_dir=%_build_dir%\layout

attrib /S -P -R %_target_dir%\*.*
rmdir /q /s %_target_dir%
mkdir %_target_dir%

:: Copy SDL
call:copy_file %_deps_dir%\SDL2-2.0.7\lib\x86 %_target_dir% SDL2.dll
call:copy_file %_deps_dir%\SDL2_image-2.0.2\lib\x86 %_target_dir% SDL2_image.dll
call:copy_file %_deps_dir%\SDL2_image-2.0.2\lib\x86 %_target_dir% zlib1.dll
call:copy_file %_deps_dir%\SDL2_image-2.0.2\lib\x86 %_target_dir% libpng16-16.dll

:: Copy data
robocopy %_root_dir%\art %_target_dir% /S /XD raw

popd

exit /b 0
