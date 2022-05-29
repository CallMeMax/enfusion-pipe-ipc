from abc import ABC, abstractmethod

try:
    import win32api, win32pipe, win32file, pywintypes, time, sys, struct
except:
    print("""
        Modul `pywin32` is necessary for this service to be running. Try ``pip install pywin32` to install. 
    """)
    exit(1)


class PipeIPCServer(ABC):
    """
        Runs a server which tries to communicate with the L1960_PipeIPC in ArmaReforgerWorkbench via a windows named pipe.
        Can only work with one client and is dependent on the specific implementation of this class.
    """

    def __init__(self, pipeIn=r"\\.\pipe\ArmaIn", pipeOut=r"\\.\pipe\ArmaOut"):
        self.pipeInName = pipeIn
        self.pipeOutName = pipeOut

        self.pipeIn = None
        self.pipeOut = None
        self.init_pipe()

        if self.pipeOut:
            win32file.CloseHandle(self.pipeOut)
        self.pipeOut = win32pipe.CreateNamedPipe(self.pipeOutName,
                                                 win32pipe.PIPE_ACCESS_INBOUND,
                                                 win32pipe.PIPE_TYPE_BYTE | win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
                                                 1, 0, 0, 0, None)

    def init_pipe(self):
        """
        Restarting the pipeIn, since arma closes the handle which terminates the pipe.
        pipeOut does not need to be restarted, because ... I don´t know. Maybe it is because of the different open type (READ vs OPENW) in ARMA??

        :return: nothing
        """
        if self.pipeIn:
            win32file.CloseHandle(self.pipeIn)
        self.pipeIn = win32pipe.CreateNamedPipe(self.pipeInName,
                                                win32pipe.PIPE_ACCESS_OUTBOUND,
                                                win32pipe.PIPE_TYPE_BYTE | win32pipe.PIPE_READMODE_BYTE | win32pipe.PIPE_WAIT,
                                                1, 0, 0, 0, None)

    @abstractmethod
    def process_request(self, req):
        """
        Gets the string which was received from ARMA and can return the answer, that should be send back.
        :param req: request from ARMA
        :return: responds from this server
        """
        pass

    def run(self):
        print("running ...")
        while True:
            try:
                win32pipe.ConnectNamedPipe(self.pipeOut, None)  # wait for new connection

                i, msg = win32file.ReadFile(self.pipeOut, 1024)  # read request

                data = self.process_request(msg.decode())  # work on request

                # Read twice because ARMA writes everything twice into the pipe.
                # Arma waits until all was read from the pipe.
                win32file.ReadFile(self.pipeOut, 1024)

                win32pipe.ConnectNamedPipe(self.pipeIn, None)
                win32file.WriteFile(self.pipeIn, data.encode())  # send aswer to arma

                win32file.ReadFile(self.pipeOut, 1024)
                win32pipe.DisconnectNamedPipe(self.pipeIn)  # since response from arma was received, can close pipeIn
                win32file.ReadFile(self.pipeOut, 1024)  # read twice because ARMA writes everything twice into the pipe

                win32pipe.DisconnectNamedPipe(self.pipeOut)
            finally:
                self.init_pipe()  # restart all pipes and close the old ones
                # print("ERROR:", win32api.FormatMessage(win32api.GetLastError()))


class SQLServer(PipeIPCServer):

    def process_request(self, req):
        """
        Query SQL DB
        :param req: SQL statement
        :return: string response
        """
        import mysql.connector
        mydb = mysql.connector.connect(
            host="",
            user="",
            password="",
            database=""
        )
        mycursor = mydb.cursor()

        mycursor.execute(req)

        myresult = mycursor.fetchall()

        return str(myresult)


if __name__ == '__main__':
    my_pipe = SQLServer(r"\\.\pipe\ArmaIn", r"\\.\pipe\ArmaOut")
    my_pipe.run()

