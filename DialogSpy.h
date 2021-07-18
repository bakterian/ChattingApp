// DialogSpy.h : header file
//

#pragma once
#include <functional>

//forward declaration
class SpiedDialog;

class DialogSpy
{
public:
	DialogSpy(std::function<void()> closeDialogCallback);
	virtual ~DialogSpy();

	BOOL IsAlreadyPopped() const;
	CDialog* GetDlg() const;

	void OnDialogClosed();
	void CloseDialog();
	BOOL IsPopped() const;

	friend SpiedDialog;			// needed for helper to access the start and stop
private:
	CDialog* pDlg;
	void StartSpying(CDialog& dlg);
	void StopSpying();
	std::function<void()> m_closeDialogCallback;
};

