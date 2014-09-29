/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

#ifndef phylo_h
#define phylo_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> /*log() */

#include "fonts.h"  //str_width, str_height
#include "cooccurrences.h" //macro distance, cooccurrences, distances
#include "read_write.h" //freq_buffer, text_buffer, point_texte
#include "arbredico.h" //Arbre_Dico_Init, Arbre_Dico_ADD, Arbre_Dico_FOUND

#include "unicode_flex.h"


#define str_buffer 10000

typedef struct RectN
{
int x;
int y;
int width;
int height;
int x1; //x+width
int y1; //y+height
int vx; //vector used for placement
int vy; //vector used for placement
int r,g,b;// colors
int l0x,l0y,l1x,l1y; //Line drawing
int fontsize;
int freq;
unichar str[256];
struct RectN *next;
} RectN;


typedef struct PhyloNode
{
	struct PhyloNode *Fils0;  //liste de fils
	struct PhyloNode *Fils1;  //liste de fils
	double weight; //length of the edge leading to the node
	double angle; //angle of this edge
	double minangle; //minimal angle below this node
	double maxangle; //maximal angle below this node
	int size; //font size of the label of the node
	//int r; //red color of the label of the node
	//int g; //green color of the label of the node
	//int b; //blue color of the label of the node
	double x; //coordinates of the node in a precomputing of the drawing
		//(before stretching to adapt to the window size)
	double y;
	int refnumber; //number of the line to look for the nb of occurences in Frequency.linesname  : string;
	unichar name[str_buffer];
} PhyloNode;

typedef struct PhyloTree
{
	//int r; //red color of the edges
    //int g;
    //int b;
    double xmin; //x min among all nodes of the tree (computed during drawing precomputing)
    double xmax; //...
    double ymin;
    double ymax;
    struct PhyloNode *root;
	unichar Newick[str_buffer];
    double angle; //general angle to add to the tree (to rotate it)
} PhyloTree;

typedef struct Infos
{
	char *typedistance;
	int nombremots;
	int taillefenetre;
} Infos;

typedef struct Tree
{
	int Adj[500][500];
	int connect[500];
	int num;	//No of vertices
	int noofwords;	//No of words(leaves)
	unichar words[500][500];	//To correspond to the id
	double length[500][500];	//lengths
	char newick[500][500];	//To store the newick string for all the vertices

}Tree;


void GenererDessin(point_texte* pt,PhyloTree *tre,int xmax,int ymax,int xx,int yy,char *file,int verbose,Infos *,char *legend);
PhyloTree *GenerationPhyloTreeBioNJ(point_texte* pt,double *distance);
unichar* GenTreeBL(char fichierNewick[256],point_texte *pt,double *distance,int verbose,unichar* Newick,char fichierhtml[256]);





#endif

