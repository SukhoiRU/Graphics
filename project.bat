@echo off
mkdir project
cmake -S . -B project -G "Visual Studio 14 2015 Win64"
pause
