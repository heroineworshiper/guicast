/*
 * CINELERRA
 * Copyright (C) 2008-2024 Adam Williams <broadcast at earthling dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include "condition.h"
#include "bcfilebox.h"
#include "bcnewfolder.h"
#include "bcresources.h"
#include "bctheme.h"
#include "bctitle.h"
#include "filesystem.h"
#include "language.h"
#include "mutex.h"

#include <sys/stat.h>




#define WINDOW_W DP(320)
#define WINDOW_H DP(150)

BC_NewFolder::BC_NewFolder(int x, int y, BC_FileBox *filebox)
 : BC_Window(filebox->get_newfolder_title(), 
 	x, 
	y, 
	WINDOW_W, 
	WINDOW_H, 
	0, 
	0, 
	0, 
	0, 
	1)
{
}

BC_NewFolder::~BC_NewFolder()
{
}


void BC_NewFolder::create_objects()
{
    int margin = BC_Resources::theme->widget_border;
	int x = margin, y = margin;
	lock_window("BC_NewFolder::create_objects");
    BC_Title *text;
	add_tool(text = new BC_Title(x, y, _("Enter the name of the folder:")));
	y += text->get_h() + margin;
	add_subwindow(textbox = new BC_TextBox(x, 
        y, 
        get_w() - margin * 2, 
        1, 
        _("Untitled")));
	y += textbox->get_h() + margin;
	add_subwindow(new BC_OKButton(x, y));
	x = get_w() - BC_CancelButton::calculate_w() - margin;
	add_subwindow(new BC_CancelButton(x, y));
	show_window();
	unlock_window();
}

const char* BC_NewFolder::get_text()
{
	return textbox->get_text();
}


BC_NewFolderThread::BC_NewFolderThread(BC_FileBox *filebox)
 : Thread(0, 0, 0)
{
	this->filebox = filebox;
	window = 0;
	change_lock = new Mutex("BC_NewFolderThread::change_lock");
	completion_lock = new Condition(1, "BC_NewFolderThread::completion_lock");
}

BC_NewFolderThread::~BC_NewFolderThread() 
{
 	interrupt();
	delete change_lock;
	delete completion_lock;
}

void BC_NewFolderThread::run()
{
	int x = filebox->get_abs_cursor_x(1);
	int y = filebox->get_abs_cursor_y(1);
	change_lock->lock("BC_NewFolderThread::run 1");
	window = new BC_NewFolder(x, 
		y,
		filebox);
	window->create_objects();
	change_lock->unlock();


	int result = window->run_window();

	if(!result)
	{
		char new_folder[BCTEXTLEN];
		filebox->fs->join_names(new_folder, filebox->fs->get_current_dir(), window->get_text());
		mkdir(new_folder, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		filebox->lock_window("BC_NewFolderThread::run");
		filebox->refresh(0, 1);
		filebox->unlock_window();
	}

	change_lock->lock("BC_NewFolderThread::run 2");
	delete window;
	window = 0;
	change_lock->unlock();

	completion_lock->unlock();
}

int BC_NewFolderThread::interrupt()
{
	change_lock->lock("BC_NewFolderThread::interrupt");
	if(window)
	{
		window->lock_window("BC_NewFolderThread::interrupt");
		window->set_done(1);
		window->unlock_window();
	}

	change_lock->unlock();

	completion_lock->lock("BC_NewFolderThread::interrupt");
	completion_lock->unlock();
	return 0;
}

int BC_NewFolderThread::start_new_folder()
{
	change_lock->lock();

	if(window)
	{
		window->lock_window("BC_NewFolderThread::start_new_folder");
		window->raise_window();
		window->unlock_window();
		change_lock->unlock();
	}
	else
	{
		change_lock->unlock();
		completion_lock->lock("BC_NewFolderThread::start_new_folder");

		Thread::start();
	}


	return 0;
}


