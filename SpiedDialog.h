#pragma once

#include "DialogSpy.h"

class SpiedDialog
{
public:
	SpiedDialog(DialogSpy& spy, CDialog& dlg);

	virtual ~SpiedDialog();
private:
	DialogSpy* pDlgSpy;
};

