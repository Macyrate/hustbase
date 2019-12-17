// HustBase.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HustBase.h"

#include "MainFrm.h"
#include "HustBaseDoc.h"
#include "HustBaseView.h"
#include "TreeList.h"

#include "IX_Manager.h"
#include "PF_Manager.h"
#include "RM_Manager.h"
#include "SYS_Manager.h"
#include "atlstr.h"
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp

BEGIN_MESSAGE_MAP(CHustBaseApp, CWinApp)
	//{{AFX_MSG_MAP(CHustBaseApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CREATEDB, OnCreateDB)
	ON_COMMAND(ID_OPENDB, OnOpenDB)
	ON_COMMAND(ID_DROPDB, OnDropDb)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp construction

CHustBaseApp::CHustBaseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHustBaseApp object

CHustBaseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp initialization
bool CHustBaseApp::pathvalue = false;

BOOL CHustBaseApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CHustBaseDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CHustBaseView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CHustBaseApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHustBaseApp message handlers

void CHustBaseApp::OnCreateDB()
{
	//关联创建数据库按钮，此处应提示用户输入数据库的存储路径和名称，并调用CreateDB函数创建数据库。
	char dbpath[] = "C:\\GitHub";	//测试用
	char dbname[] = "TestDB";
	if (CreateDB(dbpath, dbname) == SUCCESS)
		AfxMessageBox("数据库创建成功！");
	else
		AfxMessageBox("数据库创建失败！");
}

void CHustBaseApp::OnOpenDB()
{
	//关联打开数据库按钮，此处应提示用户输入数据库所在位置，并调用OpenDB函数改变当前数据库路径，并在界面左侧的控件中显示数据库中的表、列信息。
	char* folderPath = GetFolderPath();		//选择数据库文件夹路径
	if (folderPath == NULL) {
		AfxMessageBox("未选择数据库！");
		return;
	}

	SetCurrentDirectory(folderPath);	//转到选择的文件夹路径
	CFileFind fileFind;
	BOOL isSystablesExist = fileFind.FindFile("SYSTABLES");		//检查选择的文件夹是否是数据库
	BOOL isColumnsExist = fileFind.FindFile("SYSCOLUMNS");

	if (isSystablesExist && isColumnsExist) {
		SetCurrentDirectory("..");
		if (OpenDB(folderPath) == SUCCESS)	//用OpenDB进行数据库启动时的工作
			AfxMessageBox("数据库启动成功！");
		else
			AfxMessageBox("数据库启动失败！");
	}
	else
		AfxMessageBox("该文件夹不是合法的数据库！");
}

void CHustBaseApp::OnDropDb()
{
	char* folderPath = GetFolderPath();		//选择数据库文件夹路径
	if (folderPath == NULL) {
		AfxMessageBox("未选择数据库！");
		return;
	}

	SetCurrentDirectory(folderPath);	//转到选择的文件夹路径
	CFileFind fileFind;
	BOOL isSystablesExist = fileFind.FindFile("SYSTABLES");		//检查选择的文件夹是否是数据库
	BOOL isColumnsExist = fileFind.FindFile("SYSCOLUMNS");

	if (isSystablesExist && isColumnsExist) {
		SetCurrentDirectory("..");
		if (DropDB(folderPath) == SUCCESS)		//用DropDB进行数据库删除时的工作
			AfxMessageBox("数据库删除成功！");
		else
			AfxMessageBox("数据库删除失败！");
	}
	else
		AfxMessageBox("该文件夹不是合法的数据库！");
}

char* CHustBaseApp::GetFolderPath() {
	IFileDialog* pfd = NULL;	//用IFileDialog接口实现打开文件夹对话框
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr)) {
		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
			pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);	//用FOS_PICKFOLDERS限为打开文件夹
		}
		if (SUCCEEDED(pfd->Show(NULL))) {
			IShellItem* psi;
			if (SUCCEEDED(pfd->GetResult(&psi)))
			{
				LPWSTR folderPath = NULL;
				if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &folderPath))) {	//获取选择的文件夹路径，类型为PWSTR
					std::string stringOfFolderPath = CW2A(folderPath);
					psi->Release();
					pfd->Release();
					const char* cstr = stringOfFolderPath.c_str();	//以const char*的字符串形式返回文件夹路径
					char* toret = (char*)malloc((stringOfFolderPath.length() + 1) * sizeof(char));
					strcpy(toret, cstr);
					return toret;
				}
			}
			psi->Release();
		}
		pfd->Release();
	}
	return NULL;
}