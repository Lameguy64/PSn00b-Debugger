/* 
 * File:   sourcedebug.h
 * Author: Lameguy64
 *
 * Created on September 19, 2018, 11:59 AM
 */

#ifndef SOURCEDEBUG_H
#define SOURCEDEBUG_H

void ShowSourceWindow();
void ShowSymbolsWindow();

int ViewSource(SymbolsClass::SourceClass* source, int line = 0);
void UpdateSourceView();

#endif /* SOURCEDEBUG_H */

