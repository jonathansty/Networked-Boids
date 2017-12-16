@echo off
SET location=bin
echo Starting server from %location%
start %location%\src_sfml_Release -s servers
start %location%\src_sfml_Release -s servers
TIMEOUT /T 3
