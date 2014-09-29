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


#ifndef read_write_h
#define read_write_h

#define max_mot_freq 500000
#define max_buffer 1024*1024

typedef struct freq_buffer { //500000*8 = ~4Mo
	unichar* buffer[max_mot_freq];
	int freq[max_mot_freq];
	int count;
} freq_buffer;

typedef struct text_buffer {
	unichar *buffer;
	int size;
} text_buffer;
	
typedef struct point_texte { //500000*3= ~1.5Mo
	int number[max_mot_freq];
	int count;
	int nbgarde;
	unichar* mot[max_mot_freq];
	int freq[max_mot_freq];
	int *o11;
	int *o12;
	int *o21;
	int *o22;
} point_texte;

freq_buffer* Freq_Calc(text_buffer* txt);
int Freq_Number(freq_buffer* freq,int nb,int max);

void Format_Text(text_buffer* txt,char *minuscules);
unichar Formated_Char(unichar c,char *minuscules);

text_buffer* Open_File(char *filename);
void Write_File(char *filename,text_buffer* txt);
void Write_Text(text_buffer* txt,char* filename);

point_texte* Text_Pointer(text_buffer *txt,freq_buffer *freq, int fq);


void Write_Freq_File(char* filename,freq_buffer *freq,int fq);


void AntiDico(text_buffer *txt,char *filename,char *minuscules);
void DelFreqWords(text_buffer *txt,freq_buffer *freq, int fq);


#endif
