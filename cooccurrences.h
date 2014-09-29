/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

#ifndef cooccurrences_h
#define cooccurrences_h

#include "read_write.h" //freq_buffer, text_buffer, point_texte

#include "unicode_flex.h"


//Macros
#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))
#define mot(i) pt->mot[i+1]
#define freq(i) pt->freq[i+1]
#define o11(i,j) pt->o11[i+j*pt->nbgarde]
#define o12(i,j) pt->o12[i+j*pt->nbgarde]
#define o21(i,j) pt->o21[i+j*pt->nbgarde]
#define o22(i,j) pt->o22[i+j*pt->nbgarde]
#define O11 o11(i,j)
#define O12 o12(i,j)
#define O21 o21(i,j)
#define O22 o22(i,j)
#define distance(i,j) (*(distance+i*pt->nbgarde+j))

#define R1 (O11+O12)
#define R2 (O21+O22)
#define C1 (O11+O21)
#define C2 (O12+O22)
#define N (R1+R2)
#define E11 (R1*C1*1.0/N)
#define E12 (R1*C2*1.0/N)
#define E21 (R2*C1*1.0/N)
#define E22 (R2*C2*1.0/N)


#define chisquared_f 1000-(1.0*N*(O11*O22-O12*O21)*(O11*O22-O12*O21)/(R1*R2*C1*C2))
#define chisquared ((R1*R2*C1*C2>0)?(chisquared_f):(0))
#define mi log(1.0*max(0.0000000000001,O11)/max(0.0000000000001,E11))
#define liddell_f 1-(1.0*O11*O22-O12*O21)/(C1*C2)
#define liddell ((C1*C2>0)?(liddell_f):(0))
#define dice 1-(1.0*O11/(max(0.00000000001,sqrt(R1*C1))))
#define jaccard 1-(1.0*O11/max(0.00000000001,(O11+O12+O21)))
#define gmean 1-1.0*O11/(max(0.00000000001,sqrt(R1*C1)))
#define hyperlex 1-max(1.0*O11/max(0.00000000001,C1),1.0*O11/max(0.00000000001,R1))
#define ms 1-min(1.0*O11/max(0.00000000001,C1),1.0*O11/max(0.00000000001,R1))
#define oddsratio 1-log((max(0.00000000001,O11*O22/(max(0.00000000001,O12*O21)))))
#define zscore 1-(O11-E11)*1.0/max(0.00000000001,sqrt(E11))
#define loglikelihood 1-(O11*log(max(0.00000000001,O11)*1.0/max(0.00000000001,E11)))-(O12*log(max(0.00000000001,O12)*1.0/max(0.00000000001,E12)))-(O21*log(max(0.00000000001,O21)*1.0/max(0.00000000001,E21)))-(O22*log(max(0.00000000001,O22)*1.0/max(0.00000000001,E22)))
#define poissonstirling 1-O11*(log(max(O11,0.00000001))-log(max(E11,0.00000001))-1)
#define google (double)((max(log(max(0.00000000001,R1)),log(max(0.00000000001,C1)))-log(max(0.00000000001,O11)))/(N-min(log(max(0.00000000001,R1)),log(max(0.00000000001,C1)))))
//1000-(1.0*N*(O11*O22-O12*O21)*(O11*O22-O12*O21)/(R1*R2*C1*C2))

void Cooccurrence(point_texte* pt,int fenetre);
//Calcul les cooccurrences d'un texte donn� par pt;
double *Distance(point_texte* pt,char* typedistance);
//renvoie la matrice des distances � partir des matrices O11,O12,O21,O22
//(pt doit avoir d�j� �t� calcul� niveau des cooccurrences)

void Write_CSVMatrice(point_texte* pt,char* filename);
//Ecrit les matrices O11,O12,O21,O22 dans 4 fichiers s�par�s
void Write_CSVDistance(point_texte* pt,double *distance,char* filename);
//Ecrit la matrice des distances au format CSV
void EcrireNexus(point_texte* pt,double *distance,char* fichier);
//Ecrit le fichier Nexus correspondant aux distances calcul�es

#endif
