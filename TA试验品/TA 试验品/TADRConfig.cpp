//config class

class TADRConfig 
{
public:
	TADRConfig ()
	{
		;
	}
	~TADRConfig ()
	{
	 ;
	}

	BOOL GetIniBool (LPCSTR ConfigName);
	int GetGetInt (LPCSTR ConfigName, int DefaultValue);
};

BOOL TADRConfig::GetIniBool (LPCSTR ConfigName)
{

}

int TADRConfig::GetGetInt (LPCSTR ConfigName)
{

}