// ChatUserDetailsDialog.cpp : implementation file
//

#include "pch.h"
#include "ChattingApp.h"
#include "ChatUserDetailsDialog.h"
#include "afxdialogex.h"


// ChatUserDetailsDialog dialog

IMPLEMENT_DYNAMIC(ChatUserDetailsDialog, CDialogEx)

ChatUserDetailsDialog::ChatUserDetailsDialog(DialogSpy& spy) : 
	SpiedDialog(spy, *this),
	CDialogEx(IDD_DETAILS_USER, nullptr),
	m_listenersCollection(),
	m_NickEditStr(_T(""))
{

}

void ChatUserDetailsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_NickNameCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_NickEditStr);
}

BEGIN_MESSAGE_MAP(ChatUserDetailsDialog, CDialogEx)
	ON_BN_CLICKED(IDC_UPD_BTN, &ChatUserDetailsDialog::OnClickedUpdBtn)
END_MESSAGE_MAP()

BOOL ChatUserDetailsDialog::Create(UINT nID, CWnd* pParentWnd, std::string nickNameInit)
{
	auto res = CDialogEx::Create(nID, pParentWnd);

	// setting a defult or last stored nickname
	m_NickEditStr = nickNameInit.c_str();
	m_NickNameCtrl.SetWindowTextW(m_NickEditStr);

	return res;
}

void ChatUserDetailsDialog::OnCancel()
{
	delete this;
}

void ChatUserDetailsDialog::PostNcDestroy()
{
	DestroyWindow();
}

void ChatUserDetailsDialog::OnClickedUpdBtn()
{
	m_NickNameCtrl.GetWindowText(m_NickEditStr);

	CT2CA newNickNameAnsiStr(m_NickEditStr);
	std::string newNickName{ newNickNameAnsiStr };

	notifySendListeners(newNickName);
}

bool ChatUserDetailsDialog::subscribeForUserDetailUpdateNotifications(chatManagement::IUserDetailUpdatesListener* listener)
{
	auto listenerAdded = false;

	if (std::find(m_listenersCollection.begin(), m_listenersCollection.end(), listener) == m_listenersCollection.end())
	{
		m_listenersCollection.push_back(listener);
		listenerAdded = true;
	}

	return listenerAdded;
}

void ChatUserDetailsDialog::notifySendListeners(const std::string& newUserName)
{
	std::for_each(m_listenersCollection.cbegin(), m_listenersCollection.cend(),
		[&newUserName](auto listener) { listener->processUserDetailChange(newUserName); });
}
