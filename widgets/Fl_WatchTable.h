// Created on August 29, 2019, 2:28 PM

#ifndef FL_WATCHTABLE_H
#define FL_WATCHTABLE_H

#include <Fl/Fl_Table_Row.H>
#include "ProjectClass.h"

class Fl_WatchTable : public Fl_Table_Row {
private:
	
	int _selected;
	
	class Row {
	public:
		std::vector<std::string> cols;
		ProjectClass::WATCHPOINT *watch;
	};

	std::vector<Row> _row_items;
	int _show_values;
	
protected:
	
	void draw_cell(TableContext context, int R=0, int C=0,      // table cell drawing
		int X=0, int Y=0, int W=0, int H=0);
	
public:

	Fl_WatchTable(int x, int y, int w, int h, const char *l = 0);
	virtual ~Fl_WatchTable();
	
	void add_item(ProjectClass::WATCHPOINT *item);
	ProjectClass::WATCHPOINT *get_item(int i) {
		return _row_items[i].watch;
	}
	void delete_item(int row);
	
	void update_values();
	void update_types();
	void autowidth(int pad);
	void values_active(int active) {
		_show_values = active;
	}
	
	int handle(int event);

	void clear();
	
};

#endif /* FL_WATCHTABLE_H */

