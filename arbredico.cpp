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
#include <stdlib.h>
#include <string.h>

#include "arbredico.h"

#include "unicode_flex.h"


Arbre_Dico* Arbre_Dico_Init()
{
	Arbre_Dico* t=(Arbre_Dico*)malloc(sizeof(Arbre_Dico));
	int i;
	for (i=0;i<3000;i++)
	{
		t->tab[i]=NULL;
	}
	t->end=0;
	t->number=-1;
	return t;
}

Arbre_Dico* Arbre_Dico_ADD(Arbre_Dico* t,unichar *mot)
{
	if (mot==0)
		return t;
	if (t==NULL)
		t=Arbre_Dico_Init();
	if (mot[0]==0)
	{
		t->end=1;
		return t;
	} else
	{
		if (t->tab[(int)(unichar)mot[0]]==NULL)
			t->tab[(int)(unichar)mot[0]]=Arbre_Dico_Init();
		t=t->tab[(int)(unichar)mot[0]];
	}
	return Arbre_Dico_ADD(t,&mot[1]);
}

Arbre_Dico* Arbre_Dico_FOUND(Arbre_Dico* t,unichar *mot)
{
	if (mot==0)
		return 0;
	if (t==NULL)
		return 0;
	if (mot[0]==0)
	{
		if (t->end==1)
			return t;
		else
			return 0;
	} else
	{
		if (t->tab[(int)(unichar)mot[0]]==NULL)
			return 0;
	}
	return Arbre_Dico_FOUND(t->tab[(int)(unichar)mot[0]],&mot[1]);
}

void Arbre_Dico_FREE(Arbre_Dico* t)
{
	int i;
	if (t==NULL)
		return;
	for (i=0;i<2048;i++)
	{
		if (t->tab[i]!=NULL)
			Arbre_Dico_FREE(t->tab[i]);
	}
	free(t);
}
