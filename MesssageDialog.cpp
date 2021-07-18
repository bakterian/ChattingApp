// MesssageDialog.cpp : implementation file
//

#include "pch.h"
#include "ChattingApp.h"
#include "MesssageDialog.h"
#include "afxdialogex.h"
#include <ctime>
#include <iomanip>
#include <algorithm>

// MesssageDialog dialog

IMPLEMENT_DYNAMIC(MesssageDialog, CDialogEx)

MesssageDialog::MesssageDialog(DialogSpy& spy):
	 SpiedDialog(spy, *this),
	 CDialogEx(IDD_DIALOG2, nullptr),
	 chatSendOutStr(),
	 m_queueMutex(),
	 m_chatMsgsToDisplayQueue(),
	 m_sendListenersCollection(),
	 m_NickmameCtrlString(_T(""))
{

}

void MesssageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WRITE, chatSendOutStr);
	DDX_Control(pDX, IDC_LIST1, m_ChatListBox);
	DDX_Control(pDX, IDC_EDIT_WRITE, m_sendEditCtrl);
	DDX_Text(pDX, IDC_NICKNAME_EDIT, m_NickmameCtrlString);
}

BOOL MesssageDialog::Create(UINT nID, CWnd* pWnd)
{
	return CDialogEx::Create(nID, pWnd);
}

void MesssageDialog::processIncomingMessage(const sysTime& timestamp,const userAndMsgPair& userAndMsg)
{
	std::scoped_lock(m_queueMutex);

	m_chatMsgsToDisplayQueue.emplace(timestamp, userAndMsg);
}

void MesssageDialog::fillChatWithStoredData(const std::map<sysTime, userAndMsgPair>& m_msgCollection)
{
	std::scoped_lock(m_queueMutex);

	// clear the current queue if needed
	if (m_chatMsgsToDisplayQueue.empty() == false)
	{
		std::queue<std::pair<sysTime, userAndMsgPair>> empty;
		std::swap(m_chatMsgsToDisplayQueue, empty);
	}

	// populate with new values
	for (auto& [key, value] : m_msgCollection)
	{
		m_chatMsgsToDisplayQueue.emplace(key, value);
	}
}

void MesssageDialog::processUserNameChanged(const std::string& newUserNickname)
{
	UpdateData(true);
	m_NickmameCtrlString = newUserNickname.c_str();
	UpdateData(false);
}

bool MesssageDialog::subscribeForSendOutNotifactions(chatManagement::ISendOutDataListener* listener)
{
	auto listenerAdded = false;

	if (std::find(m_sendListenersCollection.begin(), m_sendListenersCollection.end(), listener) == m_sendListenersCollection.end())
	{
		m_sendListenersCollection.push_back(listener);
		listenerAdded = true;
	}

	return listenerAdded;
}

BEGIN_MESSAGE_MAP(MesssageDialog, CDialogEx)
	ON_BN_CLICKED(IDC_SEND, &MesssageDialog::OnBnClickedSend)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void MesssageDialog::OnBnClickedSend()
{
	// get chat message
	CString newChatMessage;
	m_sendEditCtrl.GetWindowText(newChatMessage);

	// gather msg relevant data 
	CT2CA newChatMessageAnsiStr(newChatMessage);
	std::string userMsg{ newChatMessageAnsiStr };

	CT2CA nickAnsiString(m_NickmameCtrlString);
	std::string userId{ nickAnsiString };

	auto timestamp = std::chrono::system_clock::now();
	auto userMsgPair = std::make_pair(userId, userMsg);

	// trigger a message send
	notifySendListeners(timestamp, userMsgPair);

	// clear the send edit control
	CString emptyText{};
	m_sendEditCtrl.SetWindowTextW(emptyText);
}

void MesssageDialog::OnTimer(UINT_PTR nIdEvent)
{
	while (nIdEvent == 2)
	{
		// could potencialy memory expensive
		// but UI rendering does not block the socket operation for too long
		auto chatMsg = getNextChatMsg();

		if (!chatMsg)
		{
			// no messages to display
			break;
		}
		else
		{
			auto [time, userAndMsg] = chatMsg.value();
			auto t = std::chrono::system_clock::to_time_t(time);
			std::stringstream ss{};
			struct tm localTime;
			localtime_s(&localTime, &t);
			ss << std::put_time(&localTime, "%m/%d/%y %H:%M:%S");
			auto chatRecord = (std::string("[") + std::string(ss.str()) + std::string("] ") + userAndMsg.first + ": " + userAndMsg.second);
			CString chatRecordCstr(chatRecord.c_str());
			LPCTSTR chatRecordLpctstr = chatRecordCstr;

			UpdateData(true);
			m_ChatListBox.AddString(chatRecordLpctstr);
			UpdateData(false);
		}
	}
}

void MesssageDialog::startTimer()
{
	UINT_PTR myTimer = SetTimer(2, 100, nullptr); // timer elapses every 100 ms
}


void MesssageDialog::OnCancel()
{
	delete this;
}

void MesssageDialog::PostNcDestroy()
{
	DestroyWindow();
	//__super::PostNcDestroy();
}

std::optional<std::pair<sysTime, userAndMsgPair>>  MesssageDialog::getNextChatMsg()
{
	std::scoped_lock(m_queueMutex);

	std::optional<std::pair<sysTime, userAndMsgPair>> nextChatMsg = std::nullopt;

	if (m_chatMsgsToDisplayQueue.empty() == false)
	{
		nextChatMsg = m_chatMsgsToDisplayQueue.front();
		m_chatMsgsToDisplayQueue.pop();
	}

	return nextChatMsg;
}

void MesssageDialog::notifySendListeners(const sysTime& timestamp, const userAndMsgPair& userAndMsg)
{
	std::for_each(m_sendListenersCollection.cbegin(), m_sendListenersCollection.cend(),
		[&timestamp, &userAndMsg](auto listener) { listener->processOutgoingMessage(timestamp, userAndMsg); });
}

