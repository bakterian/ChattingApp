#pragma once
#include "DialogSpy.h"
#include "SpiedDialog.h"
#include "IUserDetailUpdatesListener.h"
#include <string>
#include <vector>

// ChatUserDetailsDialog dialog

class ChatUserDetailsDialog : public CDialogEx, SpiedDialog
{
	DECLARE_DYNAMIC(ChatUserDetailsDialog)

public:
	ChatUserDetailsDialog(DialogSpy& spy);   // standard constructor

	virtual ~ChatUserDetailsDialog() = default;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DETAILS_USER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(UINT nID, CWnd* pParentWnd, std::string nickNameInit);

	virtual void OnCancel();

	virtual void PostNcDestroy();

	afx_msg void OnClickedUpdBtn();

	CEdit m_NickNameCtrl;

	CString m_NickEditStr;

	bool subscribeForUserDetailUpdateNotifications(chatManagement::IUserDetailUpdatesListener* listener);

private:
	void notifySendListeners(const std::string& newUserName);
	std::vector<chatManagement::IUserDetailUpdatesListener*> m_listenersCollection;
};
