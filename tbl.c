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


#include	<stdio.h>
#include	"cli.h"


int quit_emulator();
int cont();
int refresh();
int clc(), sec(), sev(), clb(), seb();
int clri(), sei(), sez(), clz(), sen(), cln(), sed(), cld(), clv();
int ver();
int shell_escape();
int do_soft_reset();
int do_bload();
int do_bsave();
int show_point();
int do_jump();
int ldx(), ldy(), lds(), lda();
int trace();
int set_break_point(), clear_break_point();
int notrace();
int insert_disk();
int dissassemble();
int hex_dump();
int deposit();
int phantom_trace();
int no_phantom_trace();
int cd();
int map();
int sex();
int help();
int set_escape_char();
int hack();
int init_point(), push_point(), dup_point(), pop_point();
int disk_names();
#ifdef	JOYSTICK_SUPPORT
int do_joystick_recalibrate();
#endif	JOYSTICK_SUPPORT

struct cmdtbl first_tbl[] = {
	{"!",		shell_escape},
	{".",		show_point},
	{"bload",	do_bload},
	{"breakpoint",	set_break_point},
	{"bsave",	do_bsave},
	{"cd",		cd},
	{"clc",		clc},
	{"cld",		cld},
	{"cli",		clri},
	{"cln",		cln},
	{"clv",		clv},
	{"clz",		clz},
	{"continue",	refresh},
	{"deposit",	deposit},
	{"disks",	disk_names},
	{"dup",		dup_point},
	{"escape",	set_escape_char},
	{"examine",	hex_dump},
	{".e",		hex_dump},
	{"hack",	hack},
	{"help",	help},
	{"insert",	insert_disk},
	{"jmp",		do_jump},
	{"lda",		lda},
	{"lds",		lds},
	{"ldx",		ldx},
	{"ldy",		ldy},
	{"list",	dissassemble},
	{"map",		map},
	{"nobreak",	clear_break_point},
	{"notrace",	notrace},
	{"pop",		pop_point},
	{"push",	push_point},
	{"quit",	quit_emulator},
	{"reset",	do_soft_reset},
#ifdef	JOYSTICK_SUPPORT
	{"recalibrate",	do_joystick_recalibrate},
#endif	JOYSTICK_SUPPORT
	{"sec",		sec},
	{"sed",		sed},
	{"sei",		sei},
	{"sen",		sen},
	{"sev",		sev},
	{".sex",	sex},
	{"sez",		sez},
	{"trace",	trace},
	{".version",	ver},
	{".c",		refresh},
	{".clb",	clb},
	{".d",		deposit},
	{".e",		hex_dump},
	{".l",		dissassemble},
	{".phantom",	phantom_trace},
	{".seb",	seb},
	{".nophantom",	no_phantom_trace},
	{NULL,		NULL},
};

