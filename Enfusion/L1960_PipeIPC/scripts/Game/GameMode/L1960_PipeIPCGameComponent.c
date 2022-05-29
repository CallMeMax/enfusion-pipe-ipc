[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Adds survival aspects.", color: "0 0 255 255")]
class L1960_PipeIPCGameComponentClass: SCR_BaseGameModeComponentClass
{
};

class L1960_PipeIPCGameComponent : SCR_BaseGameModeComponent {

	[Attribute("\\\\127.0.0.1\\pipe\\ArmaIn", UIWidgets.EditBox)]
	protected string m_sPipeIn;	
	
	[Attribute("\\\\127.0.0.1\\pipe\\ArmaOut", UIWidgets.EditBox)]
	protected string m_sPipeOut;	

	ref L1960_PipeIPC m_IPC;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//--- Public functions
	////////////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------
	void L1960_PipeIPCGameComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_IPC = new L1960_PipeIPC(m_sPipeIn, m_sPipeOut);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~L1960_PipeIPCGameComponent()
	{
		delete m_IPC;
	}
	
	/*!
	Test the implementation of the SQL Pipe by requesting the system time.
	*/
	void test()
	{
		//m_IPC.setPipeIn(m_sPipeIn);
		//m_IPC.setPipeOut(m_sPipeOut);
		//string s = m_IPC.SyncCall("SELECT name,uid,items FROM users WHERE uid='76561198058436233'");
		string s = m_IPC.SyncCall("SELECT CURRENT_TIMESTAMP()");
		if (s)
			SCR_HintManagerComponent.ShowCustomHint(s, "Pipe IPC", 12);
		else
			SCR_HintManagerComponent.ShowCustomHint("Null?!", "Pipe IPC", 12);
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//--- Static functions
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/*!
	Get instance of the IPC manager game component. This can be used to query the DB from any file (similar to the SCR_HintManagerComponent).
	\return IPC manager
	*/
	static L1960_PipeIPC GetInstance()
	{
		BaseGameMode gameMode = GetGame().GetGameMode();
		if (gameMode)
			return L1960_PipeIPC.Cast(gameMode.FindComponent(L1960_PipeIPC));
		else
			return null;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//--- Default functions
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/*!
	Test the implementation by calling test() after 3 sec.
	*/
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		GetGame().GetCallqueue().CallLater(test, 3000);
	}
}