#pragma once

enum class eStatType
{
	INT             = 1,
	FLOAT           = 2,
	STRING          = 3,
	BOOL_           = 4,
	UINT8           = 5,
	UINT16          = 6,
	UINT32          = 7,
	UINT64          = 8,
	INT64           = 26,
	DATE            = 20,
	POS             = 21,
	TEXTLABEL       = 22,
	PACKED          = 23,
	USERID          = 24,
	PROFILE_SETTING = 25,
};

class sStatData
{
public:
	virtual void destructor()                    = 0;
	virtual void _1()                            = 0;
	virtual void SetIntData(int data)            = 0;
	virtual void SetInt64Data(int64_t data)      = 0;
	virtual void SetFloatData(float data)        = 0;
	virtual void SetBoolData(bool data)          = 0;
	virtual void SetUint8Data(uint8_t data)      = 0;
	virtual void SetUint16Data(uint16_t data)    = 0;
	virtual void SetUint32Data(uint32_t data)    = 0;
	virtual void SetUint64Data(uint64_t data)    = 0;
	virtual void SetStringData(const char* data) = 0;
	virtual void _11()                           = 0;
	virtual int GetIntData()                     = 0;
	virtual int64_t GetInt64Data()               = 0;
	virtual float GetFloatData()                 = 0;
	virtual bool GetBoolData()                   = 0;
	virtual uint8_t GetUint8Data()               = 0;
	virtual uint16_t GetUint16Data()             = 0;
	virtual uint32_t GetUint32Data()             = 0;
	virtual uint64_t GetUint64Data()             = 0;
	virtual const char* GetStringData()          = 0;
	virtual void _21()                           = 0;
	virtual void _22()                           = 0;
	virtual void _23()                           = 0;
	virtual eStatType GetTypeId()                = 0;
	virtual const char* GetTypeString()          = 0;
	virtual void _26()                           = 0;
	virtual bool IsObfuscated()                  = 0;
	virtual void _28()                           = 0;
	virtual void _29()                           = 0;
	virtual bool IsZero()                        = 0;

	uint64_t m_flags;
};

// Not a r* class
template<typename T>
class sSubStatData : public sStatData
{
public:
	T m_data;
};