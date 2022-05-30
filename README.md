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
 
## Contributing
Any help or ideas for improvement are very welcome. 
Please feel free to point out any errors in the code or improvements of the code in general.
