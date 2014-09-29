/*
 * unicode_flex.h
 *
 *  Created on: 20 juil. 2011
 *      Author: claude
 */

#ifndef UNICODE_FLEX_H_
#define UNICODE_FLEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * These are the binary open modes for unicode text files.
 */
#define U_READ "rb"
#define U_WRITE "wb"
#define U_APPEND "ab"
#define U_MODIFY "r+b"


/* This caracter is used as the first one of a unicode text file... */
#define U_BYTE_ORDER_MARK 0xFEFF

/* ...and this one is used by contrast with 0xFEFF to determine the byte order */
#define U_NOT_A_CHAR 0xFFFE

/* Markers to indicate if a text file is a UTF16 one or not. */
#define FILE_DOES_NOT_EXIST 0
#define NOT_A_UTF16_FILE 1
#define UTF16_LITTLE_ENDIAN_FILE 2
#define UTF16_BIG_ENDIAN_FILE 3


typedef unsigned short int unichar;



/**
 * We define here the unicode NULL character and the unicode
 * empty string.
 */
extern unichar U_NULL;
extern unichar* U_EMPTY;


/* This constant must be used instead of 65536 (0x10000) in order
 * to avoid problems if one day the type unichar becomes larger
 * than 2 bytes */
#define MAX_NUMBER_OF_UNICODE_CHARS (1<<sizeof(unichar)*8)


/**
 * We define here the I/O encodings that are supported by the Unicode library.
 */
typedef enum {
   UTF16_LE,
   UTF16_BE,
   UTF8
} Encoding;

/**
 * Here are defined the default encodings.
 */
extern Encoding STDIN_ENC8;
extern Encoding STDOUT_ENC;
extern Encoding STDERR_ENC;
extern Encoding FILE_ENC;



/* ------------------- File functions ------------------- */
FILE* u_fopen(Encoding,char*,const char*);
int u_fclose(FILE*);
int u_fempty(Encoding,char*);
int u_is_UTF16(char*);

int u_fgetc_UTF16LE_raw(FILE*);
int u_fgetc_UTF16BE_raw(FILE*);
int u_fgetc_UTF8_raw(FILE*);
int u_fgetc_raw(Encoding,FILE*);
int u_fgetc(Encoding,FILE*);
int u_fgetc_CR(Encoding,FILE*);

int u_fread_raw(Encoding,unichar*,int,FILE*);
int u_fread(Encoding,unichar*,int,FILE*,int*);

int u_fputc_UTF16LE_raw(unichar,FILE*);
int u_fputc_UTF16BE_raw(unichar,FILE*);
int u_fputc_UTF8_raw(unichar,FILE*);
int u_fputc_raw(Encoding,unichar,FILE*);
int u_fputc(Encoding,unichar,FILE*);

int u_ungetc_UTF16LE_raw(unichar,FILE*);
int u_ungetc_UTF16BE_raw(unichar,FILE*);
int u_ungetc_UTF8_raw(unichar,FILE*);
int u_ungetc_raw(Encoding,unichar,FILE*);
int u_ungetc(Encoding,unichar,FILE*);

int u_fwrite_raw(Encoding,unichar*,int,FILE*);
int u_fwrite(Encoding,unichar*,int,FILE*);

int u_fgets(Encoding,unichar*,FILE*);
int u_fgets(Encoding,unichar*,int,FILE*);
int u_fgets2(Encoding encoding,unichar* line,FILE* f);

int u_fprintf(Encoding,FILE*,const char*,...);
int u_vfprintf(Encoding,FILE*,const char*,va_list);
int u_sprintf(unichar*,const char*,...);
int u_vsprintf(unichar*,const char*,va_list);

int u_fscanf(Encoding,FILE*,const char*,...);
int u_vfscanf(Encoding,FILE*,const char*,va_list);
int u_sscanf(unichar*,const char*,...);
int u_vsscanf(unichar*,const char*,va_list);

int htmlize(unichar*,unichar*);
int mirror(unichar*,unichar*);
int mirror(unichar*);

int u_parse_int(unichar *str);
//int u_parse_int(unichar * str, unichar ** next = NULL);




unsigned int u_strlen(const unichar*);
unichar* u_strcpy(unichar* ,const unichar* );
unichar* u_strcpy(unichar* ,const char* );
unichar* u_strncpy(unichar *,const unichar *,unsigned int);
unichar* u_strcat(unichar*,const unichar*);
unichar* u_strcat(unichar*,const char*);
int u_strcmp(const unichar*,const unichar*);
int u_strcmp(const unichar*,const char*);
unichar* u_strdup(const unichar*);
unichar* u_strdup(const char*);




unichar *strcat_realloc(unichar *,unichar *);
int is_prefixe(unichar *, unichar *);
int is_suffixe(unichar *, unichar *);
int is_sub_str(unichar *, unichar *);
int is_sub_str2(unichar *, unichar *);
unichar *strcpy_cat(unichar *,unichar *,unichar *);
unichar *strsplit_sep(unichar *,unichar *,unichar *,unichar,int);
int split_str(unichar *,unichar **,unichar);

int u_is_digit(unichar);
int u_is_basic_latin_letter(unichar);
int u_is_alpha(unichar);


int get_line(char *line, FILE *);
int get_line(unichar *line, FILE *);
int u_fputc_utf8(unsigned short c,FILE *);



/* ------------------- Character functions ------------------- */
int u_is_digit(unichar);
int u_is_basic_latin_letter(unichar);
int u_is_ASCII_alphanumeric(unichar);
int u_is_latin1_supplement_letter(unichar);
int u_is_latin_extendedA_letter(unichar);
int u_is_latin_extendedB_letter(unichar);
int u_is_IPA_extensions_letter(unichar);
int u_is_greek_letter(unichar);
int u_is_cyrillic_letter(unichar);
int u_is_armenian_letter(unichar);
int u_is_hebrew_letter(unichar);
int u_is_arabic_letter(unichar);
int u_is_thaana_letter(unichar);
int u_is_devanagari_letter(unichar);
int u_is_bengali_letter(unichar);
int u_is_gurmukhi_letter(unichar);
int u_is_gujarati_letter(unichar);
int u_is_oriya_letter(unichar);
int u_is_tamil_letter(unichar);
int u_is_telugu_letter(unichar);
int u_is_kannada_letter(unichar);
int u_is_malayalam_letter(unichar);
int u_is_sinhala_letter(unichar);
int u_is_thai_letter(unichar);
int u_is_greek_extended_letter(unichar);	//$CD:20021115
//--------Beginning of Hyungue's inserts------------------
int u_is_Hangul(unichar c);
int u_is_CJK_Unified_Ideograph(unichar c);
int u_is_CJK_compatibility_ideograph(unichar c);
int u_is_Hangul_Compatility_Jamo(unichar c);
int u_is_Hangul_Jamo(unichar c);
int u_is_Hangul_Jamo_initial_consonant(unichar c);
int u_is_Hangul_Jamo_final_consonant(unichar c);
int u_is_Hangul_Jamo_consonant(unichar c);
int u_is_Hangul_Jamo_medial_vowel(unichar c);
//--------End of Hyungue's inserts----------------
int u_is_letter(unichar);
int u_is_word(const unichar*);

unichar u_tolower(unichar);

#endif /* UNICODE_FLEX_H_ */



