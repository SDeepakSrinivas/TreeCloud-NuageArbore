/*
	"  *************************************************\n"\
	"  *          Generation d'Arbre Arbore            *\n"\
	"  *       fait par Jean-Charles BONTEMPS          *\n"\
	"  *                Licence GPL                    *\n"\
	"  *   http://poulphunter1.free.fr/ArbreArbore     *\n"\
	"  *           poulphunter@yahoo.fr                *\n"\
	"  *************************************************\n\n"\
*/

// To compile the whole code:
// make
// make clean

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> /*log() */


#include "fonts.h"  //str_width, str_height
#include "cooccurrences.h" //macro distance, cooccurrences, distances
#include "read_write.h" //freq_buffer, text_buffer, point_texte
#include "arbredico.h" //Arbre_Dico_Init, Arbre_Dico_ADD, Arbre_Dico_FOUND
#include "SVG.h"
#include "Phylo.h"

#include "unicode_flex.h"


PhyloNode *PhyloNode_Create()
{
	PhyloNode *t=(PhyloNode *)malloc(sizeof(PhyloNode));
	if (t==0)
		return 0;
	t->Fils0=0;
	t->Fils1=0;
	t->weight=0;
	t->angle=0;
	t->name[0]=0;
	t->size=10;
	//t->r=0;
	//t->g=0;
	//t->b=0;
	t->x=50000;
	t->y=50000;
	t->refnumber=0;

	return t;
}
PhyloTree *PhyloTree_Create()
{
	PhyloTree *t=(PhyloTree *)malloc(sizeof(PhyloTree));
	t->angle=0;
	t->Newick[0]=0;
	//t->r=200;
	//t->g=210;
	//t->b=255;
	t->xmin=100000;
	t->ymin=100000;
	t->xmax=-1;
	t->ymax=-1;
	t->root=0;
return t;
}

int sortLeaves(point_texte* pt,PhyloNode *t,int sofar,int tre_angle)
{
	int isleaf=0;
	int res=sofar;
	if (t->Fils0!=0){isleaf=1;res=sortLeaves(pt,t->Fils0,res,tre_angle);}
	if (t->Fils1!=0){isleaf=1;res=sortLeaves(pt,t->Fils1,res,tre_angle);}
	if (isleaf==0)
	{
		#define pi 3.14159265358979323846
		t->angle=(sofar*2*pi)/(pt->nbgarde+2+tre_angle);
		t->minangle=t->angle;
		t->maxangle=t->angle;
		t->weight=1;
		res=sofar+1;
	}
	else
	{
		if (t->Fils1!=0)
		{
			t->minangle=min(t->Fils0->minangle,t->Fils1->minangle);
			t->maxangle=max(t->Fils0->maxangle,t->Fils1->maxangle);
			t->angle=(t->minangle+t->maxangle)/2;
		} else {
			t->angle=(t->minangle+t->maxangle)/2;
		}
	}
	return res;
}

void RectN_add(RectN *rec,int x,int y,int w,int h,int re,int g,int b,int fontsize,int freq,unichar *st,int l0x,int l0y,int l1x,int l1y)
{ //insert between first and 2nd RectN in list (like that we don't have to give back the pointer of new list)
	if (rec==0)
		return;
	RectN *r=(RectN *)malloc(sizeof(RectN));
	r->next=rec->next;
	r->x=x;
	r->y=y;
	r->width=w;
	r->height=h;
	r->x1=x+w;
	r->y1=y+h;
	r->vx=0;
	r->vy=0;
	r->r=re;
	r->g=g;
	r->b=b;
	r->fontsize=fontsize;
	r->freq=freq;
	r->l0x=l0x;
	r->l0y=l0y;
	r->l1x=l1x;
	r->l1y=l1y;
	u_strncpy(r->str,st,256);
	rec->next=r;
}

void computeDrawing(PhyloNode *t,double x,double y,PhyloTree *tre)
{
	if (t->Fils0!=0)
	{
		t->Fils0->x=floor(x+50*cos(t->Fils0->angle));
		t->Fils0->y=floor(y+50*sin(t->Fils0->angle));
		if (t==tre->root) {t->Fils0->x=t->x;t->Fils0->y=t->y;}
		if (t->Fils0->x<tre->xmin) tre->xmin=t->Fils0->x;
		if (t->Fils0->y<tre->ymin) tre->ymin=t->Fils0->y;
		if (t->Fils0->x>tre->xmax) tre->xmax=t->Fils0->x;
		if (t->Fils0->y>tre->ymax) tre->ymax=t->Fils0->y;
		computeDrawing(t->Fils0,t->Fils0->x,t->Fils0->y,tre);
		//printf("tfils %f %f\n",t->Fils0->x,t->Fils0->y);

	}
	if (t->Fils1!=0)
	{
		t->Fils1->x=floor(x+50*cos(t->Fils1->angle));
		t->Fils1->y=floor(y+50*sin(t->Fils1->angle));
		if (t==tre->root) {t->Fils1->x=t->x;t->Fils1->y=t->y;}
		if (t->Fils1->x<tre->xmin) tre->xmin=t->Fils1->x;
		if (t->Fils1->y<tre->ymin) tre->ymin=t->Fils1->y;
		if (t->Fils1->x>tre->xmax) tre->xmax=t->Fils1->x;
		if (t->Fils1->y>tre->ymax) tre->ymax=t->Fils1->y;
		computeDrawing(t->Fils1,t->Fils1->x,t->Fils1->y,tre);
		//printf("tfils %f %f\n",t->Fils0->x,t->Fils0->y);
	}
}

void draw(point_texte* pt,text_buffer *imageSVG,PhyloNode *t,int x,int y,int xx,int yy,PhyloTree *tre,double frmax,double frmin,RectN *rec)
{
	double a=(x-2*xx)/(tre->xmax-tre->xmin);
	double b=(y-2*yy)/(tre->ymax-tre->ymin);
	double newx,newy,level,level1,xm,ym,lx0,ly0,lx1,ly1;
	int h=0,w=0,angle;
       int levelr=184,levelg=201,levelb=214;
	int k;//color tre->r,tre->g,tre->b
	if (t->Fils0!=0)
	{
		//draw the edge to the currently treated son:
		lx0=floor((t->x-tre->xmin)*a)+xx;
		ly0=floor((t->y-tre->ymin)*b)+yy;
		lx1=floor((t->Fils0->x-tre->xmin)*a)+xx;
		ly1=floor((t->Fils0->y-tre->ymin)*b)+yy;
		if (t->Fils0->name[0]!=0 && t->Fils0->name[0]!='(')
		{
			//si le nom n'est pas vide:
			k=t->Fils0->refnumber-1;
			level=floor(55+200*((log2((double)freq(k))-log2(frmin))/(log2(frmax)-log2(frmin))));
			//en fonction du nombre de mot d�finit la taille.
			level1=floor((level*((int)ceil((200-min(((double)pt->nbgarde),200.0))*13.0/200.0)))/256)+10;

                     levelr=184;
                     levelg=201;
                     levelb=214;

                     if (level1>12){
                         levelr=102;
                         levelg=138;
                         levelb=168;
                     }
                     if (level1>13){
                         levelr=71;
                         levelg=123;
                         levelb=123;
                     }
                     if (level1>14){
                         levelr=231;
                         levelg=98;
                         levelb=0;
                     }
                     if (level1>19){
                         levelr=255;
                         levelg=51;
                         levelb=0;
                     }

			//level1=((int)ceil((200-min(((double)pt->nbgarde),200.0))*15.0/200.0))+5+floor((level*5)/256);
			h=ceil(str_height(level1)/2.0);
			w=str_width(t->Fils0->name,1,level1);
			ym=h;
			xm=w+10;
			newx=(t->Fils0->x-tre->xmin)*a-(xm*sin(t->Fils0->angle/2))+xx+5;
                     newy=(t->Fils0->y-tre->ymin)*b-(ym)/2*sin(t->Fils0->angle+pi)+5+yy;
			angle=0;
			if (t->Fils0->angle>=pi/2 && t->Fils0->angle<pi)
				RectN_add(rec,lx1-(w+10),ly1,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils0->name,lx1,ly1,lx1,ly1);
			if (t->Fils0->angle>=0 && t->Fils0->angle<pi/2)
				RectN_add(rec,lx1,ly1,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils0->name,lx1,ly1,lx1,ly1);
			if (t->Fils0->angle>=pi*3/2 && t->Fils0->angle<=pi*2)
				RectN_add(rec,lx1,ly1-h*2,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils0->name,lx1,ly1,lx1,ly1);
			if (t->Fils0->angle>=pi && t->Fils0->angle<pi*3/2)
				RectN_add(rec,lx1-(w+10),ly1-(h*2),w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils0->name,lx1,ly1,lx1,ly1);
		}
		imageSVG->size+=SVG_Line(&imageSVG->buffer[imageSVG->size],(int)lx0,(int)ly0,(int)lx1,(int)ly1);
		draw(pt,imageSVG,t->Fils0,x,y,xx,yy,tre,frmax,frmin,rec);

	}

	if (t->Fils1!=0 && t->Fils1!=t->Fils0)
	{
		//draw the edge to the currently treated son:
		lx0=floor((t->x-tre->xmin)*a)+xx;
		ly0=floor((t->y-tre->ymin)*b)+yy;
		lx1=floor((t->Fils1->x-tre->xmin)*a)+xx;
		ly1=floor((t->Fils1->y-tre->ymin)*b)+yy;
		if (t->Fils1->name[0]!=0 && t->Fils1->name[0]!='(')
		{
			//si le nom n'est pas vide:
			k=t->Fils1->refnumber-1;
			level=floor(55+200*((log2((double)freq(k))-log2(frmin))/(log2(frmax)-log2(frmin))));
			//en fonction du nombre de mot d�finit la taille.
			level1=floor((level*((int)ceil((200-min(((double)pt->nbgarde),200.0))*13.0/200.0)))/256)+10;

                     levelr=184;
                     levelg=201;
                     levelb=214;

                     if (level1>12){
                         levelr=102;
                         levelg=138;
                         levelb=168;
                     }
                     if (level1>13){
                         levelr=71;
                         levelg=123;
                         levelb=123;
                     }
                     if (level1>14){
                         levelr=231;
                         levelg=98;
                         levelb=0;
                     }
                     if (level1>19){
                         levelr=255;
                         levelg=51;
                         levelb=0;
                     }

			h=ceil(str_height(level1)/2.0);
			w=str_width(t->Fils1->name,1,level1);
			ym=h;
			xm=w+10;
			newx=(t->Fils1->x-tre->xmin)*a-(xm*sin(t->Fils1->angle/2))+xx+5;
                     newy=(t->Fils1->y-tre->ymin)*b-(ym)/2*sin(t->Fils1->angle+pi)+5+yy;
			angle=0;
			if (t->Fils1->angle>=pi/2 && t->Fils1->angle<pi)
				RectN_add(rec,lx1-(w+10),ly1,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils1->name,lx1,ly1,lx1,ly1);
			if (t->Fils1->angle>=0 && t->Fils1->angle<pi/2)
				RectN_add(rec,lx1,ly1,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils1->name,lx1,ly1,lx1,ly1);
			if (t->Fils1->angle>=pi*3/2 && t->Fils1->angle<=pi*2)
				RectN_add(rec,lx1,ly1-h*2,w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils1->name,lx1,ly1,lx1,ly1);
			if (t->Fils1->angle>=pi && t->Fils1->angle<pi*3/2)
				RectN_add(rec,lx1-(w+10),ly1-(h*2),w+10,h*2,levelr,levelg,levelb,level1,freq(k),t->Fils1->name,lx1,ly1,lx1,ly1);
		}
		imageSVG->size+=SVG_Line(&imageSVG->buffer[imageSVG->size],(int)lx0,(int)ly0,(int)lx1,(int)ly1);
		draw(pt,imageSVG,t->Fils1,x,y,xx,yy,tre,frmax,frmin,rec);
	}
}

#define col_limite 1
void draw_end(RectN *rec,text_buffer *imageSVG,int vector_percent){
	if (rec==0)
		return;
	int vx=ceil((double)vector_percent*rec->vx/100.0);
	int vy=ceil((double)vector_percent*rec->vy/100.0);
	if (rec->height!=0)
		imageSVG->size+=SVG_Text(&imageSVG->buffer[imageSVG->size],rec->x+5+vx,rec->y+(rec->height*4/5)+vy,rec->r,rec->g,rec->b,rec->str,rec->freq,rec->fontsize);
	// imageSVG->size+=SVG_Rect(&imageSVG->buffer[imageSVG->size],rec->x+vx+col_limite,rec->y+vy+col_limite,rec->width-(2*col_limite),rec->height-(2*col_limite));
	// imageSVG->size+=SVG_Line_Point(&imageSVG->buffer[imageSVG->size],rec->l0x,rec->l0y,rec->l1x+vx,rec->l1y+vy);
	draw_end(rec->next,imageSVG,vector_percent);
}

// ((r2->x<=r1->x1 && r2->x>=r1->x && ((r2->y<=r1->y1 && r2->y>=r1->y) || (r2->y1<=r1->y1 && r2->y1>=r1->y))) ||
// (r2->x1<=r1->x1 && r2->x1>=r1->x && ((r2->y<=r1->y1 && r2->y>=r1->y) || (r2->y1<=r1->y1 && r2->y1>=r1->y))) ||
// (r1->x<=r2->x1 && r1->x>=r2->x && ((r1->y<=r2->y1 && r1->y>=r2->y) || (r1->y1<=r2->y1 && r1->y2>=r1->y))) ||
// (r1->x1<=r2->x1 && r1->x1>=r2->x && ((r1->y<=r2->y1 && r1->y>=r2->y) || (r1->y1<=r2->y1 && r1->y2>=r1->y))))

int inclu(RectN *r1,RectN *r2)
{
	if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x1 && r1->y1>r2->y1)
		return 1;
	return 0;
}

int chevauche(RectN *r1,RectN *r2)
{
	if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x && r1->y1>r2->y && r1->x1>r2->x1)
		return 1;
	if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x && r1->y1>r2->y && r1->y1>r2->y1)
		return 1;
	if (r1->x1>r2->x1 && r1->y1>r2->y1 && r1->x<r2->x1 && r1->y<r2->y1 && r1->x<r2->x)
		return 1;
	if (r1->x1>r2->x1 && r1->y1>r2->y1 && r1->x<r2->x1 && r1->y<r2->y1 && r1->y<r2->y)
		return 1;
	return 0;
}

int collision(RectN *r1,RectN *r2)
{
	if (((r2->x<=r1->x1 && r2->x>=r1->x && ((r2->y<=r1->y1 && r2->y>=r1->y) || (r2->y1<=r1->y1 && r2->y1>=r1->y))) ||
		(r2->x1<=r1->x1 && r2->x1>=r1->x && ((r2->y<=r1->y1 && r2->y>=r1->y) || (r2->y1<=r1->y1 && r2->y1>=r1->y))) ||
		(r1->x<=r2->x1 && r1->x>=r2->x && ((r1->y<=r2->y1 && r1->y>=r2->y) || (r1->y1<=r2->y1 && r1->y1>=r2->y))) ||
		(r1->x1<=r2->x1 && r1->x1>=r2->x && ((r1->y<=r2->y1 && r1->y>=r2->y) || (r1->y1<=r2->y1 && r1->y1>=r2->y)))))
		return 1;
	return 0;
}

int compute_vector(RectN *r1){
#define col_lim 1

#define add_col r1->x+=col_limite;r1->x1-=(2*col_limite);r1->y+=col_limite;r1->y1-=(2*col_limite);r2->x+=col_limite;r2->x1-=(2*col_limite);r2->y+=col_limite;r2->y1-=(2*col_limite);
#define del_col r1->x-=col_limite;r1->x1+=(2*col_limite);r1->y-=col_limite;r1->y1+=(2*col_limite);r2->x-=col_limite;r2->x1+=(2*col_limite);r2->y-=col_limite;r2->y1+=(2*col_limite);

#define add_vector add_col;r1->x+=r1->vx;r1->x1+=r1->vx;r1->y+=r1->vy;r1->y1+=r1->vy;r2->x+=r2->vx;r2->x1+=r2->vx;r2->y+=r2->vy;r2->y1+=r2->vy;vx=0;vy=0;
#define del_vector del_col;r1->x-=r1->vx;r1->x1-=r1->vx;r1->y-=r1->vy;r1->y1-=r1->vy;r2->x-=r2->vx;r2->x1-=r2->vx;r2->y-=r2->vy;r2->y1-=r2->vy;
#define add 2
#define vx1 ((vx)<(0)?((vx)>(-1)?(-1):(vx)):((vx)<(1)?(1):(vx)))
#define vy1 ((vy)<(0)?((vy)>(-1)?(-1):(vy)):((vy)<(1)?(1):(vy)))
#define vx2 ((vx)<(0)?((vx)>(-col_lim)?(-1):(vx1)):((vx)<(col_lim)?(1):(vx1)))
#define vy2 ((vy)<(0)?((vy)>(-col_lim)?(-1):(vy1)):((vy)<(col_lim)?(1):(vy1)))

#define addv1 r1->vx+=vx2;r1->vy+=vy2;r2->vx-=vx2;r2->vy-=vy2;

#define cont1 del_vector;addv1;r2=r2->next;continue;
#define cont del_vector;r2=r2->next;continue;

	int vx,vy,ret=0;
	if (r1==0)
		return 0;
	RectN *r2=r1->next;
	while (r2!=NULL)
	{
		add_vector
		if (chevauche(r1,r2) || chevauche(r2,r1))
		{	ret=1;
			//printf("chevauche:(%s,%s)\n",r1->str,r2->str);
			if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x && r1->y1>r2->y && r1->x1>r2->x1)
			{
				vy-=ceil((r2->y-r1->y)/2)+add;cont1
			}
			if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x && r1->y1>r2->y && r1->y1>r2->y1)
			{
				vx-=ceil((r2->x-r1->x)/2)+add;cont1
			}
			if (r1->x1>r2->x1 && r1->y1>r2->y1 && r1->x<r2->x1 && r1->y<r2->y1 && r1->x<r2->x)
			{
				vy+=ceil((r2->y1-r1->y)/2)+add;cont1
			}
			if (r1->x1>r2->x1 && r1->y1>r2->y1 && r1->x<r2->x1 && r1->y<r2->y1 && r1->y<r2->y)
			{
				vx+=ceil((r2->x1-r1->x)/2)+add;cont1
			}
			if (r2->x<r1->x && r2->y<r1->y && r2->x1>r1->x && r2->y1>r1->y && r2->x1>r1->x1)
			{
				vy+=ceil((r1->y-r2->y)/2)+add;cont1
			}
			if (r2->x<r1->x && r2->y<r1->y && r2->x1>r1->x && r2->y1>r1->y && r2->y1>r1->y1)
			{
				vx+=ceil((r1->x-r2->x)/2)+add;cont1
			}
			if (r2->x1>r1->x1 && r2->y1>r1->y1 && r2->x<r1->x1 && r2->y<r1->y1 && r2->x<r1->x)
			{
				vy-=ceil((r1->y1-r2->y)/2)+add;cont1
			}
			if (r2->x1>r1->x1 && r2->y1>r1->y1 && r2->x<r1->x1 && r2->y<r1->y1 && r2->y<r1->y)
			{
				vx-=ceil((r1->x1-r2->x)/2)+add;cont1
			}

		} else {
			if (collision(r1,r2) || collision(r2,r1))
			{	ret=1;//printf("collision:(%s,%s)\n",r1->str,r2->str);
				if (r1->x<r2->x && r1->y<r2->y && r1->x1>r2->x && r1->y1>r2->y)
				{
					//d�place sur l'axe des x ou y selon l'eloignement que sa fait par rapport � la position d'origine
					if (abs(r1->vx-(ceil((r1->x1-r2->x)/2)+add))<abs(r1->vx))//si distance future de vx < vx actuel
					{
						vx-=ceil((r1->x1-r2->x)/2)+add;cont1
					}
					if (abs(r1->vy-ceil((r1->y1-r2->y)/2)+add)<abs(r1->vy))//si distance future de vy < vy actuel
					{
						vy-=ceil((r2->y-r1->y)/2)+add;cont1
					}
					//printf("%d  %d\n",(r1->y1-r2->y)/2+add,(r1->x1-r2->x)/2+add);
					if (ceil((r1->y1-r2->y)/2)+add<=ceil((r1->x1-r2->x)/2)+add) //sinon on prend le plus court
					{
						vy-=ceil((r1->y1-r2->y)/2)+add;cont1
					} else {
						vx-=ceil((r1->x1-r2->x)/2)+add;cont1
					}
				}
				if (r2->x<r1->x && r2->y<r1->y && r2->x1>r1->x && r2->y1>r1->y)
				{
					//d�place sur l'axe des x ou y selon l'eloignement que sa fait par rapport � la position d'origine
					if (abs(r2->vx+(ceil((r2->x1-r1->x)/2)+add))<abs(r2->vx))//si distance future de vx < vx actuel
					{
						vx+=ceil((r2->x1-r1->x)/2)+add;cont1
					}
					if (abs(r2->vy+ceil((r2->y1-r1->y)/2)+add)<abs(r2->vy))//si distance future de vy < vy actuel
					{
						vy+=ceil((r1->y-r2->y)/2)+add;cont1
					}
					//printf("%d  %d\n",(r2->y1-r1->y)/2+add,(r2->x1-r1->x)/2+add);
					if (ceil((r2->y1-r1->y)/2)+add<=ceil((r2->x1-r1->x)/2)+add) //sinon on prend le plus court
					{
						vy+=ceil((r2->y1-r1->y)/2)+add;cont1
					} else {
						vx+=ceil((r2->x1-r1->x)/2)+add;cont1
					}
				}

				if (r1->x<r2->x1  && r1->y<r2->y && r1->x1>r2->x1 && r1->y1>r2->y)
				{
					//d�place sur l'axe des x ou y selon l'eloignement que sa fait par rapport � la position d'origine
					if (abs(r1->vx+(ceil((r2->x1-r1->x)/2)+add))<abs(r1->vx))//si distance future de vx < vx actuel
					{
						vx+=ceil((r2->x1-r1->x)/2)+add;cont1
					}
					if (abs(r1->vy-ceil((r1->y1-r2->y)/2)+add)<abs(r1->vy))//si distance future de vy < vy actuel
					{
						vy-=ceil((r2->y-r1->y)/2)+add;cont1
					}
					//printf("%d  %d\n",(r1->y1-r2->y)/2+add,(r1->x1-r2->x)/2+add);
					if (ceil((r1->y1-r2->y)/2)+add<=ceil((r2->x1-r1->x)/2)+add) //sinon on prend le plus court
					{
						vy-=ceil((r1->y1-r2->y)/2)+add;cont1
					} else {
						vx+=ceil((r2->x1-r1->x)/2)+add;cont1
					}
				}
				if (r2->x<r1->x1  && r2->y<r1->y && r2->x1>r1->x1 && r2->y1>r1->y)
				{
					//d�place sur l'axe des x ou y selon l'eloignement que sa fait par rapport � la position d'origine
					if (abs(r2->vx-(ceil((r1->x1-r2->x)/2)+add))<abs(r2->vx))//si distance future de vx < vx actuel
					{
						vx-=ceil((r1->x1-r2->x)/2)+add;cont1
					}
					if (abs(r2->vy+ceil((r2->y1-r1->y)/2)+add)<abs(r2->vy))//si distance future de vy < vy actuel
					{
						vy+=ceil((r1->y-r2->y)/2)+add;cont1
					}
					//printf("%d  %d\n",(r2->y1-r1->y)/2+add,(r2->x1-r1->x)/2+add);
					if (ceil((r2->y1-r1->y)/2)+add<=ceil((r1->x1-r2->x)/2)+add) //sinon on prend le plus court
					{
						vy+=ceil((r2->y1-r1->y)/2)+add;cont1
					} else {
						vx-=ceil((r1->x1-r2->x)/2)+add;cont1
					}
				}
				if (r1->x==r2->x)
				{
					vx+=1;cont1
				}
				if (r1->y==r2->y)
				{
					vy+=1;cont1
				}
				if (r1->x1==r2->x1)
				{
					vx+=1;cont1
				}
				if (r1->y1==r2->y1)
				{
					vy+=1;cont1
				}
			}
		}
		cont
	}
	if (compute_vector(r1->next)==1)
		return 1;
	else
		return ret;
}

void RectN_free(RectN *rec)
{
	if (rec==0)
		return;
	RectN_free(rec->next);
	free(rec);
}

int find_max(RectN *r1){
	RectN *r=r1;
	int max=0;
	while (r!=0)
	{
		if (abs(r->vx)>max)
			max=abs(r->vx);
		if (abs(r->vy)>max)
			max=abs(r->vy);
		r=r->next;
	}
	return max;
}

void red_vector(RectN *r1){
	//int max=find_max(r1)/2;
	RectN *r=r1;
	while (r!=0)
	{
		// if (abs(r->vx)>max/2 || abs(r->vy)>max/2)
		// {
			if (r1->vx>0)
				r1->vx-=2;
			if (r1->vx<0)
				r1->vx+=2;
			if (r1->vy>0)
				r1->vy-=2;
			if (r1->vy<0)
				r1->vy+=2;
		// }
		r=r->next;
	}
}

void pre_draw(point_texte* pt,text_buffer *imageSVG,PhyloNode *t,int x,int y,int xx,int yy,PhyloTree *tre)
{
	int i,j;
	//Calcul qu'une fois la frequence max et min
	double frmax=freq(0)*1.0;
	double frmin=freq(0)*1.0;
	for (i=0;i<pt->nbgarde;i++)
	{
		if (freq(i)>frmax) {frmax=freq(i);}
		if (freq(i)<frmin) {frmin=freq(i);}
	}
	RectN *rec=(RectN *)malloc(sizeof(RectN));
	rec->x=0;rec->y=0;rec->width=0;rec->height=0;rec->x1=0;rec->y1=0;rec->vx=0;rec->vy=0;rec->next=0;
	//new rec initialised to 0
	draw(pt,imageSVG,t,x,y,xx,yy,tre,frmax,frmin,rec);
	for (i=0;i<100;i++)
	{
		j=0;
		while (!(compute_vector(rec)!=0 || j<100))
		{j++;}
		red_vector(rec);
	}
	// while (compute_vector(rec)==0)
	// {}

	draw_end(rec,imageSVG,100);
	//free(rec);
	RectN_free(rec);//correction claude

}


unichar *tree(int i,unichar* tree1) {
return ((unichar *)(tree1+str_buffer*i*sizeof(unichar)));
}

PhyloTree *GenerationPhyloTreeBioNJ(point_texte* pt,double *distance)
{
//Algorithme Bio Neighbor-joining
	PhyloTree *tre=PhyloTree_Create();
	PhyloNode *t=0,*t1=0,*t2=0,*t3=0;
	Arbre_Dico* dic=Arbre_Dico_Init();
	Arbre_Dico* di=0;
	int i=pt->nbgarde;
	unichar* tree1=(unichar *)malloc(str_buffer*(i+1)*sizeof(unichar));//laisse un gros buffer de string
	unichar* strtmp=(unichar *)malloc(str_buffer*2*sizeof(unichar));
	unichar* strtmp2=(unichar *)malloc(str_buffer*2*sizeof(unichar));
	unichar* strtmp1=(unichar *)malloc(str_buffer*2*sizeof(unichar));
	strtmp[0]=0;
	strtmp1[0]=0;
	strtmp2[0]=0;
	if (tree1==0)
	{
		printf("erreur malloc tree1\n");
		exit(1);
	}

	int j=0,k=0,l=0;
	for (j=1;j<=i;j++) //copie la liste de mots dans tree()
	{

		u_strcpy(tree(j,tree1),mot(j-1));
		t=PhyloNode_Create();
		t->refnumber=j;
		u_strcpy(t->name,tree(j,tree1));
		di=Arbre_Dico_ADD(dic,tree(j,tree1));
		di->number=(long int)t;
	}
	double *ad=(double *)malloc((i+2)*(i+2)*sizeof(double));
	int *aflag=(int *)malloc((i+2)*sizeof(int));
	double *ad1=(double *)malloc((i+2)*(i+2)*sizeof(double));
	double *ad2=(double *)malloc((i+2)*sizeof(double));
	#define ad(k,l) (*(ad+k*i+l))
	#define ad1(k,l) (*(ad1+k*i+l))
	#define true 1
	#define false 0
	//Initialisation m�moire � 0
	for (j=0;j<=i;j++)
	{
		aflag[j]=0;
		ad2[j]=0.0;
		for (k=0;k<=i;k++)
		{
			ad(j,k)=0.0;
			ad1(j,k)=0.0;
		}
	}
	int i1;
	for(i1=1;i1<=i;i1++)
		aflag[i1]=true;
	int j1,j2;
	for(j1=1;j1<=i;j1++)
	{
		ad(j1,j1)=0.0;
		for(j2=1;j2<=i;j2++)
		{
			ad(j1,j2)=distance((j1-1),(j2-1));
			ad1(j1,j2)=ad(j1,j2);
		}
	}
	int k1,k2;
	for(k1=1;k1<=i;k1++)
	{
		for(k2=1;k2<=i;k2++)
			ad2[k1]+=ad(k1,k2);
	}
	int l1;
	for(l1=i;l1>3;l1--)
	{
		double d6 = 1.7976931348623157E+308;
		int i3,j3;
		for(i3=1;i3<i;i3++)
		{
			if(!aflag[i3])
				continue;
			for(j3=i3+1;j3<=i;j3++)
			{
				if(!aflag[j3])
					continue;
				double d7=((double)l1-2.0)*ad(i3,j3)-ad2[i3]-ad2[j3];
				if(d7<d6)
				{
					d6=d7;
					k=i3;
					l=j3;
				}
			}
		}
		double d2=0.5*((ad(k,l)+ad2[k]/((double)l1-2.0))-ad2[l]/((double)l1-2.0));
		double d4=ad(k,l)-d2;
		aflag[l]=false;

		u_strcpy(strtmp,tree(k,tree1));
		u_strcpy(strtmp2,tree(l,tree1));
		u_sprintf(strtmp1,"(%S,%S)",tree(k,tree1),tree(l,tree1));
		tree(l,tree1)[0]=0;
		u_strcpy(tree(k,tree1),strtmp1);

		ad2[k]=0.0;
		ad2[l]=0.0;
		double d8=ad1(k,l);
		double d5=0.0;
		if(d8==0.0||l1==3.0)
		{
			d5=0.5;
		} else
		{
			int k3;
			for(k3=1;k3<=i;k3++)
				if(k!=k3&&l!=k3&&ad(0,k3)!=0.0)
					d5+=ad1(k,k3)-ad1(l,k3);
			d5=0.5+d5/(2.0*(double)(l1-2.0)*d8);
			if(d5<0.0)
				d5=0.0;
			if(d5>1.0)
				d5=1.0;
		}
		int l3;
		for(l3=1;l3<=i;l3++)
		{
			if(l3==k||!aflag[l3])
				continue;
			double d=(1.0-d5)*(ad(l3,k)-d2)+d5*(ad(l3,l)-d4);
			if(l3!=k)
				ad2[l3]=(ad2[l3]-ad(l3,k)-ad(l3,l))+d;
			ad2[k]+=d;
			ad(k,l3)=ad(l3,k)=d;
			ad1(k,l3)=((1.0-d5)*ad1(k,l3)+d5*ad1(l,l3))-d5*(1.0-d5)*d8;
			ad1(l3,k)=ad1(k,l3);
		}
		int i4;
		for(i4=1;i4<=i;i4++)
		{
			ad(k,i4)=ad(i4,k);
			ad(i4,l)=0.0;
			ad(l,i4)=0.0;
		}

		t=PhyloNode_Create();
		di=Arbre_Dico_ADD(dic,tree(k,tree1));
		di->number=(long int)t;
		u_strcpy(t->name,tree(k,tree1));
		di=Arbre_Dico_FOUND(dic,strtmp);
		if (di==0)
		{
			u_fprintf(UTF8,stderr,"Dico Error %S 1\n",strtmp);return 0;
		}
		t1=(PhyloNode *)di->number;
		t->Fils0=t1;
		t1->weight=d2;
		di=Arbre_Dico_FOUND(dic,strtmp2);
		if (di==0)
		{
			u_fprintf(UTF8,stderr,"Dico Error%S 2\n",strtmp2);return 0;
		}
		t2=(PhyloNode *)di->number;
		t->Fils1=t2;
		t2->weight=d4;
	}
	int l2;
	for(l2=1;!aflag[l2];l2++);
	k=l2;
	for(l2++;!aflag[l2];l2++);
	l=l2;
	for(l2++;!aflag[l2];l2++);
	int i2=l2;

	u_sprintf(strtmp,"(%S,%S)",tree(l,tree1),tree(i2,tree1));
	t=PhyloNode_Create();
	di=Arbre_Dico_ADD(dic,strtmp);
	di->number=(long int)t;
	u_strcpy(t->name,strtmp);

	di=Arbre_Dico_FOUND(dic,tree(l,tree1));
	t2=(PhyloNode *)di->number;
	t->Fils0=t2;
	t2->weight=0.5*((ad(k,l)+ad(l,i2))-ad(k,i2));
	di=Arbre_Dico_FOUND(dic,tree(i2,tree1));
	t3=(PhyloNode *)di->number;
	t->Fils1=t3;
	t3->weight=0.5*((ad(k,i2)+ad(l,i2))-ad(k,l));

	u_sprintf(strtmp,"(,(%S,%S))",tree(l,tree1),tree(i2,tree1));
	t1=PhyloNode_Create();
	di=Arbre_Dico_ADD(dic,strtmp);
	di->number=(long int)t1;
	u_strcpy(t1->name,strtmp);
	t1->Fils0=t;
	t1->Fils1=0;

	u_sprintf(strtmp,"(%S,(,(%S,%S)))",tree(k,tree1),tree(l,tree1),tree(i2,tree1));
	t=PhyloNode_Create();
	di=Arbre_Dico_ADD(dic,strtmp);
	di->number=(long int)t;
	u_strcpy(t->name,strtmp);
	di=Arbre_Dico_FOUND(dic,tree(k,tree1));
	t3=(PhyloNode *)di->number;
	t->Fils0=t3;
	t3->weight=0.5*((ad(k,l)+ad(k,i2))-ad(l,i2));
	t->Fils1=t1;
	tre->root=t;
	u_strcpy(tree(0,tree1),strtmp);
	u_strcpy(tre->Newick,strtmp);


	free(strtmp);
	free(strtmp1);
	free(strtmp2);
	free(tree1);
	free(ad);
	free(ad1);
	free(ad2);
	free(aflag);
	return tre;
}

void GenererDessin(point_texte* pt,PhyloTree *tre,int xmax,int ymax,int xx,int yy,char *file,int verbose,Infos *infos,char * legend)
{
	//Compute the angles on the edges of the tree
	sortLeaves(pt,tre->root,0,tre->angle);
	if (verbose==1)
		printf("Generation sortLeaves termine.\n");
	//Precompute the drawing
	computeDrawing(tre->root,50000,50000,tre);
	if (verbose==1)
		printf("Generation computeDrawing termine.\n");
	text_buffer *imageSVG=(text_buffer *)malloc(sizeof(text_buffer));
	imageSVG->buffer=(unichar *)malloc(max_buffer);
	imageSVG->size=0;
	imageSVG->size=SVG_Create_Dim(imageSVG->buffer,(unichar *)"TreeCloud / Nuage arbore - http://www.treecloud.org - http://poulphunter1.free.fr/ArbreArbore/",xmax,ymax);
	//Compute the drawing using the width and height computed by the precomputing to scale it properly
	pre_draw(pt,imageSVG,tre->root,xmax,ymax,xx,yy,tre);

	if (strcmp(legend,"OUI") == 0) {
		int y_legend = ymax-60;
		unichar tmp[256];

		u_sprintf(tmp,"Nombre de mots: %d",infos->nombremots);
		imageSVG->size+=SVG_Text2(&imageSVG->buffer[imageSVG->size],5, y_legend,0,0,0,tmp,12);
//		strcpy(message,"Taille de la fenetre");
		u_sprintf(tmp,"Taille de la fenetre: %d\n",infos->taillefenetre);
		imageSVG->size+=SVG_Text2(&imageSVG->buffer[imageSVG->size],5, y_legend+15,0,0,0,tmp,12);
		u_sprintf(tmp,"Type distance: %s",infos->typedistance);
		imageSVG->size+=SVG_Text2(&imageSVG->buffer[imageSVG->size],5, y_legend+30,0,0,0,tmp,12);
	}

	imageSVG->size+=SVG_End(&imageSVG->buffer[imageSVG->size]);
	if (verbose==1)
		printf("Generation dessin terminee.\n");
	Write_File(file,imageSVG);
	free(imageSVG->buffer);
	free(imageSVG);
}


/*Tree construction algorithm proposed by Barthelemy and Luong(1987)
 * Implemented by S Deepak Srinivas
 * (Inter from IIT Ropar,India under Dr. Philippe Gambette)
 */
int called=0;
int num;

unichar labels[200][3500];
unichar connect1[200][3500];
unichar connect2[200][3500];
int NoOfConnect;
int NoOfNodes;

Tree *Tree_Create()
{
	Tree *t=(Tree*)malloc(sizeof(Tree));
	t->num=0;
	t->noofwords=0;


	int i,j;
	for(i=0;i<200;i++)
	{
		t->connect[i]=0;
		u_strcpy(t->words[i],"");
		strcpy(t->newick[i],"");

		for(j=0;j<200;j++)
		{
			t->Adj[i][j]=-1;
			t->length[i][j]=0;
		}
	}

	return t;
}

int In(Tree *tree,int i1,int i2)
{
	int i;
	for(i=0;i<tree->connect[i1];i++)
		if(tree->Adj[i1][i]==i2)
			return i;
	return -1;
}

void addVertex(Tree* tree,unichar* str,int i)
{
	tree->num++;
	u_strcpy(tree->words[i],str);

		int j;
		for(j=0;j<200;j++)
		{
			tree->Adj[i][j]=-1;
			tree->length[i][j]=0;
		}
}

void addEdge(Tree* tree,int id1,int id2,double len)
{
	if(In(tree,id1,id2)<0)
	{
	tree->Adj[id1][tree->connect[id1]]=id2;
	tree->connect[id1]++;
	}
	if(In(tree,id2,id1)<0)
	{
	tree->Adj[id2][tree->connect[id2]]=id1;
	tree->connect[id2]++;
	}

	tree->length[id1][id2]=len;
	tree->length[id2][id1]=len;
}

void Star_Create(Tree *tree,point_texte* pt)
{
	int i;
	addVertex(tree,(unichar*)"",0);
	for(i=1;i<=pt->nbgarde;i++)
		addVertex(tree,pt->mot[i],i);
	for(i=1;i<=pt->nbgarde;i++)
		addEdge(tree,0,i,-1);	//-1 is default length
}

int indexOfStr(char ss[3500])
{
	int i;
	for(i=0;i<NoOfNodes;i++)
		if(!u_strcmp(labels[i],ss))
			return i;
	return -1;
}

//Function to generate the Newick string
void generateNewick(unichar str[200][3500],int *group,int Ng,unichar newick[3500])
{


int i;
u_strcpy(newick,"");

u_strcat(newick,"(");

for(i=0;i<Ng;i++)
{
	if(i<Ng-1)
	{
	u_strcat(newick,str[group[i]+1]);
	u_strcat(newick,",");
	}
	else if(i==Ng-1)
		u_strcat(newick,str[group[i]+1]);
}
u_strcat(newick,")");

}

void deleteSpaces(unichar str[200][3500],int n1)
{

int i,j=1;
unichar str1[200][3500];

for(i=0;i<200;i++)
	u_strcpy(str1[i],"");
for(i=0;i<=n1;i++)
	if(u_strcmp(str[i],"")!=0)
	{
		u_strcpy(str1[j],str[i]);
		j++;
	}
for(i=0;i<200;i++)
	for(j=0;j<3500;j++)
		str[i][j]=str1[i][j];
}

void deleteRows(float dist[200][200],int group[200])
{
float dist1[200][200];
int i,j;
for(i=0;i<200;i++)
	for(j=0;j<200;j++)
		dist1[i][j]=0;

int k=0;
for(i=0;i<200;i++)
{

	if(dist[i][0]!=-1.0)
	{
		for(j=0;j<200;j++)
			dist1[k][j]=dist[i][j];
		k++;
	}
}


for(i=0;i<200;i++)
	for(j=0;j<200;j++)
		dist[i][j]=dist1[i][j];

}

void deleteCols(float dist[200][200],int group[200])
{
float dist1[200][200];
int i,j;
for(i=0;i<200;i++)
	for(j=0;j<200;j++)
		dist1[i][j]=0;

int k=0;
for(i=0;i<200;i++)
{

	if(dist[0][i]!=-1.0)
	{
		for(j=0;j<200;j++)
			dist1[j][k]=dist[j][i];
		k++;
	}
}


for(i=0;i<200;i++)
	for(j=0;j<200;j++)
		dist[i][j]=dist1[i][j];

}





int checkIn(int* arr,int x)
{
	int i;
	for(i=0;i<200;i++)
		if(arr[i]==x)
			return i;
	return -1;
}


char* floatToString(float x)
{
	    char str[30];
	  sprintf(str, "%.4g", x );
	    return str;
}

int call=0;

int indexOf(unichar* string,char ch)
{
	int i;
for(i=u_strlen(string)-1;i>=0;i--)
	if(string[i]==(unichar)ch)
		return i;
return -1;
}

void alterTree(unichar str[200][3500],float dist[200][200])
{
int i,j;

//If the number of words in the tree is 2 (Trivial case)
if(num==2)
	{
		u_strcpy(str[0],"(");
		u_strcat(str[0],str[1]);
		u_strcat(str[0],":");
		u_strcat(str[0],floatToString(dist[1][2]/2));
		u_strcat(str[0],",");
		u_strcat(str[0],str[2]);
		u_strcat(str[0],":");
		u_strcat(str[0],floatToString(dist[1][2]/2));
		u_strcat(str[0],");");
		u_strcpy(connect1[NoOfConnect],str[1]);
		u_strcpy(connect2[NoOfConnect],str[2]);
		NoOfConnect+=1;
		//u_strcpy(connect1[NoOfConnect],str[0]);
		//u_strcpy(connect2[NoOfConnect],str[1]);
		//u_strcpy(connect2[NoOfConnect+1],str[2]);
		//u_strcpy(connect1[NoOfConnect+1],str[0]);
		//NoOfConnect+=2;
		//u_strcpy(labels[NoOfNodes++],str[0]);
		num=2;
		return;
	}

//If the number of words is 3 (Trivial case)
	if(num==3)
		{
			dist[0][1]=(dist[1][2]+dist[1][3]-dist[2][3])/2;
			dist[0][2]=(dist[2][3]+dist[2][1]-dist[1][3])/2;
			dist[0][3]=(dist[3][1]+dist[3][2]-dist[1][2])/2;
			dist[1][0]=dist[0][1];
			dist[2][0]=dist[0][2];
			dist[3][0]=dist[0][3];

			u_strcpy(str[0],"(");
			u_strcat(str[0],str[1]);
			u_strcat(str[0],":");
			u_strcat(str[0],floatToString(dist[0][1]));
			u_strcat(str[0],",");
			u_strcat(str[0],str[2]);
			u_strcat(str[0],":");
			u_strcat(str[0],floatToString(dist[0][2]));
			u_strcat(str[0],",");
			u_strcat(str[0],str[3]);
			u_strcat(str[0],":");
			u_strcat(str[0],floatToString(dist[0][3]));
			u_strcat(str[0],");");

			u_strcpy(connect1[NoOfConnect],str[0]);
			u_strcpy(connect1[NoOfConnect+1],str[0]);
			u_strcpy(connect1[NoOfConnect+2],str[0]);
			u_strcpy(connect2[NoOfConnect],str[1]);
			u_strcpy(connect2[NoOfConnect+1],str[2]);
			u_strcpy(connect2[NoOfConnect+2],str[3]);
			NoOfConnect+=3;
			u_strcpy(labels[NoOfNodes++],str[0]);
			num=0;	//specially for num=3
			return;
		}


	//Building the Score Matrix using the values from the distance matrix to use in later steps of the algorithm
	//The matrix is N*N indexing is from 1.....N
int Score[200][200];

for(i=0;i<200;i++)
			for(j=0;j<200;j++)
				Score[i][j]=0;



		int a,b,x,y;
		for(a=1;a<=num;a++)
			for(b=1;b<=num;b++)
				if(a!=b)
					for(x=1;x<=num;x++)
						for(y=1;y<=num;y++)
							if(((x!=a)&&(x!=b))&&(y!=a)&&(y!=b))
								if(dist[a][b]+dist[x][y]<min(dist[a][x]+dist[b][y],dist[a][y]+dist[b][x]))
									Score[a][b]++;





	//Now, we choose a threshold to use in later steps of algorithm
		double Sm=Score[0][0];
			for(i=1;i<=num;i++)
				for(j=1;j<=num;j++)
					if((i>j)&&(Score[i][j]>Sm))
						Sm=Score[i][j];


			double Smt=(double)(((num-2)*(num-3))/2.0);
			int weak=0;	//A flag for weak threshold.
			double T;	//Threshold
			if(Sm>=Smt-(double)(num-3)/2)
				T=Smt-(double)(num-3)/2.0;
			else
				{
				T=Sm; //T is called weak threshold
				weak=1;
				}


//Next, we need to create the preGroupings using simple depth first search method

	//For that, we first create a graph G as given in the Algorithm of B&L
			//Only here do we use a Tree structure

			Tree *G=Tree_Create();
				G->noofwords=num;
				for(i=0;i<num;i++)
					addVertex(G,str[i+1],i);
				for(i=0;i<G->noofwords;i++)
					for(j=0;j<G->noofwords;j++)
						if((i!=j)&&(Score[i+1][j+1]>=T))
							addEdge(G,i,j,-1);


	//We are using simple depth first search to find the connected components which are the preGroupings
	int visited[200];

	int f;
	for(f=0;f<num;f++)
		visited[f]=0;

	int preGroup[200][200],Ng[200],k=0,l=0;






	//Initialize preGroup 2d array
	for(i=0;i<200;i++)
	{
		Ng[i]=0;
		for(j=0;j<200;j++)
			preGroup[i][j]=-1;
	}


	//Performing simple DFS and finding the preGroupings

	for(i=0;i<G->num;i++)
		if(visited[i]==0)
			{
				visited[i]+=1;
				preGroup[k][l++]=i;
				for(j=0;j<G->connect[i];j++)
				{
					if(visited[G->Adj[i][j]]==0)
					{
						preGroup[k][l]=G->Adj[i][j];
						visited[G->Adj[i][j]]+=1;	//+ve:visited
						l++;
					}
				}
				Ng[k]=l;
				k++;
				l=0;
			}


//Computing the groups in the tree according to the given algorithm

	int Ngg[200],noofgroups=0;
	for(i=0;i<200;i++)
		Ngg[i]=0;
	int Group[200][200];

	for(i=0;i<200;i++)
		for(j=0;j<200;j++)
			Group[i][j]=-1;


	if(weak==1)
	{
		for(i=0;i<200;i++)
			for(j=0;j<200;j++)
				Group[i][j]=preGroup[i][j];
		noofgroups=k;
	}
	else
	{
		double St;
		for(i=0;i<k;i++)
		{
			//Assigning group on checking if S(i,j)>St(P)
			int m,n,o;

			St=((G->num-Ng[i]-2));
			St*=(G->num-Ng[i]-1);
			St-=(G->num-3);
			St/=2.0;

			int flag=1;


				if(Ng[i]==1)
					flag=0;

				for(m=0;m<Ng[i];m++)
					for(n=0;n<Ng[i];n++)
						if((m!=n))
							if((Score[preGroup[i][m]+1][preGroup[i][n]+1]<St))
								flag=0;


			if(flag==1)
			{
				Ngg[noofgroups]=Ng[i];
			for(o=0;o<Ng[i];o++)
				Group[noofgroups][o]=preGroup[i][o];
			noofgroups++;
			}
		}
	}



//This is the major step of the algorithm where we add a new vertex to the graph and group various old leave
//In this step we also find the length between new vertex and the vertices in the group
if(noofgroups==0)
{
int l=0;
	for(i=0;i<200;i++)
	{
		if(Ng[i]>1)
		{
		Ngg[l]=Ng[i];
				for(j=0;j<200;j++)
					Group[l][j]=preGroup[i][j];
		l++;
		}

	}
	noofgroups=l;
}

if((noofgroups==1)&&(Ngg[0]==num))
{

u_strcpy(str[0],"(");
for(i=1;i<=num;i++)
{
	float sum=0.0;
	u_strcat(str[0],str[i]);
	u_strcat(str[0],":");
	for(j=0;j<Ngg[0];j++)
		for(k=0;k<Ngg[0];k++)
			if((i!=j)&&(j!=k)&&(k!=i))
				sum+=(dist[i][j]+dist[i][k]-dist[j][k]);
	sum/=((Ngg[0]-1)*(Ngg[0]-2));
	u_strcat(str[0],floatToString(sum));
	if(i<num)
		u_strcat(str[0],",");
	else if(i==num)
		u_strcat(str[0],");");
}
for(i=1;i<=num;i++)
{
	u_strcpy(connect1[NoOfConnect],str[0]);
	u_strcpy(connect2[NoOfConnect],str[i]);
	NoOfConnect++;
}
num=1;
return;
}


i=0;
int n1=num;
for(i=0;i<noofgroups;i++)
{
	for(j=0;j<Ngg[i];j++)
		if(Group[i][j]!=-1)
		{

			//Finding length of vertices in the group to the new vertex Ns
			int m,n;
			double sum=0;
			for(m=0;m<Ngg[i];m++)	//For each leaf in group
				if(m!=j)			//Other than the present vertex
					for(n=1;n<=num;n++)	//For leaves in graph
						if(checkIn(Group[i],n-1)<0)		//Check if not in group
							//Apply formula
							sum+=(double)(dist[Group[i][j]+1][Group[i][m]+1]+dist[Group[i][j]+1][n]-dist[Group[i][m]+1][n]);

		sum/=(2*(Ngg[i]-1)*(num-Ngg[i]));
			u_strcat(str[Group[i][j]+1],":");
			if(sum<0)
			{
				u_strcat(str[Group[i][j]+1],"0");

			}
			else
			{
				u_strcat(str[Group[i][j]+1],floatToString(sum));
			}
		}

num++;


//update the distance for the new node

for(j=1;j<num;j++)
{
	float sum=0.0;
	if(checkIn(Group[i],j-1)<0)
		for(k=0;k<Ngg[i];k++)
			if(dist[Group[i][k]+1][j]!=-1.0)
				sum+=dist[Group[i][k]+1][j];
	sum/=(Ngg[i]);
	dist[num][j]=sum;
	dist[j][num]=sum;
}



//delete rows and columns (keeping their rows and columns as -1 )

for(j=0;j<Ngg[i];j++)
	for(k=0;k<200;k++)
	{
		dist[Group[i][j]+1][k]=-1.0;
		dist[k][Group[i][j]+1]=-1.0;
	}

}


unichar newick[3500];

for(i=0;i<noofgroups;i++)
{
	generateNewick(str,Group[i],Ngg[i],newick);
	u_strcpy(str[++n1],newick);
for(j=0;j<Ngg[i];j++)
{

	u_strcpy(connect1[NoOfConnect],newick);
	u_strcpy(connect2[NoOfConnect],str[Group[i][j]+1]);
	NoOfConnect++;
	u_strcpy(str[Group[i][j]+1],"");
}
u_strcpy(labels[NoOfNodes++],newick);
}
deleteSpaces(str,n1);

	deleteRows(dist,Group[i]);
	deleteCols(dist,Group[i]);

for(i=0;i<noofgroups;i++)
	num-=Ngg[i];


return;
}

int indexOf1(int n)
{
	int i;
	for(i=0;i<200;i++)
		if(!u_strcmp(labels[i],connect1[n]))
			return i;
	return -1;
}

int indexOf2(int n)
{
	int i;
	for(i=0;i<200;i++)
		if(!u_strcmp(labels[i],connect2[n]))
			return i;

	return -1;
}

void correctConnect2()
{
int i,j;

for(i=0;i<NoOfConnect;i++)
{
	if((indexOf(connect2[i],')')-indexOf(connect2[i],':'))<0)
	{




connect2[i][indexOf(connect2[i],':')]=(unichar)'\0';

		//unichar word[3500];
		//u_strcpy(word,"");
		//u_strncpy(word,connect2[i],indexOf(connect2[i],':'));
		//u_strcpy(connect2[i],"");
		//u_strcpy(connect2[i],word);
	}
}

}



/*
 * The following function is to compute the tree for the distance matrix provided to the function
 * using the tree construction algorithm proposed by Barthelemy and Luong.
 * Requirements: 200 words max, 3500 characters max for the generated Newick string
 *
 * After the construction of the tree, the Newick string for the tree is saved into the file
 * passed to the function.
 *
 * This file is later accessed by the JS file to visualize the tree.
 */


unichar* GenTreeBL(char fichierNewick[256],point_texte *pt,double *distance,int verbose,unichar* Newick,char fichierhtml[256])
{
	/*
	 * This function and the functions called by this function are programmed by S Deepak Srinivas,
	 * (deepaks@iitrpr.ac.in)
	 * Indian Institute of Technology Ropar, an Intern at UPEM-LIGM (with Dr. Philippe Gambette)
	 */

	//connect1 and connect2 has all the edges
	int i,j;
	for(i=0;i<200;i++)
	{
		u_strcpy(connect1[i],"");
		u_strcpy(connect2[i],"");
		u_strcpy(labels[i],"");
	}
	NoOfConnect=0;
	NoOfNodes=0;

		//We have an array of strings which will be passed to the further functions for manipulation
		//An we shall finally have the newick string for the tree constructed (str[0])
		unichar str[200][3500];
		for(i=0;i<200;i++)
		{
			if(i==0)
			u_strcpy(str[i],"");
			else if(i<pt->nbgarde+1)
			u_strcpy(str[i],pt->mot[i]);
			else
				u_strcpy(str[i],"");
			u_strcpy(labels[i],"");
		}
		num=pt->nbgarde;
		NoOfNodes=pt->nbgarde;

		for(i=0;i<pt->nbgarde;i++)
			u_strcpy(labels[i],pt->mot[i+1]);


		//Get distance matrix
		//The distance matrix is extracted from the *distance passed to the function
		//This is done for simplicity and easy understanding of the code

		//distance between ith and jth word (indexing from 1....,1....) is distance((i-1),(j-1))

				float dist[200][200];
					for(i=0;i<200;i++)
						for(j=0;j<200;j++)
							dist[i][j]=0.0;
					for(i=1;i<=pt->nbgarde;i++)
				   	{
				   		dist[i][i]=0.0;
				   		for(j=1;j<=i;j++)
				   		{
				   			dist[i][j]=distance((i-1),(j-1));
				   			dist[j][i]=dist[i][j];
				   		}
				   	}





		//Now the alterTree() is being called repeatedly in a loop to compute the structure of the tree

while(num>=3)
	alterTree(str,dist);
if(num==2)
	alterTree(str,dist);

if(fichierNewick[0]!=0)
{
FILE *pFile;

	pFile=u_fopen(UTF8,fichierNewick,U_WRITE);
	if (pFile==0)
{
		return 0;
}
	u_fwrite(UTF8,str[0],u_strlen(str[0]),pFile);
	u_fclose(pFile);

if(verbose==1)
	printf("The Newick string has been written to the file successfully !!\n");
}
u_strcpy(Newick,str[0]);


if(verbose==1)
printf("\n Computing the visualization of the tree constructed !! \n");

correctConnect2();
	unichar declare[50000];
	u_strcpy(declare,"");
	//num
	u_strcat(declare,"var num=");
	char sss[4];
	sprintf(sss,"%d",pt->nbgarde);
	u_strcat(declare,sss);
	u_strcat(declare,";\n");
	//Labels
	u_strcat(declare,"var labels=[");
	for(i=0;i<NoOfNodes;i++)
	{
		u_strcat(declare,"\"");
		u_strcat(declare,labels[i]);
		u_strcat(declare,"\"");
		if(i<NoOfNodes-1)
			u_strcat(declare,",");
	}
	u_strcat(declare,"];\n");
	//Connect1
	//Use Connect1[i] to display the label of the i-th node

	u_strcat(declare,"var connect1=[");
		for(i=0;i<NoOfConnect;i++)
		{
			char nn[4];
			sprintf(nn,"%d",indexOf1(i));
			if(strcmp(nn,"-1")==0)
				sprintf(nn,"%d",NoOfNodes-1);
			u_strcat(declare,nn);
			if(i<NoOfConnect-1)
				u_strcat(declare,",");
		}
		u_strcat(declare,"];\n");
		//Connect2
			u_strcat(declare,"var connect2=[");
				for(i=0;i<NoOfConnect;i++)
				{
					char nn[4];
					sprintf(nn,"%d",indexOf2(i));
					u_strcat(declare,nn);
					if(i<NoOfConnect-1)
						u_strcat(declare,",");
				}
				u_strcat(declare,"];\n");



//Min font size :12px Max:24px
	u_strcat(declare,"var sizes=[");
	int sizes[pt->nbgarde];
	char size[4];
	int max=0;
	for(i=1;i<=pt->nbgarde;i++)
		if(pt->freq[i]>max)
			max=pt->freq[i];
	int min=10000;
	for(i=1;i<=pt->nbgarde;i++)
		if(pt->freq[i]<min)
			min=pt->freq[i];

	char colors[40000];
	strcpy(colors,"var colors=[");

	for(i=0;i<pt->nbgarde;i++)
	{
		sizes[i]=(10+9.999*(double)(log(pt->freq[i+1])-log(min))/(double)(log(max)-log(min)));
		sprintf(size,"%d",sizes[i]);

		switch(sizes[i])
		{
		case 19:strcat(colors,"\"#FF3300\"");break;
		case 18:strcat(colors,"\"#E76200\"");break;
		case 17:strcat(colors,"\"#E76200\"");break;
		case 16:strcat(colors,"\"#E76200\"");break;
		case 15:strcat(colors,"\"#E76200\"");break;
		case 14:strcat(colors,"\"#E76200\"");break;
		case 13:strcat(colors,"\"#E76200\"");break;
		case 12:strcat(colors,"\"#477B7B\"");break;
		case 11:strcat(colors,"\"#668AA8\"");break;
		case 10:strcat(colors,"\"#B8C9D6\"");break;
		}

		u_strcat(declare,size);

		if(i<pt->nbgarde-1)
		{
		strcat(colors,",");
		u_strcat(declare,",");
		}
	}
	strcat(colors,"];\n");
u_strcat(declare,"];\n");
u_strcat(declare,colors);
char freq[4];

u_strcat(declare,"var freqs=[");

	for(i=1;i<=pt->nbgarde;i++)
	{
		sprintf(freq,"%d",pt->freq[i]);
		u_strcat(declare,freq);
		if(i<pt->nbgarde)
		u_strcat(declare,",");
	}
u_strcat(declare,"];\n");




unichar file[30000];

u_strcpy(file,"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\r\n"
"<html lang=\"en\">\r\n"
"	<head>\r\n"
"		<meta charset=\"utf-8\">\r\n"
"		<title>TreeCloud BandL</title>\r\n"
"\r\n"
"		<style>\r\n"
".d3-tip {\r\n"
"  line-height: 1;\r\n"
"  font-weight: bold;\r\n"
"  padding: 8px;\r\n"
"  background: rgba(0, 0, 0, 0.8);\r\n"
"  color: #fff;\r\n"
"  border-radius: 2px;\r\n"
"}\r\n"
"\r\n"
"/* Creates a small triangle extender for the tooltip */\r\n"
".d3-tip:after {\r\n"
"  box-sizing: border-box;\r\n"
"  display: inline;\r\n"
"  font-size: 10px;\r\n"
"  width: 100%;\r\n"
"  line-height: 1;\r\n"
"  color: rgba(0, 0, 0, 0.8);\r\n"
"  content: \"\\25BC\";\r\n"
"  position: absolute;\r\n"
"  text-align: center;\r\n"
"}\r\n"
"\r\n"
"/* Style northward tooltips differently */\r\n"
".d3-tip.n:after {\r\n"
"  margin: -1px 0 0 0;\r\n"
"  top: 100%;\r\n"
"  left: 0;\r\n"
"}\r\n"
"</style>\r\n"
"	<script type=\"text/javascript\" src=\"d3.v2.js\"></script>\r\n"
"		<script src=\"d3.tip.v0.6.3.js\"></script>\r\n"
"	</head>\r\n"
"	<body>\r\n"
"\r\n"
"<h2>Tree constructed from the text using the algorithm proposed by Barthelemy and Luong (1987)</h2>\r\n"
"<h3><a href=\"Dendro.htm\">Click here to view the Circular Dendrogram and Phylogram Visualization.</a></h3>\r\n"
"\r\n"
"<script type=\"text/javascript\" charset=\"utf-8\">\r\n"
"			var w = 1060, h = 900;\r\n"
"\r\n"
"			var labelDistance = 0;\r\n"
"\r\n"
"			\r\n"
"			var nodes = [];\r\n"
"			var labelAnchors = [];\r\n"
"			var labelAnchorLinks = [];\r\n"
"			var tip = d3.tip()\r\n"
"  .attr(\'class\', \'d3-tip\')\r\n"
"  .offset([-10, 0])\r\n"
"  .html(function(d) {\r\n"
"    return \"<strong>Occurences:</strong><br>\"+tipArray[labels.indexOf(d.node.label)];})\r\n"
"\r\n"
"var vis = d3.select(\"body\").append(\"svg:svg\").attr(\"width\", w).attr(\"height\", h);\r\n"
"vis.call(tip);\r\n"
"			var links = [];");

u_strcat(file,declare);

u_strcat(file,"var tipArray=[];\r\n"
"\r\n"
"d3.text(\"text01.txt\",function(fileText){\r\n"
"var res = fileText.split(\" \");\r\n"
"\r\n"
"for(var i=0;i<num;i++)\r\n"
"{\r\n"
"var str=\"<span style=\'color:steelBlue\'>... \";\r\n"
"var occ=[];\r\n"
"for(var j=0;j<res.length;j++)\r\n"
"{\r\n"
"if((res[j].toLowerCase().replace(/[\\.,-\\/#!$12\\[34\\n567\\]8\\\'9\\\"0%\\^&\\*;:{}=\\-_`~()]/g,\"\").replace(/\\s{2,}/g,\" \")==labels[i]))\r\n"
"{\r\n"
"occ.push(j);\r\n"
"}\r\n"
"}\r\n"
"\r\n"
"\r\n"
"for(var j=0;j<occ.length;j++)\r\n"
"{\r\n"
"if(res[occ[j]-5])\r\n"
"str=str+res[occ[j]-5]+\" \";\r\n"
"if(res[occ[j]-4])\r\n"
"str=str+res[occ[j]-4]+\" \";\r\n"
"if(res[occ[j]-3])\r\n"
"str=str+res[occ[j]-3]+\" \";\r\n"
"if(res[occ[j]-2])\r\n"
"str=str+res[occ[j]-2]+\" \";\r\n"
"if(res[occ[j]-1])\r\n"
"str=str+res[occ[j]-1]+\" \";\r\n"
"\r\n"
"str=str+\"</span><span style=\'color:red\'>\";\r\n"
"str=str+res[occ[j]]+\" \";\r\n"
"str=str+\"</span><span style=\'color:steelBlue\'>\";\r\n"
"if(res[occ[j]+1])\r\n"
"str=str+res[occ[j]+1]+\" \";\r\n"
"if(res[occ[j]+2])\r\n"
"str=str+res[occ[j]+2]+\" \";\r\n"
"if(res[occ[j]+3])\r\n"
"str=str+res[occ[j]+3]+\" \";\r\n"
"if(res[occ[j]+4])\r\n"
"str=str+res[occ[j]+4]+\" \";\r\n"
"if(res[occ[j]+5])\r\n"
"str=str+res[occ[j]+5]+\" \";\r\n"
"str=str+\"...\";\r\n"
"if(j<occ.length-1)\r\n"
"str=str+\"<br>...\"\r\n"
"};\r\n"
"str=str+\"</span>\";\r\n"
"tipArray.push(str);\r\n"
"};\r\n"
"\r\n"
"\r\n"
"});\r\n"
"\r\n"
"\r\n"
"			for(var i = 0; i < labels.length; i++) {\r\n"
"				if(i<num)\r\n"
"				var node = {\r\n"
"					label : labels[i]\r\n"
"				};\r\n"
"				else\r\n"
"					var node={label : \"\"};\r\n"
"				nodes.push(node);\r\n"
"				labelAnchors.push({\r\n"
"					node : node\r\n"
"				});\r\n"
"				labelAnchors.push({\r\n"
"					node : node\r\n"
"				});\r\n"
"			};\r\n"
"\r\n"
"			for(var i = 0; i < connect1.length; i++) {\r\n"
"				\r\n"
"					\r\n"
"						links.push({\r\n"
"							source : connect1[i],\r\n"
"							target : connect2[i],\r\n"
"							weight : 0.1\r\n"
"						});\r\n"
"				}\r\n"
"			for(var i = 0; i < nodes.length; i++) {\r\n"
"				labelAnchorLinks.push({\r\n"
"					source : i * 2,\r\n"
"					target : i * 2 + 1,\r\n"
"					weight : 1\r\n"
"				});\r\n"
"			};\r\n"
"\r\n"
"			var force = d3.layout.force().size([w, h]).nodes(nodes).links(links).gravity(1).linkDistance(60).charge(-3000).linkStrength(function(x) {\r\n"
"				return x.weight * 10\r\n"
"			});\r\n"
"\r\n"
"\r\n"
"\r\n"
"			force.start();\r\n"
"\r\n"
"			var force2 = d3.layout.force().nodes(labelAnchors).links(labelAnchorLinks).gravity(0).linkDistance(0).linkStrength(8).charge(-100).size([w, h]);\r\n"
"			force2.start();\r\n"
"\r\n"
"			var link = vis.selectAll(\"line.link\").data(links).enter().append(\"svg:line\").attr(\"class\", \"link\").style(\"stroke\", \"#CCC\");\r\n"
"\r\n"
"			var node = vis.selectAll(\"g.node\").data(force.nodes()).enter().append(\"svg:g\").attr(\"class\", \"node\");\r\n"
"			node.append(\"svg:circle\").attr(\"r\", function(n){if(n.label!=\"\")\r\n"
"				return 5;else \r\n"
"				return 3;})\r\n"
"			.style(\"fill\", \"#555\").style(\"stroke\", \"#FFF\").style(\"stroke-width\", 3);\r\n"
"			node.call(force.drag);\r\n"
"\r\n"
"\r\n"
"			var anchorLink = vis.selectAll(\"line.anchorLink\").data(labelAnchorLinks)//.enter().append(\"svg:line\").attr(\"class\", \"anchorLink\").style(\"stroke\", \"#999\");\r\n"
"\r\n"
"			var anchorNode = vis.selectAll(\"g.anchorNode\").data(force2.nodes()).enter().append(\"svg:g\").attr(\"class\", \"anchorNode\");\r\n"
"			anchorNode.append(\"svg:circle\").attr(\"r\", 0).style(\"fill\", \"#FFF\");\r\n"
"				anchorNode.append(\"svg:text\").text(function(d, i) {\r\n"
"				return i % 2 == 0 ? \"\" : d.node.label\r\n"
"			}).style(\"fill\", function(d){return colors[labels.indexOf(d.node.label)];}).style(\"font-family\", \"Arial\").style(\"font-size\", function(d){return sizes[labels.indexOf(d.node.label)];})\r\n"
"				.on(\'mouseover\', tip.show)\r\n"
"      .on(\'mouseout\', tip.hide);\r\n"
"\r\n"
"			var updateLink = function() {\r\n"
"				this.attr(\"x1\", function(d) {\r\n"
"					return d.source.x;\r\n"
"				}).attr(\"y1\", function(d) {\r\n"
"					return d.source.y;\r\n"
"				}).attr(\"x2\", function(d) {\r\n"
"					return d.target.x;\r\n"
"				}).attr(\"y2\", function(d) {\r\n"
"					return d.target.y;\r\n"
"				});\r\n"
"\r\n"
"			}\r\n"
"\r\n"
"			var updateNode = function() {\r\n"
"				this.attr(\"transform\", function(d) {\r\n"
"					return \"translate(\" + d.x + \",\" + d.y + \")\";\r\n"
"				});\r\n"
"\r\n"
"			}\r\n"
"\r\n"
"\r\n"
"			force.on(\"tick\", function() {\r\n"
"\r\n"
"				force2.start();\r\n"
"\r\n"
"				node.call(updateNode);\r\n"
"\r\n"
"				anchorNode.each(function(d, i) {\r\n"
"					if(i % 2 == 0) {\r\n"
"						d.x = d.node.x;\r\n"
"						d.y = d.node.y;\r\n"
"					} else {\r\n"
"						var b = this.childNodes[1].getBBox();\r\n"
"\r\n"
"						var diffX = d.x - d.node.x;\r\n"
"						var diffY = d.y - d.node.y;\r\n"
"\r\n"
"						var dist = Math.sqrt(diffX * diffX + diffY * diffY);\r\n"
"\r\n"
"						var shiftX = b.width * (diffX - dist) / (dist * 2);\r\n"
"						shiftX = Math.max(-b.width, Math.min(0, shiftX));\r\n"
"						var shiftY = 5;\r\n"
"						this.childNodes[1].setAttribute(\"transform\", \"translate(\" + shiftX + \",\" + shiftY + \")\");\r\n"
"					}\r\n"
"				});\r\n"
"\r\n"
"\r\n"
"				anchorNode.call(updateNode);\r\n"
"\r\n"
"				link.call(updateLink);\r\n"
"				anchorLink.call(updateLink);\r\n"
"\r\n"
"			});\r\n"
"\r\n"
"		</script>\r\n"
"<h2>The Implementation of the algorithm has been programmed in C and the visualization in JavaScript (with extensize use of the d3.js library)  by S Deepak Srinivas</h2>\r\n"
"	</body>\r\n"
"</html>");

return file;
}


