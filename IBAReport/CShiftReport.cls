VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Unity"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
Option Explicit

Private Const CurrentModule As String = "Unity"

Public Property Get Version() As Long
    Version = App.Revision
End Property

Public Sub ShowReport(ByVal nType As Long, ByVal strXML As String)
    
'    strXML = "<ShiftInfo "
'
'    strXML = strXML & "NetbarName=" & Chr(34) & "NetbarName" & Chr(34) & " "
'    strXML = strXML & "Cashier=" & Chr(34) & "Cashier" & Chr(34) & " "
'    strXML = strXML & "OnDutyTime=" & Chr(34) & "OnDutyTime" & Chr(34) & " "
'    strXML = strXML & "OffDutyTime=" & Chr(34) & "OffDutyTime" & Chr(34) & " "
'    strXML = strXML & "CreditMoney=" & Chr(34) & "123" & Chr(34) & " "
'    strXML = strXML & "ReturnMoney=" & Chr(34) & "234" & Chr(34) & " "
'    strXML = strXML & "CashSold=" & Chr(34) & "332" & Chr(34) & " "
'    strXML = strXML & "PayCard=" & Chr(34) & "555" & Chr(34) & " "
'    strXML = strXML & "StoreIn=" & Chr(34) & "3323" & Chr(34) & " "
'    strXML = strXML & "Payout=" & Chr(34) & "233" & Chr(34) & " "
'    strXML = strXML & "UseDeposit=" & Chr(34) & "1" & Chr(34) & " "
'    strXML = strXML & "Deposit=" & Chr(34) & "455" & Chr(34) & " "
'    strXML = strXML & "ShiftIn=" & Chr(34) & "344" & Chr(34) & " "
'    strXML = strXML & "LastRemain=" & Chr(34) & "434" & Chr(34) & " "
'    strXML = strXML & "AllCash=" & Chr(34) & "43434" & Chr(34) & " "
'    strXML = strXML & "NextRemain=" & Chr(34) & "4343" & Chr(34) & " "
'    strXML = strXML & "FetchMoney=" & Chr(34) & "434" & Chr(34) & " "
'    strXML = strXML & "CreditCard=" & Chr(34) & "434" & Chr(34) & " "
'    strXML = strXML & "CardSold=" & Chr(34) & "434" & Chr(34) & " "
'    strXML = strXML & "Dodonew=" & Chr(34) & "43434" & Chr(34) & " "
'    strXML = strXML & "Description=" & Chr(34) & "Description" & Chr(34) & " "
'
'    strXML = strXML & "/>"
'
'    MsgBox strXML
    
    If nType = 1 Then
        ShowShiftReport strXML
    End If
    
End Sub


Private Sub ShowShiftReport(ByVal strShiftInfo As String)
    On Error GoTo Err_Init
    
    Dim doc As DOMDocument30
    
    Set doc = New DOMDocument30
    
    If Not doc.LoadXML(strShiftInfo) Then
        MsgBox "XML有误！"
        Exit Sub
    End If

    Dim node As IXMLDOMNode, subNode As IXMLDOMNode

    Set node = doc.selectSingleNode("//ShiftInfo")
    
    If node Is Nothing Then
        MsgBox "没有ShiftInfo节点！"
        Exit Sub
    End If
    
    Dim sr As ShiftReport
    Set sr = New ShiftReport
    
    With sr
    
        Set subNode = node.Attributes.getNamedItem("NetbarName")
        .lblNetBarName = subNode.nodeValue '& "下班交款报表"
        
        Set subNode = node.Attributes.getNamedItem("Cashier")
        .lblShiftOperator = "收银员:" & subNode.nodeValue
        .lblTime = "查询时间：" & Now
        
         Set subNode = node.Attributes.getNamedItem("OnDutyTime")
        .fldOnDutyTime.Text = subNode.nodeValue
 
        Set subNode = node.Attributes.getNamedItem("OffDutyTime")
        .fldOffDutyTime.Text = subNode.nodeValue
        
        Set subNode = node.Attributes.getNamedItem("CreditMoney")
        .fldCredit.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("ReturnMoney")
        .fldReturn.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("CashSold")
        .fldCashSold.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("PayCard")
        .fldPayCard.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("StoreIn")
        .fldStoreIn.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("Payout")
        .fldPayout.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")

        Set subNode = node.Attributes.getNamedItem("UseDeposit")

        If CLng(subNode.nodeValue) = 0 Then '不使用押金
            .fldDeposit.Visible = False
            .lblDeposit.Visible = False
            .Label10.Width = 7600
        Else
            Set subNode = node.Attributes.getNamedItem("Deposit")
            .fldDeposit.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        End If
        
        Set subNode = node.Attributes.getNamedItem("ShiftIn")
        .fldShiftIn.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("LastRemain")
        .fldRemain.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("AllCash")
        .fldAllCash.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("NextRemain")
        .fldNextRemain.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("FetchMoney")
        .fldFetch.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("CreditCard")
        .fldCreditCard.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("CardSold")
        .fldCardSold.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("Dodonew")
        .fldDdn.Text = Format(CLng(subNode.nodeValue) / 100, "0.00")
        
        Set subNode = node.Attributes.getNamedItem("Description")
        .fldDescription.Text = subNode.nodeValue

    End With
 
    Set doc = Nothing
 
    frmActiveReports.ARViewer1.Object = sr
    frmActiveReports.Show vbModal
    
    Set sr = Nothing
    
Ext_Init:
    On Error Resume Next
    
    Exit Sub

Err_Init:
    Call HandleError(CurrentModule, "ShowShiftReport", Err.Number, Err.Description)
    Err.Clear
    Resume Ext_Init
End Sub



Private Sub HandleError(ParamArray ErrMsg() As Variant)
    Dim strMsg As String
    strMsg = strMsg & "模块：" & ErrMsg(0) & vbCrLf
    strMsg = strMsg & "函数：" & ErrMsg(1) & vbCrLf
    strMsg = strMsg & "序号：" & ErrMsg(2) & vbCrLf
    strMsg = strMsg & "描述：" & ErrMsg(3) & vbCrLf
    If UBound(ErrMsg()) >= 4 Then
        strMsg = strMsg & "信息：" & ErrMsg(4) & vbCrLf
    End If
    Call MsgBox(strMsg, vbCritical, "出错信息")
End Sub
