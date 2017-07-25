
#include "common.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int output=1;
char fconf[50];


//---------------------------------------------
double readvalue(const char tc[])
{
    FILE *ez;
    char s[100];
    double zs;
    fprintf(stderr, "Read configuration file: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    //   printf(".%s\n",fconf);
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0)    // Vergleich ".end"
        fscanf(ez,"%s",s);
    fscanf(ez,"%s",s);
    //    printf(" < %s = %s >\n",tc,s);
    zs=atof(s);
    if (output==1)  printf(" < %s = %s >\n",tc,s);
    fclose(ez);
    return zs;
}
//---------------------------------------------
//---------------------------------------------
void readfilename(char s[], const char tc[])
{
    FILE *ez;
    fprintf(stderr, "Read configuration file: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    if (ez==NULL) {
        fprintf(stderr, "** Unable to open file: %s\n", fconf);
        exit(-1);
    }
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0) fscanf(ez,"%s",s);
    fscanf(ez,"%s",s);
    if (output==1)  printf(" < %s = %s >\n",tc,s);
    fclose(ez);
}
//---------------------------------------------

