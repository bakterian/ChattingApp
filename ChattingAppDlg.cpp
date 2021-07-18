
// ChattingAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ChattingApp.h"
#include "ChattingAppDlg.h"
#include "afxdialogex.h"
#include "MesssageDialog.h"
#include "ChatUserDetailsDialog.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChattingAppDlg dialog



CChattingAppDlg::CChattingAppDlg(CWnd* pParent /*=nullptr*/) : 
	CDialogEx(IDD_CHATTINGAPP_DIALOG, pParent),
	m_spyUserDetailsDialog(std::bind(&CChattingAppDlg::OnSubdialogClose, this, SubDialogEnum::UserDetailsDialog)),
	m_spyMessagingDialog(std::bind(&CChattingAppDlg::OnSubdialogClose, this, SubDialogEnum::MsgDialog)),
	m_pSocketConnector(nullptr),
	m_chatDataAggregator(),
	m_chatSessionManager()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChattingAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChattingAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Messaging, &CChattingAppDlg::OnBnClickedMessaging)
	ON_BN_CLICKED(IDC_UserDetails_, &CChattingAppDlg::OnBnClickedUserdetails)
	ON_BN_CLICKED(IDC_CloseDialogs, &CChattingAppDlg::OnBnClickedClosedialogs)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CChattingAppDlg message handlers

BOOL CChattingAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	/*  
		Socket Connection to be initiated here:
		Run a server or client depending what is happening on the Port
	*/

	m_pSocketConnector = std::make_unique<connectionLogic::SocketConnector>();

	/* This function check if connection can be established and start either in client or server mode */

	auto initSucceded = m_pSocketConnector->init();
	if (initSucceded == false)
	{
		return FALSE;
	}

	/* within the start context a message may likely be received therefore subscribing already before starting*/
	m_pSocketConnector->subscribeForNotifications(this);
	auto connectionStarted = m_pSocketConnector->start();
	if (connectionStarted == false)
	{
		return FALSE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChattingAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChattingAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChattingAppDlg::OnBnClickedMessaging()
{
	if(m_spyMessagingDialog.IsAlreadyPopped() == false)
	{ 
		MesssageDialog* pNewDialog = new MesssageDialog(m_spyMessagingDialog);
		pNewDialog->Create(IDD_DIALOG2, NULL);
		pNewDialog->startTimer();
		pNewDialog->subscribeForSendOutNotifactions(this);
		m_chatDataAggregator.removeAllListeners();
		m_chatDataAggregator.subscribeForLiveDataNotifications((ILiveDataListener*)pNewDialog);
		m_chatDataAggregator.initlizeChat();
	}
}

void CChattingAppDlg::OnBnClickedUserdetails()
{
	if (m_spyUserDetailsDialog.IsAlreadyPopped() == false)
	{
		ChatUserDetailsDialog* pNewDialog = new ChatUserDetailsDialog(m_spyUserDetailsDialog);
		auto res = pNewDialog->Create(IDD_DETAILS_USER, NULL, m_chatDataAggregator.getLocalNickname());
		
		pNewDialog->subscribeForUserDetailUpdateNotifications(this);
	}
}

void CChattingAppDlg::OnBnClickedClosedialogs()
{
	m_spyMessagingDialog.CloseDialog();
	m_spyUserDetailsDialog.CloseDialog();
}

void CChattingAppDlg::OnSubdialogClose(SubDialogEnum subDialogType)
{
	// un-attach the dialog listeners here

	if (subDialogType == SubDialogEnum::MsgDialog)
	{
		m_chatDataAggregator.removeAllListeners();
	}

	else if (subDialogType == SubDialogEnum::UserDetailsDialog)
	{
		
	}
}

void CChattingAppDlg::OnClientConnected(const SOCKADDR_IN& socketAddr)
{
	// Function to be called only if this is a server instance

	auto isNewClient = m_chatSessionManager.clientConnected(socketAddr);

	// new client would need to get the whole messsage history:
	if (isNewClient && m_chatDataAggregator.isMsgCollectionDirty())
	{
		// send a dedicated ChatMessageBatch (serialized time, user, msg) command to socketAddr
		auto chatEntries = m_chatDataAggregator.getChatEntries();

		auto serializedMsg = m_chatMessagingMaster.getChatBatchMsgStr(chatEntries);

		// send to the new client
		const std::vector<SOCKADDR_IN> socketRecepients { socketAddr };
		m_pSocketConnector->sendOnSocket(serializedMsg, socketRecepients);
	}
}

void CChattingAppDlg::OnMessageReceived(const SOCKADDR_IN& socketAddr, const char* data, int amountOfData)
{
	// TODO the incoming data to be processed in ChatMessagingMaster
	// 1. The component should detemine the type of the message internaly
	// 2. the component to deliver a optional of std::pair Btime and UsrMsgPair ( in case of the msg command it will make timestamps)
	// 3. A call to m_chatDataAggregator.processData(timestamp, userAndMsg) to be made
	// 4. m_chatDataAggregator.processRawData to be removed.

	/*
	bool isValidMsg = m_chatDataAggregator.processRawData(socketAddr, data, amountOfData);

	// Msg Forwarding to other client will happen only if we have at least two clients connected
	if (isValidMsg && m_chatSessionManager.areThereMutlipleClients())
	{
		const auto& recepients = m_chatSessionManager.getClientsForMsgForward(socketAddr);

		if (recepients.has_value())
		{
			std::string rawChatData(data, amountOfData);

			m_pSocketConnector->sendOnSocket(rawChatData, recepients.value());
		}
	}
	*/

	auto chatDataOpt = m_chatMessagingMaster.processRawData(data, amountOfData);

	if (chatDataOpt)
	{
		auto chatData = chatDataOpt.value();

		m_chatDataAggregator.processDataMul(chatData);

		if (m_pSocketConnector->getConnectionType() == connectionLogic::SocketConType::Server)
		{
			auto& [usr, msg] = chatData.begin()->second;
			m_chatDataAggregator.updateUserIdsMap(socketAddr, usr);
		}

		// Msg Forwarding to other client will happen only if we have at least two clients connected
		if (m_chatSessionManager.areThereMutlipleClients())
		{
			const auto& recepients = m_chatSessionManager.getClientsForMsgForward(socketAddr);

			if (recepients.has_value())
			{
				std::string rawChatData(data, amountOfData);

				m_pSocketConnector->sendOnSocket(rawChatData, recepients.value());
			}
		}
	}
}

void CChattingAppDlg::processOutgoingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg)
{
	if (!m_pSocketConnector)
	{
		// no message processing with unavailable socket.
		return;
	}

	// update internal storage and display in main chat control
	m_chatDataAggregator.processData(timestamp, userAndMsg);

	auto serializedMsg = m_chatMessagingMaster.getChatMsgStr(userAndMsg);

	m_pSocketConnector->sendOnSocketToAll(serializedMsg);
}

void CChattingAppDlg::processUserDetailChange(const std::string& newNick)
{
	const bool isLocalUser = true;
	m_chatDataAggregator.processUserNameChange(newNick, isLocalUser);
}

void CChattingAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	__super::OnSysCommand(nID, lParam);
}
