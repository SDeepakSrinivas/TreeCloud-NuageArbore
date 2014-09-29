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
#include <math.h> /*log() */
#include "read_write.h" //freq_buffer, text_buffer, point_texte
#include "cooccurrences.h" /*log() */

#include "unicode_flex.h"


void Cooccurrence(point_texte* pt,int fenetre)
{
	int i,j,k;
	//Initialisation des matrices et de la fen�tre
	int *window=(int *)malloc((pt->nbgarde+1)*sizeof(int));
	int *freq_window=(int *)malloc((pt->nbgarde+1)*sizeof(int));
	pt->o11=(int *)malloc(((pt->nbgarde+1)*(pt->nbgarde+1))*sizeof(int));
	pt->o12=(int *)malloc(((pt->nbgarde+1)*(pt->nbgarde+1))*sizeof(int));
	pt->o21=(int *)malloc(((pt->nbgarde+1)*(pt->nbgarde+1))*sizeof(int));
	pt->o22=(int *)malloc(((pt->nbgarde+1)*(pt->nbgarde+1))*sizeof(int));
	if (window==0 || freq_window==0 || pt->o11==0 || pt->o12==0 || pt->o21==0 || pt->o22==0)
	{
		printf("Erreur d'allocation memoire\n");
		return;
	}
	for (i=0;i<=pt->nbgarde;i++)
	{
		window[i]=0;
		freq_window[i]=0;
		for (j=0;j<=pt->nbgarde;j++)
		{
			o11(i,j)=0;
			o21(i,j)=0;
			o12(i,j)=0;
			o22(i,j)=0;
		}
	}
	#define number(i) pt->number[i]-1
	for (k=0;k<(pt->count+fenetre);k++)
	{//de k 0 au nombre de mots + taille de la fen�tre
		if (k<fenetre) //initialise la fenetre
		{
			if (pt->number[k]!=0) //si mot trouv�
			{
				window[number(k)]+=1; //stock le nombre de fois que le mot est trouv� dans la fen�tre 
			}
			continue;
		}
		if (k>=fenetre && k<pt->count)
		{
			//la fen�tre se d�place de gauche � droite jusqu'� la fin du texte
			//met � jour O11
			for (i=0;i<pt->nbgarde;i++)
			{
				if (window[i]>0)
				{//si mot trouv� on incr�mente sa table des fr�quences
					freq_window[i]+=1;
				}
				for (j=i+1;j<pt->nbgarde;j++)
				{
					if ((window[i]>0) && (window[j]>0))
					{ //si les 2 mots sont pr�sents dans la fen�tre on incr�mente la matrice O11
						o11(i,j)+=1;
						o11(j,i)=o11(i,j);
					}
				}
			}
			//d�place la fen�tre
			if (pt->number[k]!=0) //si mot suivant trouv�
			{//incr�mente + 1
				window[number(k)]+=1;
			}
			if (pt->number[k-fenetre]!=0) //a la fin de la fen�tre enl�ve la derni�re case
			{
				window[number(k-fenetre)]-=1;
			}
			continue;
		}
		if (k>=pt->count)
		{
			//la fen�tre se d�place jusqu'� retrouver la position (0,taille fen�tre)
			//met � jour O11
			for (i=0;i<pt->nbgarde;i++)
			{
				if (window[i]>0)
				{//si mot trouv� on incr�mente sa table des fr�quences
					freq_window[i]+=1;
				}
				for (j=i+1;j<pt->nbgarde;j++)
				{
					if ((window[i]>0) && (window[j]>0))
					{ //si les 2 mots sont pr�sents dans la fen�tre on incr�mente la matrice O11
						o11(i,j)+=1;
						o11(j,i)=o11(i,j);
					}
				}
			}
			//d�place la fen�tre
			if (pt->number[k-pt->count]!=0) //le d�but de la fen�tre est revenu au d�but du fichier
			{//incr�mente + 1
				window[number(k-pt->count)]+=1;
			}
			if (pt->number[k-fenetre]!=0) //a la fin de la fen�tre enl�ve la derni�re case
			{
				window[number(k-fenetre)]-=1;
			}
			continue;
		}
	}
	// for (i=0;i<pt->nbgarde;i++)
	// {
		// printf("%d;",window[i]);
	// }
	// printf("\n");
	for (i=0;i<pt->nbgarde;i++)
	{
		for (j=0;j<pt->nbgarde;j++)
		{
			if (i==j)
				continue;
			o12(i,j)=freq_window[i]-o11(i,j);
			o21(j,i)=o12(i,j);
			o12(j,i)=freq_window[j]-o11(j,i);
			o21(i,j)=o12(j,i);
			o22(i,j)=(pt->count+fenetre)-o11(i,j)-o12(i,j)-o21(i,j);
			o22(j,i)=o22(i,j);
			// ;o12[i][j]=freqWin[i]-o11[i][j]
			// ;o21[j][i]=o12[i][j]
			// ;o12[j][i]=freqWin[j]-o11[j][i]
			// ;o21[i][j]=o12[j][i]
			// ;o22[i][j]=textSize+winSize-o11[i][j]-o12[i][j]-o21[i][j]
			// ;o22[j][i]=o22[i][j]
		}
	}
	free(window);
	free(freq_window);
}

double *Distance(point_texte* pt,char* typedistance)
{
	int i,j;
	double *distance=(double *)malloc((pt->nbgarde)*(pt->nbgarde)*sizeof(double));
	for (i=0;i<pt->nbgarde;i++)
		for (j=0;j<pt->nbgarde;j++)
			distance(i,j)=0.0;
	
	#define boucle for (i=0;i<pt->nbgarde;i++){for (j=i+1;j<pt->nbgarde;j++){
	#define finboucle }}
	#define tdistance(xtt,ytt) if (strcmp(typedistance,ytt)==0){ok=1;boucle;dtmp=xtt;distance(i,j)=dtmp;distance(j,i)=dtmp;finboucle;}
	double dtmp;
	int ok=0;
	tdistance(chisquared,"chisquared");
	tdistance(mi,"mi");
	tdistance(liddell,"liddell");
	tdistance(dice,"dice");
	tdistance(jaccard,"jaccard");
	tdistance(gmean,"gmean");
	tdistance(hyperlex,"hyperlex");
	tdistance(ms,"ms");
	tdistance(oddsratio,"oddsratio");
	tdistance(zscore,"zscore");
	tdistance(loglikelihood,"loglikelihood");
	tdistance(poissonstirling,"poissonstirling");
	tdistance(google,"google");
	if (ok==0)
	{
		printf("Erreur typedistance non valide : %s\n",typedistance);
		exit(1);
	}
	
	//Normalisation de la matrice
	double themax=distance(0,0);
	double themin=distance(0,0);
	for (i=0;i<pt->nbgarde;i++)
	{
		for (j=0;j<pt->nbgarde;j++) //copie l'autre diagonale
		{
			if (distance(i,j)>themax) {themax=distance(i,j);}
			if (distance(i,j)<themin) {themin=distance(i,j);}
		}
	}
	int norm=4; //Auto pour l'instant
	double coeff;
	for (i=0;i<pt->nbgarde;i++)
	{
		for (j=i+1;j<pt->nbgarde;j++) //copie l'autre diagonale
		{	
			//Normalisation : auto
			if (norm==4 && themin<0)
			{
				norm=2; //affine
			}
			else
			{
				norm=0; //linear
			}
			//Normalisation : linear
			if (norm==0)
			{
				dtmp=(distance(i,j)*1.0)/themax;
				distance(i,j)=dtmp;
				distance(j,i)=dtmp;
			}
			//Normalisation : Log
			if (norm==1)
			{
				dtmp=log(1+99*(distance(i,j)-themin)*1.0/(themax-themin));
				distance(i,j)=dtmp;
				distance(j,i)=dtmp;
			}
			//Normalisation : affine
			if (norm==2)
			{
				coeff=(2.0/(2*(log(pt->nbgarde)))+1);
				dtmp=(coeff+(1-coeff)*(distance(i,j)-themin)*1.0/(themax-themin));
				distance(i,j)=dtmp;
				distance(j,i)=dtmp;
			}
		}
	}
	return distance;
}


void Write_CSVMatrice(point_texte* pt,char* filename)
{
	char sto11[10]="11.csv";
	char sto12[10]="12.csv";
	char sto21[10]="21.csv";
	char sto22[10]="22.csv";
	char *fo11=(char *)malloc(256);
	char *fo12=(char *)malloc(256);
	char *fo21=(char *)malloc(256);
	char *fo22=(char *)malloc(256);
	strcpy(fo11,filename);
	strcat(fo11,sto11);
	strcpy(fo12,filename);
	strcat(fo12,sto12);
	strcpy(fo21,filename);
	strcat(fo21,sto21);
	strcpy(fo22,filename);
	strcat(fo22,sto22);
	FILE *f11=u_fopen(UTF8,fo11,U_WRITE);
	FILE *f12=u_fopen(UTF8,fo12,U_WRITE);
	FILE *f21=u_fopen(UTF8,fo21,U_WRITE);
	FILE *f22=u_fopen(UTF8,fo22,U_WRITE);
	int i,j;
	for (i=0;i<pt->nbgarde;i++)
	{
		u_fprintf(UTF8,f11,";%S",mot(i));
		u_fprintf(UTF8,f12,";%S",mot(i));
		u_fprintf(UTF8,f21,";%S",mot(i));
		u_fprintf(UTF8,f22,";%S",mot(i));
	}
	u_fprintf(UTF8,f11,";\n");
	u_fprintf(UTF8,f12,";\n");
	u_fprintf(UTF8,f21,";\n");
	u_fprintf(UTF8,f22,";\n");
	for (i=0;i<pt->nbgarde;i++)
	{
		u_fprintf(UTF8,f11,"%S;",mot(i));
		u_fprintf(UTF8,f12,"%S;",mot(i));
		u_fprintf(UTF8,f21,"%S;",mot(i));
		u_fprintf(UTF8,f22,"%S;",mot(i));
		for (j=0;j<pt->nbgarde;j++)
		{
			u_fprintf(UTF8,f11,"%d;",o11(i,j));
			u_fprintf(UTF8,f12,"%d;",o12(i,j));
			u_fprintf(UTF8,f21,"%d;",o21(i,j));
			u_fprintf(UTF8,f22,"%d;",o22(i,j));
		}
		u_fprintf(UTF8,f11,"\n");u_fprintf(UTF8,f12,"\n");u_fprintf(UTF8,f21,"\n");u_fprintf(UTF8,f22,"\n");
	}
	u_fclose(f11);u_fclose(f12);u_fclose(f21);u_fclose(f22);
}

void Write_CSVDistance(point_texte* pt,double *distance,char* filename)
{
	int i,j;
	FILE *f=u_fopen(UTF8,filename,U_WRITE);
	for (i=0;i<pt->nbgarde;i++)
	{
		u_fprintf(UTF8,f,";%S",mot(i));
	}
	u_fprintf(UTF8,f,";\n");
	for (i=0;i<pt->nbgarde;i++)
	{
		u_fprintf(UTF8,f,"%S;",mot(i));
		for (j=0;j<pt->nbgarde;j++)
		{
				u_fprintf(UTF8,f,"%lf;",distance(i,j));
		}
		u_fprintf(UTF8,f,"\n");
	}
	u_fclose(f);
}

// void EcrireNexus_Java(point_texte* pt,double *distance,char* fichier)
// {
	// int i,j;
	// FILE *f=fopen(fichier,"wb");
	// fprintf(f,"double[][] distances = double[%d][%d];\n",pt->nbgarde,pt->nbgarde);
	// for (i=0;i<pt->nbgarde;i++)
	// {	
		// for (j=0;j<pt->nbgarde;j++)
		// {
				// fprintf(f,"distances[%d][%d]=%lfD;\n",i+1,j+1,distance(i,j));
		// }
	// }
	// fclose(f);
		
// }

void EcrireNexus(point_texte* pt,double *distance,char* fichier)
{
	int i,j;
	FILE *f=u_fopen(UTF8,fichier,U_WRITE);
	u_fprintf(UTF8,f,"#nexus\n\n""BEGIN Taxa;\n""DIMENSIONS ntax=%d;\nTAXLABELS\n",pt->nbgarde);
	for (i=0;i<pt->nbgarde;i++)
	{
		u_fprintf(UTF8,f,"[%d] '%S'\n",i+1,mot(i));
	}
	u_fprintf(UTF8,f,";\nEND; [Taxa]\nBEGIN Distances;\nDIMENSIONS ntax=%d;\nFORMAT labels=left diagonal triangle=both;\nMATRIX\n",pt->nbgarde);
	for (i=0;i<pt->nbgarde;i++)
	{	
		u_fprintf(UTF8,f,"[%d] '%S' ",i+1,mot(i));
		for (j=0;j<pt->nbgarde;j++)
		{
			u_fprintf(UTF8,f," %lf",distance(i,j));
		}
		u_fprintf(UTF8,f,"\n");
	}
	u_fprintf(UTF8,f,";\nEND; [Distances]\n\nBEGIN st_Assumptions;\n	disttransform=NJ;\n	treestransform=TreeSelector;\n	splitstransform=EqualAngle UseWeights = false RunConvexHull = true DaylightIterations = 0 OptimizeBoxesIterations = 0 SpringEmbedderIterations = 0;\n	SplitsPostProcess filter=dimension value=4;\n	autolayoutnodelabels;\nEND; [st_Assumptions]\n");
	u_fclose(f);
		
}
