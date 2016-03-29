Attribute VB_Name = "MCommon"
Option Explicit

Public Function AnsiToUnicode(ByVal strAnsi As String) As String
    AnsiToUnicode = StrConv(strAnsi, vbUnicode)
    AnsiToUnicode = TrimNull(AnsiToUnicode)
End Function

Public Function UnicodeToAnsi(ByVal strUnicode As String) As String
    UnicodeToAnsi = StrConv(IIf((Len(strUnicode) Mod 2) = 1, strUnicode & " ", strUnicode), vbFromUnicode)
End Function

Public Function TrimNull(ByVal Str As String) As String
    Dim pos As Long
    pos = InStr(1, Str, Chr(0))
    If pos > 0 Then Str = Left(Str, pos - 1)
    TrimNull = Trim$(Str)
End Function

Public Function GetStrPtr(ByVal Str As String) As Long
    GetStrPtr = StrPtr(UnicodeToAnsi(Str))
End Function

Public Function SaveFile(ByVal hwnd As Long, Optional ByVal dlgFilter As String = "", _
    Optional ByVal dlgInitDir As String = "", Optional ByVal dlgTitle As String = "保存") As String
    On Error Resume Next
    Dim OFName As OPENFILENAME, sFileName As String
    sFileName = Space(cMaxPath)
    If dlgFilter = "" Then
        dlgFilter = "所有文件(*.*)" & vbNullChar & "*.*" & vbNullChar
    Else
        dlgFilter = Replace(dlgFilter, "|", vbNullChar)
        dlgFilter = dlgFilter & Chr(0) & "所有文件(*.*)" & vbNullChar & "*.*"
    End If
    
    dlgFilter = UnicodeToAnsi(dlgFilter)
    With OFName
        '设置结构的大小
        .lStructSize = LenB(OFName)
        '设置父窗口
        .hwndOwner = hwnd
        '设置程序的实例
        .hInstance = App.hInstance
        '设置过滤属性
        .nMaxCustFilter = cMaxPath
        .lpstrFilter = StrPtr(dlgFilter)
        '设置默认扩展名
        .lpstrFile = StrPtr(sFileName)
        '设置返回的文件（全路径）的最大长度
        .nMaxFile = cMaxPath
        '为文件名称创建缓冲区
        '.lpstrFileTitle = String(cMAXPATH, Chr(0))
        '设置返回的文件名称的最大长度
        .nMaxFileTitle = cMaxPath
        
        .lpstrDefExt = StrPtr("")
        
        If dlgInitDir <> "" Then
            .lpstrInitialDir = GetStrPtr(dlgInitDir & vbNullChar)
        End If
        '设置对话框标题
        .lpstrTitle = GetStrPtr(dlgTitle)
        .flags = &H80000 Or &H2 Or &H1000 Or OFN_EXTENSIONDIFFERENT
    End With
    If GetSaveFileName(OFName) Then
        sFileName = AnsiToUnicode(sFileName)
        SaveFile = sFileName
    Else
        SaveFile = ""
    End If
End Function

