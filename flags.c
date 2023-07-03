#include <stdio.h>
#include "markdown.h"
#include "flagprocs.h"

struct flagnames {
    int flag;
    char *name;
};

static struct flagnames flagnames[] = {
    { MKD_NOLINKS,        "!LINKS" },
    { MKD_NOIMAGE,        "!IMAGE" },
    { MKD_NOPANTS,        "!PANTS" },
    { MKD_NOHTML,         "!HTML" },
    { MKD_TAGTEXT,        "TAGTEXT" },
    { MKD_NO_EXT,         "!EXT" },
    { MKD_CDATA,          "CDATA" },
    { MKD_NOSUPERSCRIPT,  "!SUPERSCRIPT" },
    { MKD_STRICT,         "STRICT" },
    { MKD_NOTABLES,       "!TABLES" },
    { MKD_NOSTRIKETHROUGH,"!STRIKETHROUGH" },
    { MKD_TOC,            "TOC" },
    { MKD_1_COMPAT,       "MKD_1_COMPAT" },
    { MKD_AUTOLINK,       "AUTOLINK" },
    { MKD_SAFELINK,       "SAFELINK" },
    { MKD_NOHEADER,       "!HEADER" },
    { MKD_TABSTOP,        "TABSTOP" },
    { MKD_NODIVQUOTE,     "!DIVQUOTE" },
    { MKD_NOALPHALIST,    "!ALPHALIST" },
    { MKD_EXTRA_FOOTNOTE, "FOOTNOTE" },
    { MKD_NOSTYLE,        "!STYLE" },
    { MKD_DLDISCOUNT,     "DLDISCOUNT" },
    { MKD_DLEXTRA,        "DLEXTRA" },
    { MKD_FENCEDCODE,     "FENCEDCODE" },
    { MKD_IDANCHOR,       "IDANCHOR" },
    { MKD_GITHUBTAGS,     "GITHUBTAGS" },
    { MKD_NORMAL_LISTITEM,  "NORMAL_LISTITEM" },
    { MKD_URLENCODEDANCHOR, "URLENCODEDANCHOR" },
    { MKD_LATEX,          "LATEX" },
    { MKD_EXPLICITLIST,   "EXPLICITLIST" },
    { MKD_ALT_AS_TITLE,   "ALT_AS_TITLE" },
};
#define NR(x)	(sizeof x/sizeof x[0])


int
mkd_flag_isset(mkd_flag_t *flags, int i)
{
    return flags ? is_flag_set(flags, i) : 0;
}

int
mkd_is_any_flag_set(mkd_flag_t *flags)
{
    if (!flags)
        return 0;
    for (int i = 0; i < MKD_NR_FLAGS; i++) {
        if (is_flag_set(flags, i))
            return 1;
    }
    return 0;
}

void
__mkd_flags_are(FILE *f, mkd_flag_t* flags, int htmlplease)
{
    int i;
    int not, set, even=1;
    char *name;

    if ( htmlplease )
	fprintf(f, "<table class=\"mkd_flags_are\">\n");
    for (i=0; i < NR(flagnames); i++) {
	set = mkd_flag_isset(flags, flagnames[i].flag);
	name = flagnames[i].name;
	if ( not = (*name == '!') ) {
	    ++name;
	    set = !set;
	}

	if ( htmlplease ) {
	    if ( even ) fprintf(f, " <tr>");
	    fprintf(f, "<td>");
	}
	else
	    fputc(' ', f);

	if ( !set )
	    fprintf(f, htmlplease ? "<s>" : "!");

	fprintf(f, "%s", name);

	if ( htmlplease ) {
	    if ( !set )
		fprintf(f, "</s>");
	    fprintf(f, "</td>");
	    if ( !even ) fprintf(f, "</tr>\n");
	}
	even = !even;
    }
    if ( htmlplease ) {
	if ( even ) fprintf(f, "</tr>\n");
	fprintf(f, "</table>\n");
    }
}

void
mkd_mmiot_flags(FILE *f, MMIOT *m, int htmlplease)
{
    if ( m )
	__mkd_flags_are(f, &(m->flags), htmlplease);
}

void
mkd_init_flags(mkd_flag_t *p)
{
    memset(p, 0, sizeof(*p));
}

mkd_flag_t *
mkd_flags(void)
{
    mkd_flag_t *p = malloc( sizeof(mkd_flag_t) );

    if ( p )
	mkd_init_flags(p);

    return p;
}


mkd_flag_t *
mkd_copy_flags(mkd_flag_t *original)
{
    mkd_flag_t *copy = mkd_flags();
	
    if ( original && copy )
	*copy = *original;

    return copy;
}

void
mkd_copy_flags_inplace(mkd_flag_t *to, mkd_flag_t *from)
{
    for (int i = 0; i < MKD_NR_FLAGS; i++) {
        if (mkd_flag_isset(from, i))
            mkd_set_flag_num(to, i);
    }
}

void
mkd_free_flags(mkd_flag_t *rip)
{
    if (rip) free(rip);
}
