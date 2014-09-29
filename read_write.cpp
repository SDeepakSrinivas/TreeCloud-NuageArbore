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

#include "read_write.h"
#include "arbredico.h"


#include "unicode_flex.h"




freq_buffer* Freq_Calc(text_buffer* txt)
{	
	if (txt->size==0 || txt->buffer==0)
		return 0;
	freq_buffer *freq;
	freq =(freq_buffer *) malloc(sizeof(freq_buffer));
	freq->count=0;
	Arbre_Dico *tree=Arbre_Dico_Init();
	Arbre_Dico *t;
	int i,j;
	j=0;
	for (i=0;i<txt->size;i++)
	{
		if (i>j)
		{
			if (txt->buffer[i]==0)
			{
				t=Arbre_Dico_ADD(tree,&txt->buffer[j]);
				if (t->end==1)
				{
					if (t->number!=-1)
					{
						freq->freq[t->number]+=1;
					} else 
					{
						freq->buffer[freq->count]=&txt->buffer[j];
						freq->freq[freq->count]=1;
						t->number=freq->count;
						freq->count+=1;
					}
				}
				j=i+1;
			}
		} else {
			if (txt->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	Arbre_Dico_FREE(tree);
	return freq;
}

int Freq_Number(freq_buffer* freq,int nb,int max)
{
	int fq=0;
	int count=0;
	int i;
	while (1)
	{
		count=0;
		for (i=0;i<freq->count;i++)
		{
			if (freq->freq[i]>=fq)
				count+=1;
		}
		if (count==nb)
			return fq;
		if (count<nb)
		{
			if (max==0)
			{
				return fq-1;
			} else {
				return fq;
			}
		}
		fq+=1;
	}
	return fq;
}


text_buffer* Open_File(char *filename){
	int k=1;
	text_buffer *txt=(text_buffer *)malloc(sizeof(text_buffer));
	FILE *pFile;
	int *lSize=(int *)malloc(sizeof(int));
	pFile=u_fopen(UTF8,filename,U_READ);
	//u_fprintf(UTF8,stdout,"******%s******\n",filename);
	if (pFile==0)
		return 0;
	int c, file_length;
	file_length =0;
	while ((c=u_fgetc_UTF8_raw(pFile)) != EOF) {file_length++;}
	//u_fprintf(UTF8,stdout,"file_length=%d \n:",file_length);
	//fseek(pFile,0,SEEK_END);
	//*lSize=ftell(pFile);
	//*lSize = 2 * *lSize;
	//u_fprintf(UTF8,stdout,"%d k=%d\n:",*lSize,k);
	rewind(pFile);
	*lSize = file_length;
	txt->buffer=(unichar*)malloc(sizeof(unichar)*((*lSize)+2));
	if (txt->buffer==0)
		return 0;
	int ret = u_fread_raw(UTF8,&txt->buffer[1],*lSize,pFile);
	//int i;
	//for (i=0; i < 100; i++) {
	//	u_fputc(UTF8,txt->buffer[i],stdout);
	//}
	u_fclose(pFile);
	txt->buffer[0]=0; //must begin by 0
	txt->size=(*lSize)+2;
	txt->buffer[txt->size-1]=0; //buffer must end by 0 so buffer = text + 0
	free(lSize);
	return txt;
}

void Write_File(char *filename,text_buffer* txt){
	FILE *pFile;
	pFile=u_fopen(UTF8,filename,U_WRITE);
	if (pFile==0)
		return;
	u_fwrite(UTF8,txt->buffer,txt->size,pFile);
	u_fclose(pFile);
}

unichar Formated_Char(unichar c,char *minuscules)
{
//	if (c == 0x2d || c == 0x5f || u_is_letter(c))
		if ( u_is_letter(c) || c == '_') {
			if ( strcmp(minuscules,"OUI") == 0) {
				c = u_tolower(c);
			}
		return c;
		}
	return ((unichar) 0);
}

void Format_Text(text_buffer* txt,char *minuscules)
{
//Maj > min, remove accent, remove non auth. char.
	int i;
	for (i=0;i<txt->size;i++)
	{
		//u_fprintf(UTF8,stdout,"i=%d c=%d:",i,txt->buffer[i]);
		txt->buffer[i]=Formated_Char(txt->buffer[i],minuscules);
		//u_fprintf(UTF8,stdout,"--> i=%d c=%d\n:",i,txt->buffer[i]);
	}
}

void Write_Freq_File(char* filename,freq_buffer *freq,int fq)
{
	unichar *buff=(unichar *)malloc(max_buffer*sizeof(unichar));
	int i,j=0;
	for (i=0;i<freq->count;i++)
	{
		if (freq->freq[i]>=fq)
			j+=u_sprintf(&buff[j],"%S;%d;\n",freq->buffer[i],freq->freq[i]);
	}
	text_buffer t;
	t.size=j;
	t.buffer=buff;
	Write_File(filename,&t);
	//free(t.buffer);//ajout claude
}

void AntiDico(text_buffer *txt,char *filename,char *minuscules)
{
	if (txt->size==0 || txt->buffer==0)
		return;
	text_buffer *t2=Open_File(filename);
	if (t2==0 || t2->buffer==0 || t2->size==0)
	{
		u_fprintf(UTF8,stdout,"%s non trouve ou vide\n",filename);
		return;
	}
	Format_Text(t2,minuscules);
	Arbre_Dico *tree_dico=Arbre_Dico_Init();
	Arbre_Dico * t;
	int i,j;
	j=0;
	for (i=0;i<t2->size;i++)
	{
		if (i>j)
		{
			if (t2->buffer[i]==0)
			{
				Arbre_Dico_ADD(tree_dico,&t2->buffer[j]);
				j=i+1;
			}
		} else {
			if (t2->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	j=0;
	int k;
	for (i=0;i<txt->size;i++)
	{
		if (i>j)
		{
			if (txt->buffer[i]==0)
			{
				t=Arbre_Dico_FOUND(tree_dico,&txt->buffer[j]);
				if (t!=0)
				{
					for (k=j;k<i;k++)
					{
						txt->buffer[k]=0;
					}
				}
				j=i+1;
			}
		} else {
			if (txt->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	Arbre_Dico_FREE(tree_dico);
	free(t2->buffer);
	free(t2);
}

void DelFreqWords(text_buffer *txt,freq_buffer *freq, int fq)
{
	int i,j;
	Arbre_Dico *tree=Arbre_Dico_Init();
	Arbre_Dico * t;
	for (i=0;i<freq->count;i++)
	{
		if (!(fq<=freq->freq[i]))
		{
			Arbre_Dico_ADD(tree,freq->buffer[i]);
		}
	}
	j=0;
	int k;
	for (i=0;i<txt->size;i++)
	{
		if (i>j)
		{
			if (txt->buffer[i]==0)
			{
				t=Arbre_Dico_FOUND(tree,&txt->buffer[j]);
				if (t!=0)
				{
					k=j;
					while (txt->buffer[k]!=0)
					{
						txt->buffer[k]=0;
						k++;
					}
				}
				j=i+1;
			}
		} else {
			if (txt->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	Arbre_Dico_FREE(tree);
}

point_texte* Text_Pointer(text_buffer *txt,freq_buffer *freq, int fq)
{
	point_texte *pt=(point_texte *)malloc(sizeof(point_texte));
	pt->count=0;
	pt->nbgarde=0;
	int i,j;
	Arbre_Dico *tree=Arbre_Dico_Init();
	Arbre_Dico * t;
	//u_fprintf(UTF8,stdout,"mots gardes: ");
	for (i=0;i<freq->count;i++)
	{
		if (fq<=freq->freq[i])
		{
			Arbre_Dico_ADD(tree,freq->buffer[i]);
			pt->mot[pt->nbgarde+1]=freq->buffer[i];
			pt->freq[pt->nbgarde+1]=freq->freq[i];

			//u_fprintf(UTF8,stdout,"%S:",pt->mot[pt->nbgarde+1]);
			//u_fprintf(UTF8,stdout,"%d\n",pt->freq[pt->nbgarde+1]);
			pt->nbgarde+=1;
			// pt->mot[pt->nbgarde]=freq->buffer[i];
			// pt->freq[pt->nbgarde]=freq->freq[i];
			//printf("%s;",mot(i));

		}
	}
	//u_fprintf(UTF8,stdout,"%S\n",mot(i));
	//pt->nbgarde+=1;
	j=0;
	int k;
	for (i=0;i<txt->size;i++)
	{
		if (i>j)
		{
			if (txt->buffer[i]==0)
			{
				t=Arbre_Dico_FOUND(tree,&txt->buffer[j]);
				if (t!=0)
				{
					for (k=0;k<pt->nbgarde;k++)
					{
						if (u_strcmp(&txt->buffer[j],pt->mot[k+1])==0)
						{
							pt->number[pt->count]=k+1;
							break;
						}
					}
				} else {
					pt->number[pt->count]=0;
				}
				pt->count++;
				j=i+1;
			}
		} else {
			if (txt->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	Arbre_Dico_FREE(tree);
	return pt;
}

void Write_Text(text_buffer* txt,char* filename)
{
	FILE *f=u_fopen(UTF8,filename,U_WRITE);
	int i,j;
	j=0;
	for (i=0;i<txt->size;i++)
	{
		if (i>j)
		{
			if (txt->buffer[i]==0)
			{
				u_fprintf(UTF8,f,"%S\n",&txt->buffer[j]);
				j=i+1;
			}
		} else {
			if (txt->buffer[i]==0)
				j=i+1;
			else
				j=i;
		}
	}
	u_fclose(f);
}



