class L1960_SQLQuery : array<ref L1960_GenericJson>
{
	void L1960_SQLQuery(string result)
	{
		L1960_PipeIPCJson jas = new L1960_PipeIPCJson(this);		
		jas.ExpandFromRAW(result);
	}
	
	string AsString()
	{
		string s = "";
		foreach(L1960_GenericJson ele : this)
			s = s + ele.AsString() + "\n";
		
		return s;
	}
};


class L1960_PipeIPCJson : JsonApiStruct
{
	protected ref array<string> data;
	protected ref array<string> names;
	protected ref array<string> types;
	
	array<ref L1960_GenericJson> content;

	override void OnSuccess( int errorCode )
	{
		PrintFormat("Success %1", errorCode);

		if (names && types)
		{
			for (int i = 0; i < data.Count();)
			{
				int pos = content.Insert(new L1960_GenericJson());
				for (int j = 0; j < names.Count(); j++)
				{
					string name = names[j];
					string type = types[j];
					string dat = data[i];
					
					if (type == "DATE")
						content[pos].Insert(name, new L1960_Datetime(dat));
					else if (type == "INT")
						content[pos].Insert(name, dat.ToInt());
					else if (type == "FLOAT")
						content[pos].Insert(name, dat.ToFloat());
					else if (type == "STRING")
						content[pos].Insert(name, dat);
					else if (type == "???")
						content[pos].Insert(name, dat);
					else
						Print("PipeIPCJson: Index:" + i + ", Name:" + name + ", Type:" + type + ", Data:" + dat, LogLevel.ERROR);
					
					i++;
				}
			}	
		}
		else
		{
			for (int i = 0; i < data.Count();i++)
			{
				int pos = content.Insert(new L1960_GenericJson());
				content[pos].Insert("data", data[i]);
			}
		}
	}
		
	
	void L1960_PipeIPCJson(array<ref L1960_GenericJson> c)
	{
		RegV("data");
		RegV("names");
		RegV("types");
		
		content = c;
	}	
};

enum L1960_Types
{
	TYPE_BOOL,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_DATE,
};

class L1960_Datetime : array<int>
{
	
	void L1960_Datetime(string list)
	{
		array<string> tmp = {};
		list.Split(",", tmp, false);
		foreach (string s : tmp)
			Insert(s.ToInt());
	}
	
	string AsString()
	{
		string data = "";
		foreach(int i : this)
			data = data + i + ", ";
		if (data.Length() > 0)
			return data.Substring(0, data.Length() - 2);
		else
			return "";
	}	
	
	string Format()
	{
		if (Count() == 6)
			return SCR_FormatHelper.FormatDateTime(Get(0), Get(1), Get(2), Get(3), Get(4), Get(5));
		else
			return AsString();
	}
	
	bool Equals(L1960_Datetime b)
	{
		if (Count() != b.Count())
			return false;
		foreach(int index, int i : this)
			if (i != b.Get(index))
				return false;
		return true;
	}
};

class L1960_GenericJson
{
	protected ref map<string, ref Tuple2<ref L1960_Types, string>> content = new map<string, ref Tuple2<ref L1960_Types, string>>();
	
	
	string AsString()
	{
		string s = "";
		for (int i = 0; i < content.Count(); i++)
			if (content.GetElement(i).param1 == L1960_Types.TYPE_DATE)
				s = s + content.GetKey(i) + ":" + L1960_Datetime(content.GetElement(i).param2).Format() + ", ";
			else
				s = s + content.GetKey(i) + ":" + content.GetElement(i).param2 + ", ";
		
		if (s.Length() > 0)
			return s.Substring(0, s.Length() - 2);
		else
			return "";
	}
	int Count()
	{
		return content.Count();
	}
	bool IsEmpty()
	{
		return content.IsEmpty();
	}
	void Clear()
	{
		content.Clear();
	}
	
	
	float GetFloat(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_FLOAT)
			return content.Get(key).param2.ToFloat();
		return 0.0;
	}
	int GetInt(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_INT)
			return content.Get(key).param2.ToInt();
		return 0;
	}
	string GetString(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_STRING)
			return content.Get(key).param2;
		return string.Empty;
	}
	bool GetBool(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_BOOL)
			return content.Get(key).param2.ToInt() == 1;
		return false;
	}
	L1960_Datetime GetDate(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_DATE)
			return new L1960_Datetime(content.Get(key).param2);
		return null;
	}
	
	int Insert(string key, int i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_INT, i.ToString()));
	}
	int Insert(string key, string i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_STRING, i));
	}
	int Insert(string key, float i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_FLOAT, i.ToString()));
	}
	int Insert(string key, bool i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_BOOL, i.ToString()));
	}
	int Insert(string key, L1960_Datetime i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_DATE, i.AsString()));
	}
	
	bool Find(string key, out string val)
	{
		Tuple2<ref L1960_Types, string> ele;	
		
		if (!content.Find(key, ele))
			return false;
		
		if (ele.param1 != L1960_Types.TYPE_STRING)
			return false;
		
		val = ele.param2;
			
		return true;
	}
	bool Find(string key, out int val)
	{
		Tuple2<ref L1960_Types, string> ele;	
		
		if (!content.Find(key, ele))
			return false;
		
		if (ele.param1 != L1960_Types.TYPE_INT)
			return false;
		
		val = ele.param2.ToInt();
			
		return true;
	}
	bool Find(string key, out float val)
	{
		Tuple2<ref L1960_Types, string> ele;	
		
		if (!content.Find(key, ele))
			return false;
		
		if (ele.param1 != L1960_Types.TYPE_FLOAT)
			return false;
		
		val = ele.param2.ToFloat();
			
		return true;
	}
	bool Find(string key, out bool val)
	{
		Tuple2<ref L1960_Types, string> ele;	
		
		if (!content.Find(key, ele))
			return false;
		
		if (ele.param1 != L1960_Types.TYPE_BOOL)
			return false;
		
		val = ele.param2.ToInt() == 1;
			
		return true;
	}
	
	string GetStringElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_STRING)
			return ele.param2;
		return string.Empty;
	}
	int GetIntElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_INT)
			return ele.param2.ToInt();
		return 0;
	}
	float GetFloatElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_FLOAT)
			return ele.param2.ToFloat();
		return 0;
	}
	float GetBoolElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_BOOL)
			return ele.param2.ToInt() == 1;
		return false;
	}
	L1960_Datetime GetDateElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_DATE)
			return new L1960_Datetime(ele.param2);
		return null;
	}
}