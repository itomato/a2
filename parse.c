/*
 *  a2, an Apple II emulator in C
 *  (c) Copyright 1990 by Rich Skrenta
 *
 *  Command line interface written by Tom Markson
 *
 *  Distribution agreement:
 *
 *	You may freely copy or redistribute this software, so long
 *	as there is no profit made from its use, sale, trade or
 *	reproduction.  You may not change this copyright notice,
 *	and it must be included prominently in any copy made.
 *
 *  Send emulator related mail to:  skrenta@blekko.commodore.com
 *				    skrenta@blekko.uucp
 */



#import	<stdio.h>
#import	<libc.h>
#import	<strings.h>
#import	<signal.h>
#import	<ctype.h>
#import	"a2.h"
#import	"cli.h"


char	*
split(s)
char	*s;
{
	char	*t;
	extern char	*strchr();

	assert(s != NULL);

	t = strchr(s, ' ');
	if (t == NULL)
		return("");

	assert(*t == ' ');

	*t++ = '\0';

	while (*t && *t == ' ')
		t++;

	return(t);
}


int match(char	*a, char *b)
{

	assert(a != NULL);
	assert(b != NULL);

	if (*b == '.' && strlen(b) > 1)
		b++;
	while (*a && *b) {
		if (toupper(*a) != toupper(*b))
			return(NO_MATCH);
		a++;
		b++;
	}

	if (!*a) {
		if (!*b)
			return(IDENTICAL);
		else
			return(PARTIAL);
	} else
		return(NO_MATCH);
}


int is_hex_number(char *s)
{
	char	*temp;
	temp = s;
	assert(s != NULL);
	while (*s)
		if (!isxdigit(*s++))
			return(FALSE);
	if (temp != s)
		return TRUE;
	else
		return FALSE;
}


long	get_hex_number(s)
char	*s;
{
	unsigned int	x;
	if (is_hex_number(s))
		sscanf(s, "%x", &x);
	else
		return(-1);
	return( (long) x );
}


int parse_str(struct cmdtbl tbl[], char	*s, char *t)
{
int i;
int ret;
int partial = -1;
int count = 0;

	i = 0;
	while (tbl[i].name != NULL) {
		ret = match(s, tbl[i].name);
		if (ret == IDENTICAL) {
			partial = i;
			count = 0;
			break;
		} else if (ret == PARTIAL) {
			partial = i;
			count++;
		}

		i++;
	}

	if (partial == -1)
		return(NO_MATCH);

	if (count > 1)
		return(AMBIGUOUS);

	if (tbl[partial].func == NULL)
		return(NOT_IMPLEMENTED);

	assert(t != NULL);
	return  (*tbl[partial].func)(t);
}

void dump_list(struct cmdtbl tbl[], char *header)
{
	int	i, count;

	printf(header);
	i = 0;
	count = 0;
	while (tbl[i].name != NULL) {
		if (*tbl[i].name != '.' || strlen(tbl[i].name) == 1) {
			if (count % 4 == 3)
				printf("    %-15s\n", tbl[i].name);
			else
				printf("    %-15s", tbl[i].name);

			count++;
		}
		i++;
	}

	if (count % 4 != 0)
		printf("\n");
}

int parse(struct cmdtbl tbl[], char	*s)
{
	char	*t;

	if (*s == '!') {			/* shell escape-ish thing */
		shell_escape(++s);
		return(FALSE);
	}

	t = split(s);

	if (*s == '\0') {
		return(TRUE);				/* single step */

	} else if (strcmp(s, "?") == 0) {
		dump_list(tbl, "Command, one of the following:\n\n");

	} else switch (parse_str(tbl, s, t)) {
		case AMBIGUOUS:
			printf("Ambiguous command '%s'.\n", s);
			break;

		case NO_MATCH:
			printf("Unknown command.  Type ? for a list.\n");
			break;

		case NOT_IMPLEMENTED:
			printf("Sorry, command not implemented yet.\n");
			break;

		case OK:
			break;

		case DISPLAY:
			status(stdout);
			break;

		default:
			assert(FALSE);
	}

	return(FALSE);
}
