/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

#include "unicode_flex.h"


#ifndef SVG_h
#define SVG_h

int SVG_Create_Dim(unichar* img,unichar* buff,int width,int height);
int SVG_End(unichar* img);
int SVG_Text(unichar* img,int x,int y,int r,int g,int b,unichar* buff,int freq,int importance);
int SVG_Text2(unichar* img,int x,int y,int r,int g,int b,unichar* buff,int importance);
int SVG_Line(unichar* img,int x1,int y1,int x2,int y2);
int SVG_LineB(unichar* img,int x1,int y1,int x2,int y2,unichar *stroke,int width);
int SVG_Line_Point(unichar* img,int x1,int y1,int x2,int y2);
int SVG_Rect(unichar* img,int x1,int y1,int x2,int y2);
	
#endif
