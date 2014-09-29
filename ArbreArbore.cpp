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

#include "path.h" //convert_path
#include "fonts.h"  //str_width, str_height
#include "cooccurrences.h" //macro distance, cooccurrences, distances
#include "read_write.h" //freq_buffer, text_buffer, point_texte
#include "arbredico.h" //Arbre_Dico_Init, Arbre_Dico_ADD, Arbre_Dico_FOUND
#include "SVG.h"

#include "Phylo.h" //GenerationPhyloTreeBioNJ, GenererDessin

#include "Unitex_Traitement.h"



int main(int argc, char *argv []){
	int i;
	//D�finition des param�tres
	const char *strhelp=(const char *)""\
	"  *************************************************\n"\
	"  *       Generation d'Arbre Arbore v2.2          *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *************************************************\n\n"\
	"Verbose mode : -v \n"\
	"IN:\n"\
	"Nom du fichier d'entree : -file %%s\n"\
	"Nom du fichier AntiDico : -fileantidico %%s\n"\
	"Taille de la fenetre : -taillefenetre %%d\n"\
	"Nombre maximum de mots a garder : -nombremaxmots %%d\n"\
	" ou nombre minimum de mots a garder : -nombreminmots %%d\n"\
	"Pour le Graphique (Format SVG) :\n"\
	" hauteur : -image_width %%d\n"\
	" largeur : -image_height %%d\n"\
	"Type de calcul des distances : -typedistance %%s\n"\
	"%%s in (chisquared,mi,liddell,dice,jaccard,gmean,hyperlex,ms,oddsratio,zscore)\n"\
	"       (loglikelihood,poissonstirling,google)\n"\
	"OUT:\n"\
	"Nom du fichier du texte traite, un mot par ligne : -fileout %%s\n"\
	"Nom du fichier des Frequences (CSV : mot;frequence\\n) : -filefrequence %%s\n"\
	"Nom des fichiers de matrices des distances (o11..o22): -fileCSV %%s\n"\
	"  %%s est ici juste le préfixe des matrices par exemple -fileCSV O produira :\n"\
	"  o11.csv,o12.csv,o21.csv,o22.csv\n"\
	"Nom du fichier des distances (format CSV) : -filedistance %%s\n"\
	"Nom du fichier .nexus : -filenexus %%s\n"\
	"Nom du fichier .html : -filehtml %%s\n"\
	"Nom du fichier du Graph (Format Newick) : -filenewick %%s\n"\
	"Nom du fichier du Graph (Image SVG) : -filegraph %%s\n";

	//Global Vars
	char fichierNewick[256]="";
	char fichierNexus[256]="";
	char fileimage[256]="";
	char fichierCSV[256]="";
	char fichierDistance[256]="";
	char fichierentree[256]="";
	char fichiersortie[256]="";
	char fichierhtml[256]="";
	char fichierantidico[256]="";
	char fichierfrequence[256]="";
	char typedistance[256]="jaccard"; //default
	

	char minuscules[256]="";
	char workdir[256]="";
	char tmp_annoted_fle[256]="";
	char langue[256]="";

	char virtuel[256]="";
	char anti_perso[256]="";
	char debug[256]="";
	char legend[256]="";

	char anti_dico_perso[256]="";

	int taillefenetre=50;
	int nombremots=20;
	int image_width=800;
	int image_height=640;
	int verbose=0;
	int max=1;
	
	for (i = 0; i < argc; i++)
	{
	
		if ((argv[i][0]=='-' && argv[i][1]=='h') )
			{	//affichage de l'aide
				printf("%s\n",strhelp);
				exit(0);
			}
		if (argv[i][0]=='-')
		{
			if ((strcmp(argv[i],"-legend") == 0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",legend);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-virtuel")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",virtuel);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-anti_perso")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",anti_perso);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-debug") == 0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",debug);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-langue")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",langue);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-taillefenetre")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%d",&taillefenetre);
				//printf("-taillefenetre %s\n", argv[i+1]);
			}
			if ((strcmp(argv[i],"-nombremaxmots")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%d",&nombremots);
				//printf("-nombremaxmots %s\n", argv[i+1]);
				max=1;
			}
			if ((strcmp(argv[i],"-nombreminmots")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%d",&nombremots);
				//printf("-nombreminmots %s\n", argv[i+1]);
				max=0;
			}
			if ((strcmp(argv[i],"-minuscules")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",minuscules);}
			if ((strcmp(argv[i],"-workdir")==0) && (i+1<argc))
			{
				sscanf(argv[i+1],"%s",workdir);
			}
			if ((strcmp(argv[i],"-image_width")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%d",&image_width);}
			if ((strcmp(argv[i],"-image_height")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%d",&image_height);}
			if ((strcmp(argv[i],"-file")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierentree);}
			if ((strcmp(argv[i],"-fileantidico")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierantidico);}
			if ((strcmp(argv[i],"-filefrequence")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierfrequence);}
			if ((strcmp(argv[i],"-fileout")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichiersortie);}
			if ((strcmp(argv[i],"-filehtml")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierhtml);}
			if ((strcmp(argv[i],"-filenexus")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierNexus);}
			if ((strcmp(argv[i],"-filegraph")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fileimage);}
			if ((strcmp(argv[i],"-filenewick")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierNewick);}
			if ((strcmp(argv[i],"-fileCSV")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierCSV);}
			if ((strcmp(argv[i],"-filedistance")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",fichierDistance);}
			if ((strcmp(argv[i],"-typedistance")==0) && (i+1<argc))
			{sscanf(argv[i+1],"%s",typedistance);}
			}
	}
	if (strcmp(debug,"OUI")==0) {verbose =1;}
	else verbose = 0;

	if (verbose)printf("debug:%s verbose:%d langue:%s\n",debug,verbose,langue);


	if ((strcmp(anti_perso,"OUI")== 0) || (strcmp(langue,"NONE") != 0) ) {

		if (verbose) printf("Traitement Unitex \n");

		sprintf(fichierantidico,"%s/%s",workdir,"Antidico_XXX.txt");

		if ((strcmp(anti_perso,"OUI")== 0)) {
			sprintf(anti_dico_perso,"%s/%s",workdir,"Antidico_Perso.txt");
			if (verbose) printf("anti_dico_perso:%s\n",anti_dico_perso);
		}
		traitement_unitex(fichierentree,tmp_annoted_fle,verbose, virtuel,langue,anti_perso,workdir,anti_dico_perso,1);

		if (verbose) printf("UNITEX TERMINE\n");
	}

Infos *infos = (Infos *) malloc(sizeof(Infos));

infos->nombremots = nombremots;
infos->taillefenetre = taillefenetre;
infos->typedistance = strdup(typedistance);


	#ifdef __CYGWIN__ //if windows we need to correct path name
	if (fichierentree[0]!=0)
		convert_path(fichierentree);
	if (fichierantidico[0]!=0)
		convert_path(fichierantidico);
	if (fichiersortie[0]!=0)
		convert_path(fichiersortie);
	if (fichierfrequence[0]!=0)
		convert_path(fichierfrequence);
	if (fichierCSV[0]!=0)
		convert_path(fichierCSV);
	if (fichierDistance[0]!=0)
		convert_path(fichierDistance);
	if (fichierNexus[0]!=0)
		convert_path(fichierNexus);
	if (fichierNewick[0]!=0)
		convert_path(fichierNewick);
	if (fileimage[0]!=0)
		convert_path(fileimage);
	#endif

	if (nombremots<3)
	{
		printf("Le nombre de mot doit etre > 3 : %d",nombremots);
		return 1;
	}
	if (taillefenetre<2)
	{
		printf("La taille de fenetre doit etre > 2 : %d",taillefenetre);
		return 1;
	}
	//Ouverture / Lecture du fichier d'entrée
	if (fichierentree[0]==0)
	{
		printf("%s\n",strhelp);
		printf("\nVous devez specifier un fichier a traiter exemple: \". -file 1.txt\"\n");
		exit(1);
	}
	text_buffer *t1;
	if ((strcmp(anti_perso,"OUI")== 0) || (strcmp(langue,"NONE") != 0) ) {
	t1=Open_File(tmp_annoted_fle);
	}
	else {
	t1=Open_File(fichierentree);
		}
	if (t1->buffer==0 || t1->size==0)
	{
		printf("%s non trouvé ou vide**\n",fichierentree);
		return 1;
	}

	if (verbose==1)
		printf("Fichier %s ouvert, taille :%d\n",fichierentree,t1->size);
	//Mise au format du texte
	Format_Text(t1,minuscules);
	if (verbose==1)
		printf("Texte au format\n");
	if (fichierantidico[0]!=0)
	{
		//Application de l'anti-dictionaire
		AntiDico(t1,fichierantidico,minuscules);
		if (verbose==1)
			printf("Anti-dico ok\n");
	}
	//Ecrit le texte formaté, sans les mots de l'anti-dictionaire

	if (fichiersortie[0]!=0)
	{
		Write_Text(t1,fichiersortie);
		if (verbose==1)
			u_fprintf(UTF8,stdout,"Ecriture du fichier de sortie :%s\n",fichiersortie);
	}


	//Calcul des fréquences des mots du texte
	freq_buffer *freq=Freq_Calc(t1);
	if (verbose==1)
		printf("....Calcul des fréquences fini\n");

	//Calcul de la fréquence  partir de laquelle on devra ne plus considérer les mots pour n'avoir que "nombremots" mots
	//si max=1 alors on gardera au maximum "nombremots" mots
	//sinon on gardera au minimum "nombremots" mots
	int fq=Freq_Number(freq,nombremots,max);	
	if (verbose==1)
		printf("Calcul de la fréquence max : %d\n",fq);

	
	//Ecriture des fréquences des mots gardés
	if (fichierfrequence[0]!=0)
	{
		Write_Freq_File(fichierfrequence,freq,fq);
		if (verbose==1)
			u_fprintf(UTF8,stdout,"Ecriture du fichier des fréquences :%s\n",fichierfrequence);
	}
	//Synthétisation du texte en suite de pointeurs
	point_texte* pt=Text_Pointer(t1,freq,fq);
	if (verbose==1)
		printf("Texte Synthetise en pointeurs\n");
	if (pt==0 || pt->nbgarde<2)
	{
		printf("Pas assez de mot gardés, veuillez augmenter le nombre de mots à garder\n");
		return 1;
	} else {
		 printf("%d mots gardés\n",pt->nbgarde);
	}
	//Calcul des cooccurrences
	if (pt->count>taillefenetre) // le texte doit contenir plus de mot que ce qu'impose la taille de la fenêtre
	{
		Cooccurrence(pt,taillefenetre);
	} else {
		u_fprintf(UTF8,stdout,"La taille de la fenêtre doit etre < au nombre de mots du texte :\ntaillefenêtre:%d\nnombre de mot:%d\n",taillefenetre,pt->count);
		return 1;
	}
	if (verbose==1)
		printf("Cooccurrences calculées\n");
	//Ecriture des matrices o11,o12,o21,o22
	if (fichierCSV[0]!=0)
	{
		Write_CSVMatrice(pt,fichierCSV);
		if (verbose==1)
			printf("Ecriture des fichiers de Matrices o11,o12,o21,o22 :%s\n",fichierCSV);	
	}
	//Calcul des distances
	double *distance=Distance(pt,typedistance);
	if (verbose==1)
		printf("Calcul des distances fini\n");
	//Ecriture du fichier des distances
	if (fichierDistance[0]!=0)
	{
		Write_CSVDistance(pt,distance,fichierDistance);
		if (verbose==1)
			printf("Ecriture du fichier des distances :%s\n",fichierDistance);	
	}
	
	//Ecrit le Nexus si voulu
	if (fichierNexus[0]!=0)
	{
		EcrireNexus(pt,distance,fichierNexus);
		if (verbose==1)
			printf("Ecriture du fichier Nexus :%s\n",fichierNexus);
	}

	//Calcul du Newik (si on veut le Newik ou image) BioNJ

	if (fichierNewick[0]!=0 || fileimage[0]!=0)
	{
		PhyloTree *tre=GenerationPhyloTreeBioNJ(pt,distance);
		//char* tree1=GenerationNewik(pt,distance);
		if (verbose==1) {
			printf("Newick terminé");
		u_fprintf(UTF8,stdout," :\n%S\n",tre->Newick);
		}
		if (fichierNewick[0]!=0)
		{
			FILE *f=u_fopen(UTF8,fichierNewick,U_WRITE);
			u_fprintf(UTF8,f,"%S;",tre->Newick);
			u_fclose(f);
			if (verbose==1)
				printf("Ecriture du fichier Newick :%s\n",fichierNewick);
		}
		if (fileimage[0]!=0)
		{
			GenererDessin(pt,tre,image_width,image_height,120,120,fileimage,verbose,infos,legend);
			if (verbose==1)
				printf("Calcul et écriture du fichier image :%s\n",fileimage);
		}
	}


//Calling the function implementing the algorithm for tree construction by Barthelemy and Luong
		if(verbose==1)
			printf("Starting the tree construction using the algorithm by Barthelemy and Luong..\n");

unichar *Newick=(unichar*)malloc(sizeof(unichar));
		unichar* file=GenTreeBL(fichierNewick,pt,distance,verbose,Newick,fichierhtml);


FILE *pFile;

	pFile=u_fopen(UTF8,fichierhtml,U_WRITE);
	if (pFile==0)
{
		return 0;
}
	u_fwrite(UTF8,file,u_strlen(file),pFile);
	u_fclose(pFile);
	if(verbose==1)
			printf("Computation of tree structure completed....asdkfjhskdjfh\n");


printf("Written to file %s\n",fichierhtml);						

	if (verbose==1)
		printf("Fini !\n");
	return 0;
}
