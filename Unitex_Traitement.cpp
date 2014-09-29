/*
 * Unitex
 *
 * Copyright (C) 2001-2011 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */

/*
 * File created and contributed by Gilles Vollant (Ergonotics SAS) 
 * as part of an UNITEX optimization and reliability effort
 *
 * additional information: http://www.ergonotics.com/unitex-contribution/
 * contact : unitex-contribution@ergonotics.com
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "UnitexTool.h"
#include "UnitexLibIO.h"
#include "PersistenceInterface.h"
#include "SyncTool.h"
#include "AbstractFilePlugCallback.h"
#include "unicode_flex.h"

#define _NOT_UNDER_WINDOWS
#define DEBUG
#define DEBUG2

#ifdef HAS_UNITEX_NAMESPACE
using namespace unitex;
#endif

#ifdef _NOT_UNDER_WINDOWS
   #define UNITEX_PATH_SEPARATOR_CHAR '/'
   #define UNITEX_PATH_SEPARATOR_STRING "/"
#else
   #define UNITEX_PATH_SEPARATOR_CHAR '\\'
   #define UNITEX_PATH_SEPARATOR_STRING "\\"
#endif

int is_suffixe(const char *s,const char *r) {
  int i,k,l_s,l_r;

  l_s = strlen(s);  l_r = strlen(r);
  i = l_s - l_r;

    k = 0;
    while ( k < l_r && s[i] == r[k]) {
      i++; k++;
    }
    if ( k == l_r) return 1;
    else return 0;
}

const char* getVirtualFilePfx()
{
	if (UnitexAbstractPathExists("*") != 0)
		return "*";

	if (UnitexAbstractPathExists("$:") != 0)
		return "$:";

	return "";
}


char* combineUnitexFileComponent(const char* resource_folder,const char* filename)
{
	if (resource_folder == NULL)
		resource_folder = "";
	if (filename == NULL)
		filename = "";

	size_t len_resource = strlen(resource_folder);
	size_t len_filename = strlen(filename);

	char* buffer = (char*)malloc(len_resource+len_filename+4);

	int must_add_separator = 0;
	if (len_resource > 0)
	{
		char last_folder_char = * (resource_folder + len_resource - 1);
		must_add_separator = ((last_folder_char == '\\') || (last_folder_char == '/')) ? 0 : 1;
	}
	
	memcpy(buffer,resource_folder,len_resource);
	if (must_add_separator != 0)
		*(buffer + len_resource) = UNITEX_PATH_SEPARATOR_CHAR;
	memcpy(buffer + len_resource + must_add_separator,filename,len_filename);
	*(buffer + len_resource + must_add_separator + len_filename) = '\0';
	return buffer;
}

int processUnitexWork(
	int verbose,
	const char* langue,
	const char* anti_perso,
	const char* alphabetName,
	const char* normFullPath,
	const char* dictionaryNormalFullName,
	const char* dictionaryMorphoFullName,
	const char* dictionaryAntiPersoFullName,
	const char* graphFullFileName,
	const char* graphFullFileName2,
	const char* corpusFullFileName,
	const char* corpusAnnotedFullFileName,
	const char* virtualCorpusTextFileNameTxt,
	const char* virtualAnnotedCorpusTextFileNameTxt1,
	const char* virtualAnnotedCorpusTextFileNameTxt2,
	const char* virtualCorpusTextFileNameSnt,
	const char* virtualCorpusTextFileNameSntDir,
	const char* virtualCorpusTextFileNameConcordInd,
	const char* virtualCorpusTextFileNameSnt2,
	const char* virtualCorpusTextFileNameSntDir2,
	const char* virtualCorpusTextFileNameConcordInd2,
	const char* inputCorpusEncoding,
	const char* outputCorpusEncoding
	)
{
	#ifdef DEBUG
	if (verbose) {
	printf("\ncorpusFullFileName:%s\n\n",corpusFullFileName);
	printf("\ndictionaryFullName:%s\n\n",dictionaryNormalFullName);
	printf("\ndictionaryFullName:%s\n\n",dictionaryMorphoFullName);
	printf("\ngraphFullFileName2:%s\n\n",graphFullFileName2);
	printf("\nvirtualAnnotedCorpusTextFileNameTxt2:%s\n\n",virtualAnnotedCorpusTextFileNameTxt2);
	printf("\ncorpusAnnotedFullFileName:%s\n\n",corpusAnnotedFullFileName);
	}
	#endif

	CreateUnitexFolder(virtualCorpusTextFileNameSntDir);
	CreateUnitexFolder(virtualCorpusTextFileNameSntDir2);

	CopyUnitexFile(corpusFullFileName,virtualCorpusTextFileNameTxt);

	const char *Normalize_Argv[] = {"UnitexTool","Normalize",virtualCorpusTextFileNameTxt,"-r",normFullPath,"-k",inputCorpusEncoding};
	int nb_Normalize_Argv = 7;
	int result_Normalize = UnitexTool_public_run(nb_Normalize_Argv,(char**)Normalize_Argv,NULL,NULL);

	const char *Tokenize_Argv[] = {"UnitexTool","Tokenize",virtualCorpusTextFileNameSnt,"-a",alphabetName};
	int nb_Tokenize_Argv = 5;
	int result_Tokenize = UnitexTool_public_run(nb_Tokenize_Argv,(char**)Tokenize_Argv,NULL,NULL);
	

	if ((strcmp(anti_perso,"OUI")== 0) && (strcmp(langue,"NONE") != 0) ) {

		if (verbose) printf("DEUX DICOS dictionaryAntiPersoFullName:%s\n\n",dictionaryAntiPersoFullName);

		const char *Dico_Argv[] = {"UnitexTool","Dico","-t",virtualCorpusTextFileNameSnt,"-a",alphabetName,dictionaryNormalFullName,dictionaryAntiPersoFullName};
		int nb_Dico_Argv = 8;
		int result_Dico = UnitexTool_public_run(nb_Dico_Argv,(char**)Dico_Argv,NULL,NULL);
	}
	else if ((strcmp(anti_perso,"NON")== 0) && (strcmp(langue,"NONE") != 0) ) {

		if (verbose) printf("DICO SEUL  dictionaryNormalFullName:%s<br>\n\n",dictionaryNormalFullName);

		const char *Dico_Argv[] = {"UnitexTool","Dico","-t",virtualCorpusTextFileNameSnt,"-a",alphabetName,dictionaryNormalFullName};
		int nb_Dico_Argv = 7;
		int result_Dico = UnitexTool_public_run(nb_Dico_Argv,(char**)Dico_Argv,NULL,NULL);
	}
	else if ((strcmp(anti_perso,"OUI")== 0) && (strcmp(langue,"NONE")==0) ) {

		if (verbose) printf("ANTIDICO SEUL  dictionaryAntiPersoFullName:%s<br>\n\n",dictionaryAntiPersoFullName);

		const char *Dico_Argv[] = {"UnitexTool","Dico","-t",virtualCorpusTextFileNameSnt,"-a",alphabetName,dictionaryAntiPersoFullName};
		int nb_Dico_Argv = 7;
		int result_Dico = UnitexTool_public_run(nb_Dico_Argv,(char**)Dico_Argv,NULL,NULL);
	}
	int result_Locate;
	if ((strcmp(langue,"NONE") !=0) ) {
	const char *Locate_Argv[] = {"UnitexTool","Locate","-t",virtualCorpusTextFileNameSnt,"-a",alphabetName,"-L","-R","--all","-m",dictionaryMorphoFullName,"-b","-Y",graphFullFileName,"-q",outputCorpusEncoding};
	int nb_Locate_Argv = 16;
	result_Locate = UnitexTool_public_run(nb_Locate_Argv,(char**)Locate_Argv,NULL,NULL);
	}
	else {
		const char *Locate_Argv[] = {"UnitexTool","Locate","-t",virtualCorpusTextFileNameSnt,"-a",alphabetName,"-L","-R","--all","-b","-Y",graphFullFileName,"-q",outputCorpusEncoding};
		int nb_Locate_Argv = 14;
		result_Locate = UnitexTool_public_run(nb_Locate_Argv,(char**)Locate_Argv,NULL,NULL);
	}

	const char *Concord_Argv[] = {"UnitexTool","Concord",virtualCorpusTextFileNameConcordInd,"-m",virtualAnnotedCorpusTextFileNameTxt1,"-q",outputCorpusEncoding};
	int nb_Concord_Argv = 7;
	int result_Concord = UnitexTool_public_run(nb_Concord_Argv,(char**)Concord_Argv,NULL,NULL);

	const char *Normalize_Argv2[] = {"UnitexTool","Normalize",virtualAnnotedCorpusTextFileNameTxt1,"-r",normFullPath,"-k",inputCorpusEncoding};
	int nb_Normalize_Argv2 = 7;
	int result_Normalize2 = UnitexTool_public_run(nb_Normalize_Argv2,(char**)Normalize_Argv2,NULL,NULL);

	const char *Tokenize_Argv2[] = {"UnitexTool","Tokenize",virtualCorpusTextFileNameSnt2,"-a",alphabetName};
	int nb_Tokenize_Argv2 = 5;
	int result_Tokenize2 = UnitexTool_public_run(nb_Tokenize_Argv2,(char**)Tokenize_Argv2,NULL,NULL);

	const char *Locate_Argv2[] = {"UnitexTool","Locate","-t",virtualCorpusTextFileNameSnt2,"-a",alphabetName,"-L","-R","--all","-b","-Y",graphFullFileName2,"-q",outputCorpusEncoding};
	int nb_Locate_Argv2 = 14;
	int result_Locate2 = UnitexTool_public_run(nb_Locate_Argv2,(char**)Locate_Argv2,NULL,NULL);

	const char *Concord_Argv2[] = {"UnitexTool","Concord",virtualCorpusTextFileNameConcordInd2,"-m",virtualAnnotedCorpusTextFileNameTxt2,"-q",outputCorpusEncoding};
	int nb_Concord_Argv2 = 7;
	int result_Concord2 = UnitexTool_public_run(nb_Concord_Argv2,(char**)Concord_Argv2,NULL,NULL);

	CopyUnitexFile(virtualAnnotedCorpusTextFileNameTxt2,corpusAnnotedFullFileName);

	if ((result_Normalize != 0) || (result_Tokenize != 0) ||  (result_Locate != 0) || (result_Concord != 0)) return 0;

	return 1;
}

void language_to_resource (const char * langue, char *dico_normal, char *dico_morpho, char *unitex_sub_workdir,char* main_graph, char* main_graph2) {
if ((strcmp(langue,"FR")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_FR.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_FR.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/French/src");
	strcpy(main_graph,"Treecloud_FR.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"GR")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_GR.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_GR.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/Greek/src");
	strcpy(main_graph,"Treecloud_GR.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"ES")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_ES.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_ES.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/Spanish/src");
	strcpy(main_graph,"Treecloud_ES.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"EN")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_EN.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_EN.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/English/src");
	strcpy(main_graph,"Treecloud_EN.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"PT")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_PT.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_PT.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/Portuguese/src");
	strcpy(main_graph,"Treecloud_PT.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"IT")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_IT.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_IT.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/Italian/src");
	strcpy(main_graph,"Treecloud_IT.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"SR_LAT")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_SR_LAT.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_SR_LAT.bin");
	strcpy(main_graph,"Treecloud_SR_LAT.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"SR_CYR")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_SR_CYR.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_SR_CYR.bin");
	strcpy(main_graph,"Treecloud_SR_CYR.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"DE")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_DE.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_DE.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/German/src");
	strcpy(main_graph,"Treecloud_DE.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
else if ((strcmp(langue,"NONE")==0)) {
	strcpy(dico_normal,"ALL_NORMAL_DICS_FR.bin");
	strcpy(dico_morpho,"ALL_MORPHO_DICS_FR.bin");
	strcpy(unitex_sub_workdir,"TreeCloud_WS/French/src");
	strcpy(main_graph,"Treecloud_FR.fst2");
	strcpy(main_graph2,"NC_Underscore.fst2");
}
}

void convert_to_DELA( char * dico_DELA,char *dico_liste) {
FILE *dest, *src;
unichar line[256];
int c;
src = u_fopen(UTF8,dico_liste,U_READ);
dest = u_fopen(UTF8,dico_DELA,U_WRITE);

while( ( c = u_fgets(UTF8,line,src) ) != EOF ) {
	u_fprintf(UTF8,dest,"%S,.ANTIPERSO\n",line);
}
u_fclose(dest);
u_fclose(src);
}



int	traitement_unitex(char *fichierentree,char *tmp_annoted_fle,int verbose ,char* virtuel, char * langue,  char* anti_perso, char* workdir, char* anti_dico_perso, int persist) {
char normal_dic[256];
char morpho_dic[256];
char unitex_sub_workdir[256];
char unitex_workdir[256];
char tmp_fle[256];
char main_graph[256];
char main_graph2[256];

//RemoveUnitexFolder("/home/projets/treecloud/tree_03/WORK/CorpusFolder");


	language_to_resource(langue,normal_dic,morpho_dic,unitex_sub_workdir,main_graph,main_graph2);

	sprintf(tmp_annoted_fle,"%s/%s",workdir,"tmp_annote.txt");

	if (verbose) printf("tmp_annoted_fle:%s\n",tmp_annoted_fle);


    SetStdWriteCB(stdwrite_kind_out, 1-verbose, NULL,NULL);
	SetStdWriteCB(stdwrite_kind_err, 1-verbose, NULL,NULL);

	const char* pfx_prefix;
	if ((strcmp(virtuel,"OUI")== 0))
	pfx_prefix = getVirtualFilePfx();
	else  pfx_prefix = "";

	#ifdef DEBUG
	if (verbose) {
	printf("pfx_prefix:%s\n",pfx_prefix);
	printf("anti_perso:%s\n",anti_perso);
	}
	#endif

	char anti_dico_perso_dic[256]="";
	char anti_dico_perso_bin[256]="";

	if ((strcmp(anti_perso,"OUI")== 0)) {

		size_t ll = strlen(anti_dico_perso);
		strcpy(anti_dico_perso_dic,anti_dico_perso);
		anti_dico_perso_dic[ll-4]='\0';
        strcat(anti_dico_perso_dic,(char *) ".dic");

        convert_to_DELA(anti_dico_perso_dic,anti_dico_perso);

    	if (verbose) {
    		printf("anti_dico_perso:%s\n",anti_dico_perso);
    		printf("anti_dico_perso_dic:%s\n",anti_dico_perso_dic);
    	}

		const char *Compress_Argv[] = {"UnitexTool","Compress","--v2",anti_dico_perso_dic};
		int nb_Compress_Argv = 4;
		int result_Compress = UnitexTool_public_run(nb_Compress_Argv,(char**)Compress_Argv,NULL,NULL);
		ll = strlen(anti_dico_perso_dic);
		strcpy(anti_dico_perso_bin,anti_dico_perso_dic);
		anti_dico_perso_bin[ll-4]='\0';
        strcat(anti_dico_perso_bin,(char *) ".bin");
	}

	#ifdef DEBUG
	if (verbose) {
	printf("verbose:%d\n",verbose);
	printf("workdir:%s\n",workdir);
	printf("unitex_sub_workdir:%s\n",unitex_sub_workdir);
	printf("normal_dic:%s\n",normal_dic);
	printf("morpho_dic:%s\n",morpho_dic);
	printf("anti_dico_perso_dic:%s\n",anti_dico_perso_dic);
	printf("main_graph:%s\n",main_graph);
	printf("main_graph2:%s\n",main_graph2);
	}
	#endif

	char* ressourceDir = combineUnitexFileComponent(workdir,unitex_sub_workdir);

	char* graphResDir = combineUnitexFileComponent(ressourceDir, "Graphs");
	char* dictionnaryResDir = combineUnitexFileComponent(ressourceDir, "Dela");
	char* othersResDir = combineUnitexFileComponent(ressourceDir, "");
	char* corpusResDir = combineUnitexFileComponent(ressourceDir, "Corpus");

	char* AlphabetName = combineUnitexFileComponent(othersResDir,"Alphabet.txt");
	char* normFullPath = combineUnitexFileComponent(othersResDir,"Norm.txt");
	char* dictionnaryNormalName = combineUnitexFileComponent(dictionnaryResDir, normal_dic);
	char* dictionnaryMorphoName = combineUnitexFileComponent(dictionnaryResDir, morpho_dic);
	char* graphName = combineUnitexFileComponent(graphResDir, main_graph);
	char* graphName2 = combineUnitexFileComponent(graphResDir, main_graph2);
	char* corpus_filename = fichierentree;
	
	char AlphabetNamePersisted[0x200];
	char dictionnaryNormalNamePersisted[0x200];
	char dictionnaryMorphoNamePersisted[0x200];

	char dictionnaryAntiPersoNamePersisted[0x200];

	char graphNamePersisted[0x200];
	char graphNamePersisted2[0x200];
	char normInVfs[0x200];
    
    
	#ifdef DEBUG
	if (verbose) {
	printf("graphResDir:%s\n",graphResDir);
	printf("dictionnaryResDir:%s\n",dictionnaryResDir);
	printf("othersResDir:%s\n",othersResDir);
	printf("corpusResDir:%s\n",corpusResDir);
	printf("AlphabetName:%s\n",AlphabetName);
	printf("normFullPath:%s\n",normFullPath);
	printf("dictionnaryNormalName:%s\n",dictionnaryNormalName);
	printf("dictionnaryMorphoName:%s\n",dictionnaryMorphoName);
	printf("graphName:%s\n",graphName);
	printf("graphName2:%s\n",graphName2);
	printf("corpus_filename:%s\n",corpus_filename);
	}
	#endif

	if (persist == 1)
	{
		if (verbose) printf("AVEC PERSISTANCE\n");

		persistence_public_load_alphabet(AlphabetName,AlphabetNamePersisted,sizeof(AlphabetNamePersisted)-1);
		persistence_public_load_dictionary(dictionnaryNormalName,dictionnaryNormalNamePersisted,sizeof(dictionnaryNormalNamePersisted)-1);
		persistence_public_load_dictionary(dictionnaryMorphoName,dictionnaryMorphoNamePersisted,sizeof(dictionnaryMorphoNamePersisted)-1);

		persistence_public_load_dictionary(anti_dico_perso_bin,dictionnaryAntiPersoNamePersisted,sizeof(dictionnaryAntiPersoNamePersisted)-1);

		persistence_public_load_fst2(graphName,graphNamePersisted,sizeof(graphNamePersisted)-1);
		persistence_public_load_fst2(graphName2,graphNamePersisted2,sizeof(graphNamePersisted2)-1);
//		sprintf(normInVfs,"%sresInVfs%cNorm.txt",pfx_prefix,UNITEX_PATH_SEPARATOR_CHAR);
//		CopyUnitexFile(normFullPath,normInVfs);
		strcpy(normInVfs,normFullPath);
	}
	else
	{
		if (verbose) printf("SANS PERSISTANCE\n");

		strcpy(AlphabetNamePersisted,AlphabetName);
		strcpy(dictionnaryNormalNamePersisted,dictionnaryNormalName);
		strcpy(dictionnaryMorphoNamePersisted,dictionnaryMorphoName);

		strcpy(dictionnaryAntiPersoNamePersisted,anti_dico_perso_bin);

		strcpy(graphNamePersisted,graphName);
		strcpy(graphNamePersisted2,graphName2);
		strcpy(normInVfs,normFullPath);
	}
	
	#ifdef DEBUG
	if (verbose) {
	printf("AlphabetNamePersisted:%s\n",AlphabetNamePersisted);
	printf("dictionnaryNormalNamePersisted:%s\n",dictionnaryNormalNamePersisted);
	printf("dictionnaryMorphoNamePersisted:%s\n",dictionnaryMorphoNamePersisted);
	printf("dictionnaryAntiPersoNamePersisted:%s\n",dictionnaryAntiPersoNamePersisted);
	printf("graphNamePersisted:%s\n",graphNamePersisted);
	printf("graphNamePersisted2:%s\n",graphNamePersisted2);
	printf("normInVfs:%s\n",normInVfs);
	}
	#endif

	char corpusFolder[256];

	sprintf(corpusFolder,"%s%sCorpusFolder",pfx_prefix,workdir);
//	sprintf(corpusFolder,"%s%s/CorpusFolder",pfx_prefix,workdir);
	CreateUnitexFolder(corpusFolder);
	#ifdef DEBUG
	if (verbose) {
	printf("corpusFolder:%s\n",corpusFolder);
	}
	#endif

	//hTimeElapsed beginAll = SyncBuidTimeMarkerObject();

	char* virtualCorpusTextFileNameTxt = combineUnitexFileComponent(corpusFolder,"corpus.txt") ;
	char* virtualAnnotedCorpusTextFileNameTxt1 = combineUnitexFileComponent(corpusFolder,"corpusAnnoted1.txt") ;
	char* virtualAnnotedCorpusTextFileNameTxt2 = combineUnitexFileComponent(corpusFolder,"corpusAnnoted2.txt") ;
	char* virtualCorpusTextFileNameSnt = combineUnitexFileComponent(corpusFolder,"corpus.snt") ;
	char* virtualCorpusTextFileNameSntDir = combineUnitexFileComponent(corpusFolder,"corpus_snt") ;
	char* virtualCorpusTextFileNameSnt2 = combineUnitexFileComponent(corpusFolder,"corpusAnnoted1.snt") ;
	char* virtualCorpusTextFileNameSntDir2 = combineUnitexFileComponent(corpusFolder,"corpusAnnoted1_snt") ;
	char* virtualCorpusTextFileNameConcordInd = combineUnitexFileComponent(virtualCorpusTextFileNameSntDir,"concord.ind") ;
	char* virtualCorpusTextFileNameConcordInd2 = combineUnitexFileComponent(virtualCorpusTextFileNameSntDir2,"concord.ind") ;

	#ifdef DEBUG
	if (verbose) {
	printf("virtualCorpusTextFileNameTxt:%s\n",virtualCorpusTextFileNameTxt);
	printf("virtualAnnotedCorpusTextFileNameTxt1:%s\n",virtualAnnotedCorpusTextFileNameTxt1);
	printf("virtualAnnotedCorpusTextFileNameTxt2:%s\n",virtualAnnotedCorpusTextFileNameTxt2);
	printf("virtualCorpusTextFileNameSnt:%s\n",virtualCorpusTextFileNameSnt);
	printf("virtualCorpusTextFileNameSntDir:%s\n",virtualCorpusTextFileNameSntDir);
	printf("virtualCorpusTextFileNameConcordInd:%s\n",virtualCorpusTextFileNameConcordInd);
	printf("virtualCorpusTextFileNameSnt2:%s\n",virtualCorpusTextFileNameSnt2);
	printf("virtualCorpusTextFileNameSntDir2:%s\n",virtualCorpusTextFileNameSntDir2);
	printf("virtualCorpusTextFileNameConcordInd2:%s\n",virtualCorpusTextFileNameConcordInd2);
	}
	#endif

                 processUnitexWork(
                	verbose,
                	langue,
                	anti_perso,
                	AlphabetNamePersisted,
                	normInVfs,
                	dictionnaryNormalNamePersisted,
                	dictionnaryMorphoNamePersisted,
                	dictionnaryAntiPersoNamePersisted,
                	graphNamePersisted,
                	graphNamePersisted2,
                	corpus_filename,
                	tmp_annoted_fle,
                	virtualCorpusTextFileNameTxt,
                	virtualAnnotedCorpusTextFileNameTxt1,
                	virtualAnnotedCorpusTextFileNameTxt2,
                	virtualCorpusTextFileNameSnt,
                	virtualCorpusTextFileNameSntDir,
                	virtualCorpusTextFileNameConcordInd,
                	virtualCorpusTextFileNameSnt2,
                	virtualCorpusTextFileNameSntDir2,
                	virtualCorpusTextFileNameConcordInd2,
                 	"utf8-no-bom",
                	"utf8-no-bom"
                	);

	free(virtualCorpusTextFileNameTxt);
	free(virtualAnnotedCorpusTextFileNameTxt1);
	free(virtualCorpusTextFileNameSnt);
	free(virtualCorpusTextFileNameSntDir);
	free(virtualCorpusTextFileNameConcordInd);

	//unsigned int allTimeMsec = SyncGetMSecElapsed(beginAll);

	/* work finish, cleanup */
	//printf("all time %d msec \n",allTimeMsec);

	if (persist)
	{
		//persistence_public_unload_dictionary(AlphabetNamePersisted);
		//persistence_public_unload_dictionary(dictionnaryNamePersisted);
		//persistence_public_unload_fst2(graphNamePersisted);
		//RemoveUnitexFile(normInVfs);
	}
	free(ressourceDir);
	free(graphResDir);
	free(dictionnaryResDir);
	free(othersResDir);
	free(corpusResDir);
	free(AlphabetName);
	free(dictionnaryNormalName);
	free(dictionnaryMorphoName);
	free(graphName);
	free(graphName2);
	free(normFullPath);
	RemoveUnitexFolder(corpusFolder);
	return 0;
}




