#pragma once
#include <afxdialogex.h>
#include <map>
#include "ILiveDataListener.h"
#include "SpiedDialog.h"

class MessagingDialog : public CDialogEx, SpiedDialog, chatManagement::ILiveDataListener
{
	DECLARE_DYNAMIC(MessagingDialog)

public:
	MessagingDialog(DialogSpy& spy);
	BOOL Create(UINT nID, CWnd* pWnd);
	void postNcDestroy() { delete this; }
	void onCancel() { DestroyWindow(); }

	// ILiveDataListener manadtory function:
	void processIncomingMessage(sysTime& timestamp, userAndMsgPair& userAndMsg) final;

	void fillChatWithStoredData(std::map<sysTime, userAndMsgPair>& m_msgCollection);

	virtual ~MessagingDialog();

	//enum { IDD = IDD_DIALOG2 };
	enum { IDD = 130 };

	void startTimer();

	void OnTimer(UINT_PTR nIdEvent);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

