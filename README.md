# ARMA Reforger Interprocess Communication 
This project offers a proof of concept implementation of an IPC class writing from ARMA Reforger Workbench (Enfusion Engine) using the FileIO class together with windows named pipes.
Additionally a python program is supplied as a back-end to communicate with. 
It enables the communication with a SQL DB as an example use case.
This implementation is not suitable for any production environment, because of performance and stability issues.

## Requirements
The python program requires Python 3 and the `pywin32` package for the interaction with the windows named pipes and the `mysql-connector-python` package for the communication with the SQL server.
The ARMA Reforger code can be imported into any existing project without any further requirements.

## Getting started
To use this code, first the connection and login information in the python file have to be inserted. These can be found in the `process_request` function in the `SQLServer` class.
Then the python service has to be started by simply running the python file via the command line
    
    $ python L1960_pipeIPC.py
    
The Arma Reforger code must be copied into an existing project which contains at least a SCR_BaseGameMode.
Then the `L1960_PipeIPCGameComponent` and the `SCR_HintManagerComponent` must be added to the game mode. 
3 seconds after the game start, a query to the SQL DB will be performed, asking for the current time.
The result will be visible in a hint.
 
## Customizations options
After adding the `L1960_PipeIPCGameComponent` to the Game Mode in the World Editor, one can customize the name of the `Pipe In` and `Pipe Out`. These pipes must be the same as the used by the python script - otherwise the programs cannot communicate. 

## Troubleshooting
If the pipes are configured correctly, but the game cannot establish a connection, there can be a problem with windows pipes. 
Especially, this problem might occur only in the published project (downloaded from the Workshop), but not in the developing environment of the World Editor.
There are two possible ways to fix this (exemplary for `Pipe In`):
1. Use `\\.\pipe\ArmaIn` instead of `\\127.0.0.1\pipe\ArmaIn`
2. Create a link to the pipe in the profile folder 

       $ mklink ArmaIn.pipe \\127.0.0.1\pipe\ArmaIn
 
    and use `$profile:ArmaIn.pipe` as the `Pipe In` parameter.
 
## Contributing
Any help or ideas for improvement are very welcome. 
Please feel free to point out any errors in the code or improvements of the code in general.
