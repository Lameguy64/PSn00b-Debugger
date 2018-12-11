/* 
 * File:   Fl_Symbol_Table.h
 * Author: Lameguy64
 *
 * Created on September 23, 2018, 7:24 PM
 */

#ifndef FL_SYMBOL_TABLE_H
#define FL_SYMBOL_TABLE_H

#include <Fl/Fl_Table_Row.H>
#include <vector>
#include <string.h>

#include "SymbolsClass.h"

class Fl_Symbol_Table : public Fl_Table_Row {
private:
	
	class Row {
	public:
		std::vector<std::string> cols;
		SymbolsClass::SYM_NAME *sym;
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

	/*static void event_callback(Fl_Widget*, void*);
    void event_callback2();*/
	
protected:
	
	void draw_cell(TableContext context, int R=0, int C=0,      // table cell drawing
                   int X=0, int Y=0, int W=0, int H=0);
    void sort_column(int col, int reverse=0);                   // sort table by a column
    void draw_sort_arrow(int X,int Y,int W,int H);
	
public:
	
	Fl_Symbol_Table(int x, int y, int w, int h, const char *l=0);
	virtual ~Fl_Symbol_Table();

	void additem(SymbolsClass::SYM_NAME *sym_item);
	void clear();
	
	void autowidth(int pad);
	const SymbolsClass::SYM_NAME *item(int row);
	
	int handle(int event);
	
};

#endif /* FL_SYMBOL_TABLE_H */

