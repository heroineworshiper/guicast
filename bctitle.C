
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

#include "bcresources.h"
#include "bctitle.h"
#include <string.h>
#include <unistd.h>

BC_Title::BC_Title(int x, 
		int y, 
		const char *text, 
		int font, 
		int color, 
		int centered,
		int fixed_w)
 : BC_SubWindow(x, y, -1, -1, -1)
{
	this->font = font;
	if(color < 0) 
	{
    	this->color = get_resources()->default_text_color;
	}
    else
	{
    	this->color = color;
	}
    this->centered = centered;
	this->fixed_w = fixed_w;
	this->text.assign(text);
}

BC_Title::~BC_Title()
{
}


int BC_Title::initialize()
{
	if(w <= 0 || h <= 0)
	{
		get_size(this, font, text.c_str(), fixed_w, w, h);
	}

	if(centered) x -= w / 2;

	BC_SubWindow::initialize();
	draw(0);
	show_window(0);
	return 0;
}

int BC_Title::set_color(int color)
{
	this->color = color;
	draw(0);
	return 0;
}

int BC_Title::resize(int w, int h)
{
	resize_window(w, h);
	draw(0);
	return 0;
}

int BC_Title::reposition(int x, int y, int fixed_w)
{
	this->fixed_w = fixed_w;
	if(fixed_w > 0)
	{
		reposition_window(x, y, fixed_w, h);
	}
	else
	{
		reposition_window(x, y, w, h);
	}
	
	draw(0);
	return 0;
}


int BC_Title::update(const char *text, int flush)
{
	int new_w, new_h;

	this->text.assign(text);
	get_size(this, font, this->text.c_str(), fixed_w, new_w, new_h);
	resize_window(new_w, new_h);
	
	draw(flush);
	return 0;
}

void BC_Title::update(float value)
{
	char string[BCTEXTLEN];
	sprintf(string, "%.04f", value);
	update(string);
}

const char* BC_Title::get_text()
{
	return text.c_str();
}

int BC_Title::draw(int flush)
{
	int i, j, x, y;

// Fix background for block fonts.
// This should eventually be included in a BC_WindowBase::is_blocked_font()

 	if(font == MEDIUM_7SEGMENT)
 	{
		BC_WindowBase::set_color(get_bg_color());
 		draw_box(0, 0, w, h);
 	}
	else
	{
 		draw_top_background(parent_window, 0, 0, w, h);
	}

	set_font(font);
	BC_WindowBase::set_color(color);
	const char *ptr = text.c_str();
// draw 1 line at a time
	for(i = 0, j = 0, x = 0, y = get_text_ascent(font); 
		i <= text.length(); 
		i++)
	{
// end of a line
		if(ptr[i] == '\n' || ptr[i] == 0)
		{
// copy the line
            string *truncated = new string;
            truncated->assign(text.c_str() + j, i - j);
// insert ...
            if(fixed_w > 0)
            {
                string *truncated2 = get_truncated_text(font, truncated, fixed_w);
                delete truncated;
                truncated = truncated2;
            }
//printf("BC_Title::draw %d fixed_w=%d %s\n", __LINE__, fixed_w, truncated->c_str());
        
        
			if(centered)
			{
				draw_center_text(get_w() / 2, 
					y,
					truncated->c_str());
				j = i + 1;
			}
			else
			{
				draw_text(x, 
					y,
					truncated->c_str());
				j = i + 1;
			}
			y += get_text_height(font);
            
            delete truncated;
		}
	}
	set_font(MEDIUMFONT);    // reset
	flash(flush);
	return 0;
}

int BC_Title::calculate_w(BC_WindowBase *gui, const char *text, int font)
{
	int temp_w, temp_h;
	get_size(gui, font, text, 0, temp_w, temp_h);
	return temp_w;
}

int BC_Title::calculate_h(BC_WindowBase *gui, const char *text, int font)
{
	int temp_w, temp_h;
	get_size(gui, font, text, 0, temp_w, temp_h);
	return temp_h;
}



void BC_Title::get_size(BC_WindowBase *gui, 
	int font, 
	const char *text, 
	int fixed_w, 
	int &w, 
	int &h)
{
	int i, j, x, y, line_w = 0;
	w = 0;
	h = 0;

	for(i = 0, j = 0; i <= strlen(text); i++)
	{
		line_w = 0;
		if(text[i] == '\n')
		{
			h++;
			line_w = gui->get_text_width(font, &text[j], i - j);
			j = i + 1;
		}
		else
		if(text[i] == 0)
		{
			h++;
			line_w = gui->get_text_width(font, &text[j]);
		}
		if(line_w > w) w = line_w;
	}

	h *= gui->get_text_height(font);
	w += 5;
	if(fixed_w > 0) w = fixed_w;
}