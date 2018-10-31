/*
 *  $Header: /home/cvs/3520G/user/openswan-2.4.9/doc/utils/rfc_pg.c,v 1.1.1.1 2011/08/17 11:10:49 brwang Exp $
 *
 * from 2-nroff.template file.
 *
 *  Remove N lines following any line that contains a form feed (^L).
 *  (Why can't this be done with awk or sed?)
 *
 *  OPTION:
 *	-n#	Number of lines to delete following each ^L (0 default).
 * $Log: rfc_pg.c,v $
 * Revision 1.1.1.1  2011/08/17 11:10:49  brwang
 * 3520G start
 *
 * Revision 1.1.1.1  2011/03/15 05:24:33  lei.minghuan
 * import ralink
 *
 * Revision 1.1.1.1  2009/12/14 12:44:51  brwang
 * ralink wlan
 *
 * Revision 1.1.1.1  2007-10-08 08:01:06  steven
 *
 *
 * Revision 1.1  2002/07/23 18:42:43  mcr
 * 	required utility from IETF to help with formatting of drafts.
 *
 */
#include <stdio.h>

#define FORM_FEED	'\f'
#define OPTION		"n:N:"		/* for getopt() */

extern char *optarg;
extern int optind;

main(argc, argv)
int	argc;
char	*argv[];
{
  int	c,				/* next input char */
	nlines = 0;			/* lines to delete after ^L */
  void	print_and_delete();		/* print line starting with ^L,
					   then delete N lines */

/*********************** Process option (-nlines) ***********************/

  while ((c = getopt(argc, argv, OPTION)) != EOF)
    switch(c)
    {
      case 'n' :
      case 'N' :  nlines = atoi(optarg);
		  break;
    }
/************************* READ AND PROCESS CHARS **********************/

  while ((c = getchar()) != EOF)
    if (c == FORM_FEED)
      print_and_delete(nlines);		/* remove N lines after this one */
    else
      putchar(c);			/* we write the form feed */
  exit(0);
}


/*
 *  Print rest of line, then delete next N lines.
 */
void print_and_delete(n)
int  n;					/* nbr of lines to delete */
{
  int	c,				/* next input char */
	cntr = 0;			/* count of deleted lines */

  while ((c = getchar()) != '\n')	/* finish current line */
    putchar(c);
  putchar('\n');			/* write the last CR */
  putchar(FORM_FEED);

  for ( ; cntr < n; cntr++)
    while ((c = getchar()) != '\n')
      if (c == EOF)
	exit(0);			/* exit on EOF */
  putchar(c);				/* write that last CR */
}

