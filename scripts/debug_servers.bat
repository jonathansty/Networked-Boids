@echo off
cd ../src

SET location=../bin/windows
echo Starting server from %location%
pushd ../src
start %location%/boid-simulation-debug.exe -s servers
start %location%/boid-simulation-debug.exe -s servers
popd

TIMEOUT /T 3
