#include "pch.h"
#include "SpiedDialog.h"


// Helper definitions 

SpiedDialog::SpiedDialog(DialogSpy& spy, CDialog& dlg)
{
	pDlgSpy = &spy;
	pDlgSpy->StartSpying(dlg);
}

SpiedDialog::~SpiedDialog()
{
	pDlgSpy->StopSpying();
}

