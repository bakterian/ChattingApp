#pragma once
#include <afxdialogex.h>
#include <map>
#include "ILiveDataListener.h"
#include "ISocketDataListener.h"
#include "SpiedDialog.h"
#include "ISendOutDataListener.h"
#include <mutex>
#include <queue>
#include <optional>

class MesssageDialog : public CDialogEx, SpiedDialog, chatManagement::ILiveDataListener
{
	DECLARE_DYNAMIC(MesssageDialog)

public:
	MesssageDialog(DialogSpy& spy);

	virtual ~MesssageDialog() = default;

	BOOL Create(UINT nID, CWnd* pWnd);

	// Incoming Chat Messages:
	void processIncomingMessage(const sysTime& timestamp, const userAndMsgPair& userAndMsg) final;

	void fillChatWithStoredData(const std::map<sysTime, userAndMsgPair>& m_msgCollection) final;

	// Incoming UserId:
	void processUserNameChanged(const std::string& newUserNickname) final;

	// Ougoing Chat Messages:
	bool subscribeForSendOutNotifactions(chatManagement::ISendOutDataListener* listener);


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString chatSendOutStr;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnTimer(UINT_PTR nIdEvent);
	void startTimer();
	CListBox m_ChatListBox;
	virtual void OnCancel();
	virtual void PostNcDestroy();

private:
	std::optional<std::pair<sysTime, userAndMsgPair>> getNextChatMsg();
	void notifySendListeners(const sysTime& timestamp, const userAndMsgPair& userAndMsg);

	std::mutex m_queueMutex;
	std::queue<std::pair<sysTime, userAndMsgPair>> m_chatMsgsToDisplayQueue;
	std::vector<chatManagement::ISendOutDataListener*> m_sendListenersCollection;
public:
	CEdit m_sendEditCtrl;
	CString m_NickmameCtrlString;
};
