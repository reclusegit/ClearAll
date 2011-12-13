// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(VFC_MAINDLG_H__558ED513_68A6_422c_8675_7BDA782AC993__INCLUDED_)
#define VFC_MAINDLG_H__558ED513_68A6_422c_8675_7BDA782AC993__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"
#include "PictureExWnd.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <map>

#define CFGINFO_CONTEXT _T("RegCfg.txt")

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(ID_STARTCHECK, BN_CLICKED, OnBnClickedOk)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		m_Gif.SubclassWindow( GetDlgItem(IDC_CHECKING) );
		m_Text = GetDlgItem( IDS_TEXT );	

		InitDlgUI();

		return TRUE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
		//dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	void InitDlgUI()
	{
		// ���� IDR_DEFAULT gif ͼƬ����ʾ
		if ( m_Gif.Load( MAKEINTRESOURCE( IDR_DEFAULT), _T("GIF") ) )
		{
			m_Gif.Draw();
		}
	}

	void StartChecking()
	{
		// ��ʾ IDR_CHECKING gif ͼƬ
		m_Gif.UnLoad();		
		if ( m_Gif.Load( MAKEINTRESOURCE(IDR_CHECKING), _T("GIF") ) )
		{
			m_Gif.Draw();
		}

		m_Text.SetWindowText( _T("���ڼ��,���Ե�...") );
	}

	BOOL ClearAllTraces()
	{
		if ( !GetCfgInfo() )
			return FALSE;

		for ( auto itor = m_RegCfgInfo.begin(); itor != m_RegCfgInfo.end(); ++itor )
		{
			if ( !DeleteTree( (*itor).first, Ansi2WChar( (*itor).second.c_str(),(*itor).second.length() ) ) )			
				return FALSE;			
		}		
		for ( auto itor = m_RegCfgInfo.begin(); itor != m_RegCfgInfo.end(); ++itor )
		{
			HKEY hKey;
			if( ERROR_SUCCESS == RegOpenKeyEx( (*itor).first, Ansi2WChar( (*itor).second.c_str(),(*itor).second.length() ), 0,
					KEY_ALL_ACCESS, &hKey ) )
			{
				RegDeleteKey( (*itor).first, Ansi2WChar( (*itor).second.c_str(), (*itor).second.length() ) );							
				RegCloseKey( hKey );
			}
		}
		return TRUE;
	}

	void FinishChecking()
	{
		// ��ʾ IDR_COMPLETE gif ͼƬ
		m_Gif.UnLoad();
		if (m_Gif.Load( MAKEINTRESOURCE(IDR_COMPLETE), _T("GIF") ) )
		{
			m_Gif.Draw();			
		}

		m_Text.SetWindowText( _T("������ϣ�ллʹ�ã�") );
	}

	void DisplayError()
	{
		// ��ʾ IDR_ERROR gif ͼƬ
		m_Gif.UnLoad();
		if (m_Gif.Load( MAKEINTRESOURCE(IDR_ERROR), _T("GIF") ) )
		{
			m_Gif.Draw();			
		}

		m_Text.SetWindowText( _T("��������г��ִ���") );
	}

	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: �ڴ���ӿؼ�֪ͨ����������
		StartChecking();
		
		if ( !ClearAllTraces() )
		{
			DisplayError();
			return 0;
		}
		
		FinishChecking();
		
		return 0;
	}

	void GetModuleDiretory(CString &str)
	{
		wchar_t pszFileLoc[_MAX_PATH] = {0};
		GetModuleFileName(NULL, (LPWSTR)pszFileLoc, sizeof(pszFileLoc) - 1);
		wchar_t drive[_MAX_DRIVE]  = {0};
		wchar_t dir[_MAX_DIR]	   = {0};
		wchar_t fname[_MAX_FNAME]  = {0};
		wchar_t ext[_MAX_EXT]	   = {0};
		_wsplitpath_s(pszFileLoc, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT); //�ֽ�·��
		wchar_t exeDir[_MAX_PATH]  = {0};
		wsprintf(exeDir, _T("%s%s"), drive, dir);

		str.Format(_T("%s"), exeDir);
	}

	//Converting a Ansi string to WChar string

	_TCHAR * Ansi2WChar(LPCSTR pszSrc, int nLen)
	{
		int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
		if(nSize <= 0) return NULL;

		_TCHAR *pwszDst = new _TCHAR[nSize+1];
		if( NULL == pwszDst) return NULL;

		MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
		pwszDst[nSize] = 0;

		if( pwszDst[0] == 0xFEFF)                    // skip Oxfeff
			for(int i = 0; i < nSize; i ++) 
				pwszDst[i] = pwszDst[i+1]; 

		return pwszDst;
	}

	BOOL GetCfgInfo()
	{
		CString sPath;
		GetModuleDiretory( sPath );
		sPath += CFGINFO_CONTEXT;
		if ( _waccess( sPath.GetBuffer( sPath.GetLength() ), 0 ) != 0 )
		{
			tstring strErrorInfo;
			strErrorInfo.append( _T("can not find the RegCfg.txt") );
			MessageBox( strErrorInfo.c_str(), _T("ERROR"), MB_ICONERROR | MB_OK );
			return FALSE;
		}

		fstream in_file( sPath.GetBuffer( sPath.GetLength() ) );
		string strLine;
		vector< string > vec;		
		while( getline( in_file,strLine ) )
			vec.push_back( strLine );
		sort( vec.begin(), vec.end() ); // ��������
		auto itor = unique( vec.begin(), vec.end() ); // �ƶ��ظ������
		vec.erase( itor, vec.end() );  // ɾ���ظ�
		
		if ( vec.empty() )
		{
			tstring strErrorInfo;
			strErrorInfo.append( _T("�뽫��Ҫɾ���� ������ ���뱾Ŀ¼ ��RegCfg.txt�� �ļ��� ") );
			MessageBox( strErrorInfo.c_str(), _T("ERROR"), MB_ICONERROR | MB_OK );
			return FALSE;
		}

		for ( auto itor = vec.begin(); itor != vec.end(); ++itor )
		{
			HKEY hKey;
			int iRootKeyWidth = (*itor).find( "\\" );

			// �ַ��� "HKEY_LOCAL_MACHINE" TO HKEY			
			string strKey = (*itor).substr( 0, iRootKeyWidth );

			if ( 0 == strKey.compare( "HKEY_LOCAL_MACHINE" ) )
				hKey = HKEY_LOCAL_MACHINE;
			else if ( 0 == strKey.compare( "HKEY_CLASSES_ROOT" ) )
				hKey = HKEY_CLASSES_ROOT;
			else if ( 0 == strKey.compare( "HKEY_USERS" ) )
				hKey = HKEY_USERS;
			else if( 0 == strKey.compare( "HKEY_CURRENT_USER" ) )
				hKey = HKEY_CURRENT_USER;
			else  
				hKey = HKEY_CURRENT_CONFIG;

			string strValue = (*itor).substr( iRootKeyWidth + 1 );
						
			m_RegCfgInfo.insert( make_pair( hKey, strValue ) );
		}

		return TRUE;
	}

	/************************************************************************
	���ܣ��ݹ�ɾ��ע����Ӽ�(�������䱾��)
	������RootKey	   -> ����
		 strSubKey -> Ҫɾ�����Ӽ�
	����ֵ��TRUE	   -> ɾ���ɹ�
		  FALSE	   -> ɾ���г���
	************************************************************************/
	BOOL DeleteTree(HKEY RootKey, _TCHAR *pSubKey)
	{
		HKEY hKey;
		DWORD nRet;
		DWORD NameCnt,NameMaxLen;
		DWORD KeyCnt,KeyMaxLen,MaxDateLen;
				
		static int dwDeep = -1;
		dwDeep++;
		nRet=RegOpenKeyEx(RootKey,pSubKey,0,KEY_ALL_ACCESS,&hKey);
		if(nRet!=ERROR_SUCCESS)
		{
			tstring strErrorInfo;
			strErrorInfo.append( _T("can not find the ") );
			 			
			if ( HKEY_CLASSES_ROOT == RootKey )
			 	strErrorInfo.append( _T("HKEY_CLASSES_ROOT\\") );
			if ( HKEY_CURRENT_CONFIG == RootKey )
			 	strErrorInfo.append( _T("HKEY_CURRENT_CONFIG\\") );
			if ( HKEY_CURRENT_USER == RootKey )
			 	strErrorInfo.append( _T("HKEY_CURRENT_USER\\") );
			if ( HKEY_LOCAL_MACHINE == RootKey )
			 	strErrorInfo.append( _T("HKEY_LOCAL_MACHINE\\") );
			if ( HKEY_USERS == RootKey )
			 	strErrorInfo.append( _T("HKEY_USERS\\") );
			 	strErrorInfo.append( pSubKey );
			 
			//MessageBox( strErrorInfo.c_str(), _T("ERROR"), MB_ICONERROR | MB_OK );
			ATLTRACE( _T("%s"), strErrorInfo.c_str() );

			return TRUE;			
		}
		nRet = RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,
			&NameMaxLen,&MaxDateLen,NULL,NULL);
		if(nRet == ERROR_SUCCESS)
		{
			for(int dwIndex = KeyCnt - 1; dwIndex >= 0; dwIndex--)  //ö�ټ�ֵ
			{
				_TCHAR sKeyName[256] = { 0 };
				RegEnumKey(hKey, dwIndex, sKeyName, sizeof(sKeyName));
				HKEY hKeySub;
				DWORD KeyCntSub;
				_TCHAR pSubKeyTemp[256] = { 0 };
				wcscpy(pSubKeyTemp, pSubKey);
				wcscat(pSubKeyTemp, L"\\");
				wcscat(pSubKeyTemp, sKeyName);

				nRet = RegOpenKeyEx(RootKey,pSubKeyTemp,0,KEY_ALL_ACCESS,&hKeySub);
				if(nRet == ERROR_SUCCESS)
				{
					nRet = RegQueryInfoKey(hKeySub,NULL,NULL,NULL,&KeyCntSub,&KeyMaxLen,NULL,&NameCnt,
						&NameMaxLen,&MaxDateLen,NULL,NULL);
					if(nRet == ERROR_SUCCESS)
					{
						if (KeyCntSub != 0)
						{
							DeleteTree(RootKey, pSubKeyTemp);
						}
						RegCloseKey(hKeySub);
					}
				}
				RegDeleteKey(RootKey ,pSubKeyTemp);
			}
			RegCloseKey(hKey);
		}
		
		if (dwDeep == 0)
		{
			RegDeleteKey(RootKey ,pSubKey);
		}
		return TRUE;
	}

private:
	CPictureExWnd	m_Gif;
	CStatic			m_Text;
	multimap< HKEY, string > m_RegCfgInfo;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// VisualFC AppWizard will insert additional declarations immediately before the previous line.

#endif // !defined(VFC_MAINDLG_H__558ED513_68A6_422c_8675_7BDA782AC993__INCLUDED_)
