class L1960_SQLQuery : array<ref L1960_GenericJson>
{
	string m_sError;  // error from the DB 
	string m_sQuery;  // query which was requested
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//--- Public functions
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*!
	Parses the SQL query result into a L1960_PipeIPCJson object.
	\param result Json object containing 3 string lists: data, names, types. Where names and types must be the same length and data must be a multiple.
	*/
	void L1960_SQLQuery(string result = "{\"data\":[], \"types\":[], \"names\":[], \"error\":\" \", \"query\":\" \"}")
	{
		L1960_PipeIPCJson jas = new L1960_PipeIPCJson(this);		
		jas.ExpandFromRAW(result);
	}
	
	/*!
	Generates a string containing all data form the SQL query.
	\return string representation
	*/
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
	protected ref array<string> data;   // data conntent of the SQL query
	protected ref array<string> names;  // names of the SQL columns 
	protected ref array<string> types;  // types of the SQL columns 
	
	protected string error;  // error from the DB 
	protected string query;  // query which was requested
	
	L1960_SQLQuery content;  // SQL query parsed as rows of L1960_GenericJson

	/*!
	Called after successful parsing of the Json
	\param errorCode
	*/
	override void OnSuccess( int errorCode )
	{
		content.m_sError = error;
		content.m_sQuery = query;
		
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
		
	/*!	
	data, names and types are parse automatically from the Json data.
	\param c the L1960_GenericJson list for the output
	*/
	void L1960_PipeIPCJson(L1960_SQLQuery c)
	{
		RegV("data");
		RegV("names");
		RegV("types");
		
		RegV("error");
		RegV("query");
		
		content = c;
	}	
};


/*!
Type of Json element. Used by L1960_GenericJson.
*/
enum L1960_Types
{	
	TYPE_BOOL,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_DATE,  // SQL Timestamp containing Year, Month, day, hour, minute, second
};

/*!
SQL Timestamp containing Year, Month, day, hour, minute, second
*/
class L1960_Datetime : array<int>
{
	/*!	
	List of Year, Month, day, hour, minute, second in this order.
	\param list string of 6 integers separated by `,`
	*/
	void L1960_Datetime(string list)
	{
		array<string> tmp = {};
		list.Split(" ", tmp, false);
		
		array<string> tmp_2 = {};		
		tmp[0].Split("-", tmp_2, false);
		array<string> tmp_3 = {};
		tmp[1].Split(":", tmp_3, false);
		
		foreach (string s : tmp_2)
		{
			int i = s.ToInt();
			Insert(i);
		}
		
		foreach (string s : tmp_3)
		{
			int j = s.ToInt();
			Insert(j);
		}
	}
	
	/*!	
	Parses the data of this class as a more beautiful string.
	\return string representation
	*/
	string AsString()
	{
		if (Count() == 6)
			return SCR_FormatHelper.FormatDateTime(Get(0), Get(1), Get(2), Get(3), Get(4), Get(5));
		else
			return "";
	}
	
	/*!	
	Compares two objects of type L1960_Datetime
	\return true if equal false otherwise
	*/
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

/*!
L1960_GenericJson represents a Json object which can contain a varying number of atributes of a type defined by L1960_Types. 
Specifically no lists are possible. 
*/
class L1960_GenericJson
{
	protected ref map<string, ref Tuple2<ref L1960_Types, string>> content = new map<string, ref Tuple2<ref L1960_Types, string>>();
	
	/*!	
	Parses the data of this class as string.
	\return string representation
	*/
	string AsString()
	{
		string s = "";
		for (int i = 0; i < content.Count(); i++)
			s = s + content.GetKey(i) + ":" + content.GetElement(i).param2 + ", ";
		
		if (s.Length() > 0)
			return s.Substring(0, s.Length() - 2);
		else
			return "";
	}
	
	/*!	
	Return the number of Json atributes of the Json object.
	\return int number of attributes
	*/
	int Count()
	{
		return content.Count();
	}
	
	/*!	
	Return wether this objects contains any attributes.
	\return true if empty false otherwise
	*/	
	bool IsEmpty()
	{
		return content.IsEmpty();
	}

	/*!	
	Removes all data from the class.
	*/		
	void Clear()
	{
		content.Clear();
	}
	
	/*!	
	Returns a float value of the corresponding key attribute.
	\param key name of the Json attribute
	\return float value of the attriubte, 0 if it is not a float
	*/	
	float GetFloat(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_FLOAT)
			return content.Get(key).param2.ToFloat();
		return 0.0;
	}
	/*!	
	Returns an int value of the corresponding key attribute.
	\param key name of the Json attribute
	\return int value of the attriubte, 0 if it is not an int
	*/	
	int GetInt(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_INT)
			return content.Get(key).param2.ToInt();
		return 0;
	}
	/*!	
	Returns a string value of the corresponding key attribute.
	\param key name of the Json attribute
	\return string value of the attriubte, empty string if it is not a string
	*/	
	string GetString(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_STRING)
			return content.Get(key).param2;
		return string.Empty;
	}
	/*!	
	Returns a bool value of the corresponding key attribute.
	\param key name of the Json attribute
	\return bool value of the attriubte, false if it is not a bool
	*/	
	bool GetBool(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_BOOL)
			return content.Get(key).param2.ToInt() == 1;
		return false;
	}
	/*!	
	Returns a date value of the corresponding key attribute.
	\param key name of the Json attribute
	\return date value of the attriubte, null if it is not a date
	*/	
	L1960_Datetime GetDate(string key)
	{
		if (content.Get(key).param1 == L1960_Types.TYPE_DATE)
			return new L1960_Datetime(content.Get(key).param2);
		return null;
	}
	
	/*!	
	Inserts an int value with the key attribute.
	\param key Json attribute name
	\param i Json attribute value
	\return true if successfull, false otherwise
	*/	
	bool Insert(string key, int i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_INT, i.ToString()));
	}
	/*!	
	Inserts a string value with the key attribute.
	\param key Json attribute name
	\param i Json attribute value
	\return true if successfull, false otherwise
	*/	
	bool Insert(string key, string i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_STRING, i));
	}
	/*!	
	Inserts a float value with the key attribute.
	\param key Json attribute name
	\param i Json attribute value
	\return true if successfull, false otherwise
	*/
	bool Insert(string key, float i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_FLOAT, i.ToString()));
	}
	/*!	
	Inserts a bool value with the key attribute.
	\param key Json attribute name
	\param i Json attribute value
	\return true if successfull, false otherwise
	*/
	bool Insert(string key, bool i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_BOOL, i.ToString()));
	}
	/*!	
	Inserts a L1960_Datetime value with the key attribute.
	\param key Json attribute name
	\param i Json attribute value
	\return true if successfull, false otherwise
	*/
	bool Insert(string key, L1960_Datetime i)
	{
		return content.Insert(key, new Tuple2<ref L1960_Types, string>(L1960_Types.TYPE_DATE, i.AsString()));
	}
	
	/*!	
	Search for an element with the given key.
	\param key The key of the element to find
	\param val result is stored to val
	\return returns True if given key exist.
	*/
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
	/*!	
	Search for an element with the given key.
	\param key The key of the element to find
	\param val result is stored to val
	\return returns True if given key exist.
	*/
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
	/*!	
	Search for an element with the given key.
	\param key The key of the element to find
	\param val result is stored to val
	\return returns True if given key exist.
	*/
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
	/*!	
	Search for an element with the given key.
	\param key The key of the element to find
	\param val result is stored to val
	\return returns True if given key exist.
	*/
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
	
	/*!	
	Return the i:th element in the Json object.
	Note: This operation is O(n) complexity. Use with care!
	\param index The position of the element in the map
	\return The string on the i:th position or empty string
	*/
	string GetStringElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_STRING)
			return ele.param2;
		return string.Empty;
	}
	/*!	
	Return the i:th element in the Json object.
	Note: This operation is O(n) complexity. Use with care!
	\param index The position of the element in the map
	\return The int on the i:th position or 0
	*/
	int GetIntElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_INT)
			return ele.param2.ToInt();
		return 0;
	}
	/*!	
	Return the i:th element in the Json object.
	Note: This operation is O(n) complexity. Use with care!
	\param index The position of the element in the map
	\return The float on the i:th position for 0
	*/
	float GetFloatElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_FLOAT)
			return ele.param2.ToFloat();
		return 0;
	}
	/*!	
	Return the i:th element in the Json object.
	Note: This operation is O(n) complexity. Use with care!
	\param index The position of the element in the map
	\return The bool on the i:th position or false
	*/
	float GetBoolElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_BOOL)
			return ele.param2.ToInt() == 1;
		return false;
	}
	/*!	
	Return the i:th element in the Json object.
	Note: This operation is O(n) complexity. Use with care!
	\param index The position of the element in the map
	\return The L1960_Datetime on the i:th position or null
	*/
	L1960_Datetime GetDateElement(int index)
	{
		Tuple2<ref L1960_Types, string> ele = content.GetElement(index);
		
		if (ele.param1 == L1960_Types.TYPE_DATE)
			return new L1960_Datetime(ele.param2);
		return null;
	}
}