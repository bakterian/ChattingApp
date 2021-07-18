
// ChattingAppDlg.h : header file
//

#pragma once
#include "InternalUtilTypes.h"
#include "DialogSpy.h"
#include "ISocketDataListener.h"
#include "SocketConnector.h"
#include "ChatDataAggregator.h"
#include "ISendOutDataListener.h"
#include "IUserDetailUpdatesListener.h"
#include <unordered_map>
#include <memory>
#include "ChatSessionManager.h"
#include "ChatMessagingMaster.h"

using namespace chatManagement;


// CChattingAppDlg dialog
class CChattingAppDlg : public CDialogEx, connectionLogic::ISocketDataListener, chatManagement::ISendOutDataListener, chatManagement::IUserDetailUpdatesListener
{
// Construction
public:
	DialogSpy m_spyUserDetailsDialog;
	DialogSpy m_spyMessagingDialog;
	ChatDataAggregator m_chatDataAggregator;
	CChattingAppDlg(CWnd* pParent = nullptr);	// standard constructor


	//virtual ~CChattingAppDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATTINGAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMessaging();
	afx_msg void OnBnClickedUserdetails();
	afx_msg void OnBnClickedClosedialogs();

	// Called on subdialog close
	void OnSubdialogClose(SubDialogEnum subDialogType);

	// ISocketDataListener mandatory functions:
	void OnClientConnected(const SOCKADDR_IN& socketAddr) final;

	void OnMessageReceived(const SOCKADDR_IN& socketAddr, const char* data, int amountOfData) final;

	// ISendOutDataListen implementations:
	void processOutgoingMessage(const sysTime& timestamp,const userAndMsgPair& userAndMsg) final;

	// IUserDetailUpdatesListener implementations:
	void processUserDetailChange(const std::string& newNick) final;

private:
	std::unique_ptr<connectionLogic::SocketConnector> m_pSocketConnector;
	ChatSessionManager								  m_chatSessionManager;
	ChatMessagingMaster								  m_chatMessagingMaster;


public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
