@echo off
SET location=../bin/windows
echo Starting server from %location%

pushd ../src
start %location%/boid-simulation.exe -s servers
start %location%/boid-simulation.exe -s servers
popd

TIMEOUT /T 3
