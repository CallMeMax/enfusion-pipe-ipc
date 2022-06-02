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
		
		L1960_SQLQuery jas = new L1960_SQLQuery(m_IPC.SyncCall("SELECT CURRENT_TIMESTAMP() as time"));		
		
		if (!jas.IsEmpty())
		{
			SCR_HintManagerComponent.ShowCustomHint(jas.AsString(), "Pipe IPC", 12);
			PrintFormat("The time is %1", jas[0].GetDate("time").Format())
		}
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
	static L1960_PipeIPCGameComponent GetInstance()
	{
		BaseGameMode gameMode = GetGame().GetGameMode();
		if (gameMode)
			return L1960_PipeIPCGameComponent.Cast(gameMode.FindComponent(L1960_PipeIPCGameComponent));
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
