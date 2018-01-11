@echo off
SET location=../bin/windows
echo Starting server from %location%
start %location%/BoidsSimulation.exe -s servers.txt
start %location%/BoidsSimulation.exe -s servers.txt
TIMEOUT /T 3
