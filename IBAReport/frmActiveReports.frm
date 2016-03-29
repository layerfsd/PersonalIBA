VERSION 5.00
Object = "{A45D986F-3AAF-4A3B-A003-A6C53E8715A2}#1.0#0"; "ARVIEW2.OCX"
Begin VB.Form frmActiveReports 
   Caption         =   "嘟嘟牛收银台报表"
   ClientHeight    =   7755
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   11985
   Icon            =   "frmActiveReports.frx":0000
   LinkTopic       =   "Form1"
   MinButton       =   0   'False
   ScaleHeight     =   7755
   ScaleWidth      =   11985
   ShowInTaskbar   =   0   'False
   WindowState     =   2  'Maximized
   Begin DDActiveReportsViewer2Ctl.ARViewer2 ARViewer1 
      Height          =   4365
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   6975
      _ExtentX        =   12303
      _ExtentY        =   7699
      SectionData     =   "frmActiveReports.frx":08CA
   End
   Begin VB.Menu File 
      Caption         =   "文件(&F)"
      Begin VB.Menu File_PDF 
         Caption         =   "保存为PDF文件(&S)"
      End
      Begin VB.Menu File_Line 
         Caption         =   "-"
      End
      Begin VB.Menu File_Exit 
         Caption         =   "关闭(&X)"
      End
   End
End
Attribute VB_Name = "frmActiveReports"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Const CurrentModule As String = "frmActiveReports"

Private Sub Form_Load()
    ARViewer1.Toolbar.Tools(0).Visible = False
    ARViewer1.Toolbar.Tools(1).Visible = False
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    ARViewer1.Left = 0
    ARViewer1.Top = 0
    ARViewer1.Width = Me.ScaleWidth
    ARViewer1.Height = Me.ScaleHeight
End Sub

Private Sub File_Exit_Click()
    Unload Me
End Sub

Private Sub File_PDF_Click()

    On Error GoTo Err_Init
    
    'Dim pdf As New ActiveReportsPDFExport.ARExportPDF

    Dim pdf As Object
    
    Set pdf = CreateObject("ActiveReportsPDFExport.ARExportPDF")
    
    pdf.FileName = SaveFile(hwnd, "PDF文件(*.PDF)|*.PDF")
    
    If pdf.FileName = "" Then Exit Sub
    
    pdf.AcrobatVersion = 2
    pdf.SemiDelimitedNeverEmbedFonts = ""
    
    If ARViewer1.Pages.Count > 0 Then
        pdf.Export ARViewer1.Pages
    ElseIf Not ARViewer1.ReportSource Is Nothing Then
        If ARViewer1.ReportSource.Pages.Count > 0 Then
            pdf.Export ARViewer1.ReportSource.Pages
        End If
    End If
    
    MsgBox "保存交班报表成功！", vbInformation, Me.Caption


Ext_Init:
    On Error Resume Next
    
    Exit Sub

Err_Init:
    Call HandleError(CurrentModule, "File_PDF_Click", Err.Number, Err.Description)
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
