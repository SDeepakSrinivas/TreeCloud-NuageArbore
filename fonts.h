/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

#ifndef fonts_h
#define fonts_h

#include "unicode_flex.h"


int str_width(unichar *st,int font,int fontsize);
//Prend une chaine st, de taille fontsize et renvoie le nombre de pixel que sa prendra � l'affichage
//font=0 : Times New Romant
//font=1 : Arial
//font=2 : Courrier

int str_height(int fontsize);
//Renvoie la hauteur d'un lettres pour une taille donn�e (ne d�pend pas de la police)

#endif

