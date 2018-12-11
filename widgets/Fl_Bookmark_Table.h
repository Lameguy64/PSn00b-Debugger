/* 
 * File:   Fl_Bookmark_Table.h
 * Author: Lameguy64
 *
 * Created on October 25, 2018, 2:18 PM
 */

#ifndef FL_BOOKMARK_TABLE_H
#define FL_BOOKMARK_TABLE_H

#include <Fl/Fl_Table_Row.H>
#include <vector>
#include <string>

#include "ProjectClass.h"

class Fl_Bookmark_Table : public Fl_Table_Row {
private:
	
	class Row {
	public:
		std::vector<std::string> cols;
		unsigned int addr;
	};

	std::vector<Row> _row_items;                                  // data in each row
    int _sort_reverse;
    int _sort_lastcol;
	
	class SortColumn {
		int _col, _reverse;
	public:
		SortColumn(int col, int reverse) {
			_col = col;
			_reverse = reverse;
		}
		bool operator()(const Row &a, const Row &b) {
			const char *ap = ( _col < (int)a.cols.size() ) ? a.cols[_col].c_str() : "",
					   *bp = ( _col < (int)b.cols.size() ) ? b.cols[_col].c_str() : "";
			
			// Alphabetic sort
			return( _reverse ? strcmp(ap, bp) > 0 : strcmp(ap, bp) < 0 );
			
		}
	};
	
protected:
	
	void draw_cell(TableContext context, int R=0, int C=0,      // table cell drawing
                   int X=0, int Y=0, int W=0, int H=0);
    void sort_column(int col, int reverse=0);                   // sort table by a column
    void draw_sort_arrow(int X,int Y,int W,int H);
	
public:
	
	Fl_Bookmark_Table(int x, int y, int w, int h, const char *l=0);
	virtual ~Fl_Bookmark_Table();

	void additem(ProjectClass::BOOKMARK *mark_item);
	unsigned int item(int row);
	void clear();
	
	void autowidth(int pad);
	
	int handle(int event);
	
};

#endif /* FL_BOOKMARK_TABLE_H */

