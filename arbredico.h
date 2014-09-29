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


#ifndef arbredico_h
#define arbredico_h

	typedef struct Arbre_Dico {
		struct Arbre_Dico* tab[3000];
		int end;
		long int number;
	} Arbre_Dico;

	Arbre_Dico* Arbre_Dico_Init();
	Arbre_Dico* Arbre_Dico_ADD(Arbre_Dico* t,unichar *mot);
	Arbre_Dico* Arbre_Dico_FOUND(Arbre_Dico* t,unichar *mot);
	void Arbre_Dico_FREE(Arbre_Dico* t);
	
#endif
