
/*
 * CINELERRA
 * Copyright (C) 2008 Adam Williams <broadcast at earthling dot net>
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

#include "bcdelete.h"
#include "bcsignals.h"
#include "filesystem.h"
#include "language.h"





BC_DeleteFile::BC_DeleteFile(BC_FileBox *filebox, int x, int y)
 : BC_Window(filebox->get_delete_title(), 
 	x, 
	y, 
	DP(320), 
	DP(480), 
	0, 
	0, 
	0, 
	0, 
	1)
{
	this->filebox = filebox;
	data = 0;
}

BC_DeleteFile::~BC_DeleteFile()
{
	delete data;
}

void BC_DeleteFile::create_objects()
{
    int margin = BC_Resources::theme->widget_border;
	int x = margin, y = margin;
	data = new ArrayList<BC_ListBoxItem*>;
	int i = 0;
	char *path;
	FileSystem fs;


	lock_window("BC_DeleteFile::create_objects");
	while((path = filebox->get_path(i)))
	{
//printf("BC_DeleteFile::create_objects %d i=%d %s\n", __LINE__, i, path);
		data->append(new BC_ListBoxItem(path));
		i++;
	}
		
	BC_Title *title;
	add_subwindow(title = new BC_Title(x, y, _("Really delete the following files?")));
	y += title->get_h() + DP(5);
	BC_DeleteList *list;
	add_subwindow(list = new BC_DeleteList(filebox, 
		0, 
		y, 
		get_w(), 
		get_h() - y - BC_OKButton::calculate_h() - DP(20),
		data));
	y += list->get_h() + DP(5);
	add_subwindow(new BC_OKButton(this));
	add_subwindow(new BC_CancelButton(this));
	show_window();
	unlock_window();
}





BC_DeleteList::BC_DeleteList(BC_FileBox *filebox, 
	int x, 
	int y, 
	int w, 
	int h,
	ArrayList<BC_ListBoxItem*> *data)
 : BC_ListBox(x, 
 	y, 
	w, 
	h, 
	LISTBOX_TEXT,
	data)
{
	this->filebox = filebox;
}










BC_DeleteThread::BC_DeleteThread(BC_FileBox *filebox)
 : BC_DialogThread()
{
	this->filebox = filebox;
}

void BC_DeleteThread::handle_done_event(int result)
{
	if(!result)
	{
		filebox->lock_window("BC_DeleteThread::handle_done_event");
		filebox->delete_files();
		filebox->unlock_window();
	}
}

BC_Window* BC_DeleteThread::new_gui()
{
	int x = filebox->get_abs_cursor_x(1);
	int y = filebox->get_abs_cursor_y(1);
	BC_DeleteFile *result = new BC_DeleteFile(filebox, x, y);
	result->create_objects();
	return result;
}








