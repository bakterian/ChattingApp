#include "pch.h"
#include "DialogSpy.h"

DialogSpy::DialogSpy(std::function<void()> m_closeDialogCallback) :
	pDlg(NULL),
	m_closeDialogCallback(m_closeDialogCallback)
{
}

DialogSpy::~DialogSpy()
{
	CloseDialog();
}
CDialog* DialogSpy::GetDlg() const
{
	return (pDlg);
}
void DialogSpy::CloseDialog()
{
	if (IsPopped())
	{
		GetDlg()->DestroyWindow();
	}
}
BOOL DialogSpy::IsPopped() const
{
	return (pDlg != NULL);
}
BOOL DialogSpy::IsAlreadyPopped() const
{
	if (IsPopped())
	{
		pDlg->SetActiveWindow();
		return TRUE;
	}
	return FALSE;
}

void DialogSpy::OnDialogClosed()
{
	// notify main dialog from here
	m_closeDialogCallback();
}

void DialogSpy::StartSpying(CDialog& dlg)
{
	pDlg = &dlg;
}
void DialogSpy::StopSpying()
{
	pDlg = NULL;

	OnDialogClosed();			// allows for overriding and getting notification of a closing.
}
