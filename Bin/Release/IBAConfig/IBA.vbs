'IBA脚本

'IBA 全局对象

'AddSystemConfig(LPCTSTR strKeyType, LPCTSTR strKeyName, LPCTSTR strKeyString, 
'INT nKeyValue1, INT nKeyValue2, LPCTSTR strKeyValue3, LPCTSTR strKeyDescription)

Sub UpdateDB()
	Call AddSystemConfig("xxx", "老板密码", "BossPwd", 0, 0, "6CC20790A53320DC85723029796DB837", "老板密码")
	'MsgBox "UpdateDB"
End Sub