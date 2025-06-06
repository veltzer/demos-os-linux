/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <wx/wx.h>
#include <wx/frame.h>

/*
 * This example is a simple wxWindows application.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-cast-function-type -Wno-deprecated-copy
 * EXTRA_COMPILE_CMD=wx-config --cflags
 * EXTRA_LINK_CMD=wx-config --libs
 */

class MyApp: public wxApp {
	virtual bool OnInit(void);

// virtual int OnRun(void);
};
IMPLEMENT_APP(MyApp)

class MyFrame: public wxFrame {
public:
	MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

enum {
	ID_Quit=1,
	ID_About
};

bool MyApp::OnInit(void) {
	MyFrame *frame=new MyFrame(_T("Hello World"), wxPoint(50, 50), wxSize(450, 350));

	frame->Connect(ID_Quit, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction) & MyFrame::OnQuit);
	frame->Connect(ID_About, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction) & MyFrame::OnAbout);

	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame((wxFrame *)NULL, -1, title, pos, size) {
	// create menubar
	wxMenuBar *menuBar=new wxMenuBar;
	// create menu
	wxMenu *menuFile=new wxMenu;

	// append menu entries
	menuFile->Append(ID_About, _T("&About..."));
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, _T("E&xit"));
	// append menu to menubar
	menuBar->Append(menuFile, _T("&File"));
	// set frame menubar
	SetMenuBar(menuBar);

	// create frame statusbar
	CreateStatusBar();
	// set statusbar text
	SetStatusText(_T("Welcome to wxWindows!"));
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxMessageBox(_T("wxWindows Hello World example."), _T("About Hello World"), wxOK | wxICON_INFORMATION, this);
}
