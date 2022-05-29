class L1960_PipeIPC {

	protected string m_sPipeIn;	
	protected string m_sPipeOut;	
	
	protected FileHandle m_FileIn;
	protected FileHandle m_FileOut;
	
	protected string m_Buff;
	
	const string NO_CONNECTION_ERROR = "Could not connect to pipe.";
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//--- Public functions
	////////////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------
	void L1960_PipeIPC(string pipeIn, string pipeOut)
	{
		m_sPipeIn = pipeIn;
		m_sPipeOut = pipeOut;
		
		for (int i=0; i < 1024; i++)
			m_Buff += "\0";
	}
	
	void ~L1960_PipeIPC()
	{
		/*
		// crashes in clean up^^
		
		if (m_FileIn)
			m_FileIn.CloseFile();
		if (m_FileOut)
			m_FileOut.CloseFile();
		*/
	}
	
	void setPipeIn(string pipeIn)
	{
		m_sPipeIn = pipeIn;
	}
	
	void setPipeOut(string pipeOut)
	{
		m_sPipeOut = pipeOut;
	}
	
	string SyncCall(string msg)
	{		
		m_FileOut = FileIO.OpenFile(m_sPipeOut, FileMode.OPENW);	
		
		
		if (!m_FileOut)
		{
			Print("No pipeOut", LogLevel.ERROR);
			return NO_CONNECTION_ERROR;
		}

		m_FileOut.FPrint(msg); // blocking main thread until msg received

		if (SCR_Global.IsEditMode())
			return NO_CONNECTION_ERROR;
		
		m_FileIn = FileIO.OpenFile(m_sPipeIn, FileMode.READ);	
		if (!m_FileIn)
		{
			Print("No pipeIn", LogLevel.ERROR);
			return NO_CONNECTION_ERROR;
		} 
		
		int i = m_FileIn.ReadFile(m_Buff, m_Buff.Length()); // non blocking call
		string s = m_Buff.Substring(0, i);

		m_FileIn.CloseFile();  // also kills the pipe
		
		if (SCR_Global.IsEditMode())
			return NO_CONNECTION_ERROR;
		
		if (m_FileOut) 
			m_FileOut.FPrint("ende");  // blocking call before closing the pipe
		
		m_FileOut.CloseFile();  // also kills the pipe
		
		if (!s)
			return NO_CONNECTION_ERROR;
		
		return s;
	}
}