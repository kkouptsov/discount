/* markdown: a C implementation of John Gruber's Markdown markup language.
 *
 * Copyright (C) 2007-2011 David L Parsons.
 * The redistribution terms are provided in the COPYRIGHT file that must
 * be distributed with this source code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#include "markdown.h"
#include "amalloc.h"

#if HAVE_LIBGEN_H
#include <libgen.h>
#endif

/*
 * discount2 shim layer
 */

/* 2.2.6 flags */
#define MKD2_NOLINKS		0x00000001
#define MKD2_NOIMAGE		0x00000002
#define MKD2_NOPANTS		0x00000004
#define MKD2_NOHTML		0x00000008
#define MKD2_STRICT		0x00000010
#define MKD2_TAGTEXT		0x00000020
#define MKD2_NO_EXT		0x00000040
#define MKD2_CDATA		0x00000080
#define MKD2_NOSUPERSCRIPT	0x00000100
#define MKD2_STRICT2   		0x00000200
#define MKD2_NOTABLES		0x00000400
#define MKD2_NOSTRIKETHROUGH	0x00000800
#define MKD2_TOC		0x00001000
#define MKD2_1_COMPAT		0x00002000
#define MKD2_AUTOLINK		0x00004000
#define MKD2_SAFELINK		0x00008000
#define MKD2_NOHEADER		0x00010000
#define MKD2_TABSTOP		0x00020000
#define MKD2_NODIVQUOTE		0x00040000
#define MKD2_NOALPHALIST	0x00080000
#define MKD2_NODLIST		0x00100000
#define MKD2_EXTRA_FOOTNOTE	0x00200000
#define MKD2_NOSTYLE		0x00400000
#define MKD2_NODLDISCOUNT	0x00800000
#define	MKD2_DLEXTRA		0x01000000
#define MKD2_FENCEDCODE		0x02000000
#define MKD2_IDANCHOR		0x04000000
#define MKD2_GITHUBTAGS		0x08000000
#define MKD2_URLENCODEDANCHOR	0x10000000
#define MKD2_LATEX		0x40000000
#define MKD2_EXPLICITLIST	0x80000000


#ifdef V2_INTERFACE
/*
 * convert v2 flags into v3 flags
 */
#define set_mkd3_flag(flags, item) ((flags)->bit[item] = 1)
#define clear_mkd3_flag(flags, item) ((flags)->bit[item] = 0)

static void
convert_v2flags(mkd2_flag_t* v2flags, mkd3_flag_t *v3flags)
{
    int i;
    uint32_t bitmask = *v2flags;
    uint32_t bit;

    mkd_init_flags(v3flags);
    set_mkd3_flag(v3flags, MKD_DLDISCOUNT);	/* default for discount dls changed from on to off in v3 */

    for (i=0; i < sizeof(uint32_t)*8; i++) {

		bit = 1L << i;

		switch ( bitmask & bit ) {
			case MKD2_STRICT:
				set_mkd3_flag(v3flags, MKD_STRICT);
				break;
			case MKD2_NOLINKS:
				set_mkd3_flag(v3flags, MKD_NOLINKS);
				break;
			case MKD2_NOIMAGE:
				set_mkd3_flag(v3flags, MKD_NOIMAGE);
				break;
			case MKD2_NOPANTS:
				set_mkd3_flag(v3flags, MKD_NOPANTS);
				break;
			case MKD2_NOHTML:
				set_mkd3_flag(v3flags, MKD_NOHTML);
				break;
			case MKD2_TAGTEXT:
				set_mkd3_flag(v3flags, MKD_TAGTEXT);
				break;
			case MKD2_NO_EXT:
				set_mkd3_flag(v3flags, MKD_NO_EXT);
				break;
			case MKD2_CDATA:
				set_mkd3_flag(v3flags, MKD_CDATA);
				break;
			case MKD2_NOSUPERSCRIPT:
				set_mkd3_flag(v3flags, MKD_NOSUPERSCRIPT);
				break;
			case MKD2_STRICT2:
				set_mkd3_flag(v3flags, MKD_STRICT);
				break;
			case MKD2_NOTABLES:
				set_mkd3_flag(v3flags, MKD_NOTABLES);
				break;
			case MKD2_NOSTRIKETHROUGH:
				set_mkd3_flag(v3flags, MKD_NOSTRIKETHROUGH);
				break;
			case MKD2_TOC:
				set_mkd3_flag(v3flags, MKD_TOC);
				break;
			case MKD2_1_COMPAT:
				set_mkd3_flag(v3flags, MKD_1_COMPAT);
				break;
			case MKD2_AUTOLINK:
				set_mkd3_flag(v3flags, MKD_AUTOLINK);
				break;
			case MKD2_SAFELINK:
				set_mkd3_flag(v3flags, MKD_SAFELINK);
				break;
			case MKD2_NOHEADER:
				set_mkd3_flag(v3flags, MKD_NOHEADER);
				break;
			case MKD2_TABSTOP:
				set_mkd3_flag(v3flags, MKD_TABSTOP);
				break;
			case MKD2_NODIVQUOTE:
				set_mkd3_flag(v3flags, MKD_NODIVQUOTE);
				break;
			case MKD2_NOALPHALIST:
				set_mkd3_flag(v3flags, MKD_NOALPHALIST);
				break;
			case MKD2_EXTRA_FOOTNOTE:
				set_mkd3_flag(v3flags, MKD_EXTRA_FOOTNOTE);
				break;
			case MKD2_NOSTYLE:
				set_mkd3_flag(v3flags, MKD_NOSTYLE);
				break;
			case MKD2_NODLDISCOUNT:
				set_mkd3_flag(v3flags, MKD_DLDISCOUNT);
				break;
			case MKD2_DLEXTRA:
				set_mkd3_flag(v3flags, MKD_DLEXTRA);
				break;
			case MKD2_FENCEDCODE:
				set_mkd3_flag(v3flags, MKD_FENCEDCODE);
				break;
			case MKD2_IDANCHOR:
				set_mkd3_flag(v3flags, MKD_IDANCHOR);
				break;
			case MKD2_GITHUBTAGS:
				set_mkd3_flag(v3flags, MKD_GITHUBTAGS);
				break;
			case MKD2_URLENCODEDANCHOR:
				set_mkd3_flag(v3flags, MKD_URLENCODEDANCHOR);
				break;
			case MKD2_LATEX:
				set_mkd3_flag(v3flags, MKD_LATEX);
				break;
			case MKD2_EXPLICITLIST:
				set_mkd3_flag(v3flags, MKD_EXPLICITLIST);
				break;
        }
    }
}
#endif

#ifdef V2_INTERFACE
MMIOT *mkd_in(FILE *input, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return (MMIOT*)__mkd_in(input, &v3flags);
}
#else
MMIOT *mkd_in(FILE *input, mkd_flag_t* v3flags)
{
    return (MMIOT*)__mkd_in(input, v3flags);
}
#endif


#ifdef V2_INTERFACE
MMIOT *
mkd_string(const char *text, int length, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return (MMIOT*)__mkd_string(text, length, &v3flags);
}
#else
MMIOT *
mkd_string(const char *text, int length, mkd_flag_t* v3flags)
{
    return (MMIOT*)__mkd_string(text, length, v3flags);
}
#endif


#ifdef V2_INTERFACE
MMIOT *
gfm_in(FILE *input, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return (MMIOT*)__gfm_in(input, &v3flags);
}
#else
MMIOT *
gfm_in(FILE *input, mkd_flag_t* v3flags)
{
    return (MMIOT*)__gfm_in(input, v3flags);
}
#endif


#ifdef V2_INTERFACE
MMIOT *
gfm_string(const char *text, int length, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return (MMIOT*)__gfm_string(text, length, &v3flags);
}
#else
MMIOT *
gfm_string(const char *text, int length, mkd_flag_t* v3flags)
{
    return (MMIOT*)__gfm_string(text, length, v3flags);
}
#endif


#ifdef V2_INTERFACE
int
mkd_compile(MMIOT *document, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __mkd_compile((Document*)document, &v3flags);
}
#else
int
mkd_compile(MMIOT *document, mkd_flag_t* v3flags)
{
    return __mkd_compile((Document*)document, v3flags);
}
#endif


#ifdef V2_INTERFACE
int
mkd_dump(Document *document, FILE *output, mkd_flag_t* v2flags, char *word)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __mkd_dump(document, output, &v3flags, word);
}
#else
int
mkd_dump(Document *document, FILE *output, mkd_flag_t* v3flags, char *word)
{
    return __mkd_dump(document, output, v3flags, word);
}
#endif


#ifdef V2_INTERFACE
int
markdown(Document *document, FILE *output, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __markdown(document, output, &v3flags);
}
#else
int
markdown(Document *document, FILE *output, mkd_flag_t* v3flags)
{
    return __markdown(document, output, v3flags);
}
#endif


#ifdef V2_INTERFACE
int
mkd_line(char *text, int size, char **ret, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __mkd_line(text, size, ret, &v3flags);
}
#else
int
mkd_line(char *text, int size, char **ret, mkd_flag_t* v3flags)
{
    return __mkd_line(text, size, ret, v3flags);
}
#endif


#ifdef V2_INTERFACE
int
mkd_xhtmlpage(Document *document, mkd_flag_t* v2flags, FILE *output)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __mkd_xhtmlpage(document, &v3flags, output);
}
#else
int
mkd_xhtmlpage(Document *document, mkd_flag_t* v3flags, FILE *output)
{
    return __mkd_xhtmlpage(document, v3flags, output);
}
#endif


#ifdef V2_INTERFACE
int
mkd_generateline(char *text, int size, FILE *output, mkd_flag_t* v2flags)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    return __mkd_generateline(text, size, output, &v3flags);
}
#else
int
mkd_generateline(char *text, int size, FILE *output, mkd_flag_t* v3flags)
{
    return __mkd_generateline(text, size, output, v3flags);
}
#endif


#ifdef V2_INTERFACE
void
mkd_flags_are(FILE *output, mkd_flag_t* v2flags, int htmlplease)
{
    mkd3_flag_t v3flags;
    convert_v2flags(v2flags, &v3flags);
    __mkd_flags_are(output, &v3flags, htmlplease);
}
#else
void
mkd_flags_are(FILE *output, mkd_flag_t* v3flags, int htmlplease)
{
    __mkd_flags_are(output, v3flags, htmlplease);
}
#endif
