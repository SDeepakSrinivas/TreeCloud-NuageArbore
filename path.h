/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

#ifdef __CYGWIN__ //if windows we need to correct path name
#include <sys/cygwin.h>

	#ifndef path_h
	#define path_h
	int convert_path(char *);
	//Utilisé pour corriger les chemins windows (transforme C:/test en /cygdrive/c/test)
	#endif
	
#endif

