//---------------------------------------------------------------------
//   Thomas Ortlepp  11.09.2001 / 15.04.2002
//   Version 1.2
//   Phasendaten aus dat File ausschneiden
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "common.hpp"

#define ja   1
#define nein 0

char fa[250],fb[250];
double extstep=0.0003,offset;

int main (int argc, char *argv[])
  {
    output=nein;

    printf("cutt v 1.21  Thomas Ortlepp 21.10.2002 \n");  
    printf("--------------------------------------\n");
    char s[100],ta[10]=".END",tb[10]="loop",f3[50]=".conf";

    int c,n,q=0,zd=0;
    double tx=40.0e-12,eps=1e-15;
    printf("Auswertungstabelle der Phasenwerte erstellen\n");
    FILE *ex,*ey;
    double w;
    if (argc==2)
      {
	strcpy(fconf,argv[1]);   // config-File   .conf
	strcat(fconf,f3);          
	readfilename(fa,"simout");
	readfilename(fb,"extout");
        extstep=readvalue("extstep");
 //   printf("Einlesen der Datei :%s  %s\n",fa,fb);	
//	c=1-n;
        n=100;
        c=-99;
      }	
    else    
      {
        printf("Programmaufruf: \n");      
        printf("ext config-filename \n");      	
	printf("simout, extout muss im .conf File stehen!\n");
        printf("extstep muss auch in .conf stehen ( Schrittweite fuer Spalten 2-x ) \n"); 
        printf("Umwandlung von simout in extout Daten\n"); 
	exit(1);
      }	
    tx=1e-12*readvalue("timestep");
  // printf("Einlesen der Datei :%s  %s\n",fa,fb);	
      
    printf("Einlesen der Datei :%s\n",fa);
    ex=fopen(fa,"r+");
    ey=fopen(fb,"w+");
     
       fscanf(ex,"%s",s);
//     printf("%s\n",s);
       while (strcmp(s,ta)!=0)    // Vergleich ".end"
           fscanf(ex,"%s",s);         
 
       if (output==ja) printf("%s\n",s);
       fscanf(ex,"%s",s);
       while (strcmp(s,tb)!=0)    // Kopie bis "loop"
        {
          if (output==ja) printf("%s ",s);
	  w=atof(s);
	  if( (c<0)&&(fabs(w)>eps) )
	    {
	      n=99+c;
              c=1;
              if (output==ja)  printf("Zähler: n=%d ",n);
              q=0;
              fprintf(ey,"\n");	    
	      zd++;	      
	    }  
//	  fprintf(ey,"%s ",s);
          if (c<0) offset=extstep*(c%n+99);
 	      else offset=extstep*((c-1)%n);
          fprintf(ey,"%15.14lf ",w+offset );
          fscanf(ex,"%s ",s);  
	  if (c++%n==0) 
	    {
              if (output==ja) printf("\n");
              fprintf(ey,"\n");	    
	    }        
	}        
    printf("%d Zeilen geschrieben!\n",c+3);        
    fclose(ey);  
    fclose(ex); 
     
    printf("Fertig!\n");            

    return 0;
  }
