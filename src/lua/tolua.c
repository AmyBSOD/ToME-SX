/* tolua
** Support code for Lua bindings.
** Written by Waldemar Celes
** TeCGraf/PUC-Rio
** Jul 1998
** $Id: tolua.c,v 1.4 2004/06/04 13:42:10 neil Exp $
*/

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/

#include "tolua.h"
#include "lua.h"
#include "lualib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void help (void)
{
 fprintf(stderr,"\n"
         "usage: tolua [options] input_file\n"
         "\n"
         "Command line options are:\n"
         "  -v       : print version information.\n"
         "  -o  file : set output file; default is stdout.\n"
         "  -H  file : create include file.\n"
         "  -n  name : set package name; default is input file root name.\n"
         "  -p       : parse only.\n"
         "  -P       : parse and print structure information (for debug).\n"
         "  -h       : print this message.\n"
         "Should the input file be omitted, stdin is assumed;\n"
         "in that case, the package name must be explicitly set.\n\n" 
        );
}

static void version (void)
{
 fprintf(stderr, "%s (written by W. Celes)\n",TOLUA_VERSION);
}

static void setfield (lua_State* L, int table, char* f, char* v)
{
 lua_pushstring(L,f);
 lua_pushstring(L,v);
 lua_settable(L,table);
}

static void error (char* o)
{
 fprintf(stderr,"tolua: unknown option '%s'\n",o);
 help();
 exit(1);
}

int main (int argc, char* argv[])
{
 lua_State* L = lua_open(0);
 lua_baselibopen(L);
 lua_iolibopen(L);
 lua_strlibopen(L);
 lua_pushstring(L,TOLUA_VERSION); lua_setglobal(L,"TOLUA_VERSION");

 if (argc==1)
 {
  help();
  return 0;
 }
 else
 {
  int i, t;
  lua_newtable(L);
  lua_pushvalue(L,-1);
  lua_setglobal(L,"flags");
  t = lua_gettop(L);
  for (i=1; i<argc; ++i)
  {
   if (*argv[i] == '-')
   {
    switch (argv[i][1])
    {
     case 'v': version(); return 0;
     case 'h': help(); return 0;
     case 'p': setfield(L,t,"p",""); break;
     case 'P': setfield(L,t,"P",""); break;
     case 'o': setfield(L,t,"o",argv[++i]); break;
     case 'n': setfield(L,t,"n",argv[++i]); break;
     case 'H': setfield(L,t,"H",argv[++i]); break;
     default: error(argv[i]); break;
    }
   }
   else
   {
    setfield(L,t,"f",argv[i]);
    break;
   }
  }
  lua_pop(L,1);
 }

#if 1
 {
  int tolua_tolualua_open(lua_State* L);
  tolua_tolualua_open(L);
 }
#else
 {
  int i;
  char* p;
  char  path[BUFSIZ];
  char* files[] = {
                   "basic.lua",
                   "feature.lua",
                   "verbatim.lua",
                   "code.lua",
                   "typedef.lua",
                   "container.lua",
                   "package.lua",
                   "module.lua",
                   "define.lua",
                   "enumerate.lua",
                   "declaration.lua",
                   "variable.lua",
                   "array.lua",
                   "function.lua",
                   "operator.lua",
                   "class.lua",
                   "clean.lua",
                   "doit.lua",
                   NULL
                  };
  strcpy(path,argv[0]);
  p = strrchr(path,'/');
  p = (p==NULL) ? path : p+1;
  for (i=0; files[i]; ++i)
  {
   sprintf(p,"%s",files[i]);
   lua_dofile(L,path); 
  }
 }

#endif
 return 0;
}
