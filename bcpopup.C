
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

#include "bcpopup.h"


BC_FullScreen::BC_FullScreen(int w, 
    int h, 
    int bg_color,
    int vm_scale,
    int hide,
    BC_Pixmap *bg_pixmap)
 : BC_WindowBase()
{
	this->w = w; 
	this->h = h;
	this->bg_color = bg_color;
    this->vm_scale = vm_scale;
	this->hidden = hide;
	this->bg_pixmap = bg_pixmap;
    this->window_type = POPUP_WINDOW;
}


BC_FullScreen::~BC_FullScreen()
{
}


int BC_FullScreen::initialize()
{
#ifdef HAVE_LIBXXF86VM
    if (vm_scale) 
    {
        this->window_type = VIDMODE_SCALED_WINDOW;
	    create_window(parent_window,
            "Fullscreen", 
            parent_window->get_root_x(0),
            parent_window->get_root_y(0),
            w, 
            h, 
            w, 
            h, 
            0,
            top_level->private_color,
            hidden,
            bg_color,
            NULL,
            bg_pixmap,
            0);
    }
    else
#endif
    {
        create_window(parent_window,
            "Fullscreen", 
            parent_window->get_root_x(0),
            parent_window->get_root_y(0),
            w, 
            h, 
            w, 
            h, 
            0,
            top_level->private_color, 
            hidden,
            bg_color,
            NULL,
            bg_pixmap,
            0);
    }
    return 0;
}








BC_Popup::BC_Popup(int x,
	int y,
	int w, 
	int h, 
	int bg_color,
	int hide,
	BC_Pixmap *bg_pixmap)
 : BC_WindowBase()
{
	this->x = x; 
	this->y = y; 
	this->w = w; 
	this->h = h;
	this->bg_color = bg_color;
	this->hidden = hide;
	this->bg_pixmap = bg_pixmap;
    this->window_type = POPUP_WINDOW;
}


BC_Popup::~BC_Popup()
{
}

int BC_Popup::initialize()
{
	create_window(parent_window,
	    "Popup", 
	    x,
	    y,
	    w, 
	    h, 
	    w, 
	    h, 
	    0,
	    top_level->private_color, 
	    hidden,
	    bg_color,
	    NULL,
	    bg_pixmap,
	    0);
    return 0;
}





