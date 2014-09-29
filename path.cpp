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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unicode_flex.h"


int convert_path(char *str)
//Used in Cygwin (Windows) to convert Windows path to Posix path
{
	ssize_t size;
	char *posix=0;
	size = cygwin_conv_path (CCP_WIN_A_TO_POSIX | CCP_ABSOLUTE, str, NULL, 0);
	if (size < 0)
	  perror ("cygwin_conv_path");
	else
	  {
		posix = (char *) malloc (size);
		if (cygwin_conv_path (CCP_WIN_A_TO_POSIX | CCP_ABSOLUTE, str,posix, size))
		  perror ("cygwin_conv_path");
	  }
	strcpy(str,posix);
	return size;
}
#endif
