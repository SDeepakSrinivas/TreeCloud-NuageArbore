/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/
#include <stdio.h>
#include "SVG.h"

#include "unicode_flex.h"


int SVG_Create_Dim(unichar* img,unichar* buff,int width,int height)
{
	return u_sprintf(img,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"\
"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"\
"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n"\
"\t<title>%s</title>\n"\
"\t<defs>\n"\
"\t<script type=\"text/javascript\"><![CDATA[\n"\
"\tvar svgdoc;\n"\
"\tvar appui=false,xc=0,yc=0;\n"\
"\tvar dx=0,dy=0;\n"\
"\tfunction mov(evt)\n"\
"\t{\n"\
"\t\tif (!appui)\n"\
"\t\t\t\t\treturn\n"\
"\t\tvar xc1 = evt.clientX;\n"\
"\t\tvar yc1 = evt.clientY;\n"\
"\t\tevt.target.setAttribute(\"dx\",xc1-xc);\n"\
"\t\tevt.target.setAttribute(\"dy\",yc1-yc);\n"\
"\t}\n"\
"\tfunction clic(evt)\n"\
"\t{\n"\
"\t\tappui=true;\n"\
"\t\txc = evt.clientX;\n"\
"\t\tyc = evt.clientY;\n"\
"\t\tdx = evt.target.getAttribute(\"dx\");\n"\
"\t\tdy = evt.target.getAttribute(\"dy\");\n"\
"\t}\n"\
"\tfunction txt(evt,txt) {\n"\
"\t\tsvgdoc  = evt.target.ownerDocument;\n"\
"\t\tvar node = evt.target;\n"\
"\t\tchgtxt(txt);\n"\
"\t\tnode.addEventListener(\"mousemove\", afftip, false);\n"\
"\t\tnode.addEventListener(\"mouseout\", hide, false);\n"\
"\t}\n"\
"\tfunction afftip(evt) {\n"\
"\t\tvar rectangle, texte;\n"\
"\t\trectangle = svgdoc.getElementById(\"tip\");\n"\
"\t\ttexte = svgdoc.getElementById(\"tiptxt\");\n"\
"\t\ttexte.setAttribute(\"style\", \"fill: rgb(255,131,9); font-size: 20px; visibility: visible\");\n"\
"\t\ttexte.setAttribute(\"x\", evt.clientX+10);\n"\
"\t\trectangle.setAttribute(\"x\", evt.clientX+6);\n"\
"\t\ttexte.setAttribute(\"y\", evt.clientY-5);\n"\
"\t\trectangle.setAttribute(\"y\", evt.clientY-20);\n"\
"\t\trectangle.setAttribute(\"style\", \"fill: #FFEBD5; stroke: #FF8309; stroke-width: 0.5px; visibility: visible\");\n"\
"\t\trectangle.setAttribute(\"width\",texte.getComputedTextLength()+6);\n"\
"\t}\n"\
"\tfunction chgtxt(txt){\n"\
"\t\tvar rectangle, texte;\n"\
"\t\ttexte = svgdoc.getElementById(\"tiptxt\");\n"\
"\t\tchild=texte.firstChild;\n"\
"\t\tchild.data=txt;\n"\
"\t}\n"\
"\tfunction hide() {\n"\
"\t\tvar rectangle, texte;\n"\
"\t\trectangle = svgdoc.getElementById(\"tip\");\n"\
"\t\ttexte = svgdoc.getElementById(\"tiptxt\");\n"\
"\t\trectangle.setAttribute(\"style\",\"visibility: hidden\");\n"\
"\t\ttexte.setAttribute(\"style\",\"visibility: hidden\");\n"\
"\t\tappui=false;\n"\
"\t}\n"\
"\t]]></script>\n"\
"\t</defs>\n"\
"\t<g font-family=\"Arial\">\n"\
,width+100,height,buff);
}

int SVG_End(unichar* img)
{
	return u_sprintf(img,\
	"\t</g>\n"\
	"\t<rect id=\"tip\" x=\"50\" y=\"50\" rx=\"5\" ry=\"5\" width=\"100\" height=\"21\" style=\"visibility: hidden\"/>\n"\
	"\t<text id=\"tiptxt\" x=\"50\" y=\"50\" style=\"visibility: hidden\">...</text>\n"\
	"</svg>\n"\
	);
}

void conversion_underscores(unichar *dest,unichar *src) {
int i = 0, j=0;

	while (src[i] != '\0') {
		if (src[i] != '_') dest[j++] = src[i++];
		else {
			if (src[i+1] == '_' && src[i+2] == '_') {dest[j++] = '\''; i += 3;}
			else if (src[i+1] == '_' ) {dest[j++] = '-'; i += 2;}
			else {dest[j++] = '_'; i++;}
		}
	}
	dest[j] = '\0';
}


int SVG_Text2(unichar* img,int x,int y,int r,int g,int b,unichar* buff,int importance)
{
	return u_sprintf(img,"\t<text x=\"%d\" y=\"%d\" fill=\"rgb(%d,%d,%d)\" font-size=\"%d\">%S</text>\n",x,y,r,g,b,importance,buff);
}

int SVG_Text(unichar* img,int x,int y,int r,int g,int b,unichar* buff,int freq,int importance)
{
	unichar buff2[256];
	conversion_underscores(buff2,buff);
	return u_sprintf(img,"\t<text x=\"%d\" y=\"%d\" fill=\"rgb(%d,%d,%d)\" onmouseover=\"txt(evt,'%S (%d)')\" onmousemove=\"mov(evt)\" onmouseup=\"appui=false\" onmousedown=\"clic(evt)\" font-size=\"%d\">%S</text>\n",x,y,r,g,b,buff2,freq,importance,buff2);
}

int SVG_Line(unichar* img,int x1,int y1,int x2,int y2)
{
	return u_sprintf(img,"\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#BBBBBB\" stroke-width=\"1\"  />\n",x1,y1,x2,y2);
}

int SVG_LineB(unichar* img,int x1,int y1,int x2,int y2,unichar *stroke,int width)
{
	return u_sprintf(img,"\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%S\" stroke-width=\"%d\"  />\n",x1,y1,x2,y2,stroke,width);
}

int SVG_Line_Point(unichar* img,int x1,int y1,int x2,int y2)
{
	return u_sprintf(img,"\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"red\"  style=\"stroke-dasharray: 1, 1\" stroke-width=\"0.3\"  />\n",x1,y1,x2,y2);
}

int SVG_Rect(unichar* img,int x1,int y1,int x2,int y2)
{
	//return sprintf(img,"\t<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\" stroke-width=\"1\"  />\n",x1,y1,x2,y2);
	return u_sprintf(img,"\t<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.2\"/>\n",x1,y1,x2,y2);
}



