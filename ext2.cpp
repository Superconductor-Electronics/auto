//---------------------------------------------------------------------
//   Thomas Ortlepp  11.09.2001 / 15.04.2002 / 17.05.2004 / 22.06.2004
//                   24.01.2006
//   Version 2.3
//   Phasendaten aus dat File ausschneiden
//   
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define ja   1
#define nein 0

int output=ja,ende=0;
char fa[250],fb[250];
double maxzerovoltage=0.0, minonevoltage=80.0,voltagesw;
double pi=3.14159625;
double timing[100000];
int junction[100000];
int bits[100000];

char fconf[50];
//---------------------------------------------
double readvalue(char tc[])
  {
    FILE *ez; 
    char s[100];
    double zs;
    printf("Reading configfile: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    if (ez==NULL) 
      {  
         printf("Config-File not found.\n");
         assert(0);
      }

 //   printf(".%s\n",fconf);
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0)    // Vergleich ".end"
       fscanf(ez,"%s",s);         
    fscanf(ez,"%s",s);
//    printf(" < %s = %s >\n",tc,s);
    zs=atof(s);
    printf(" < %s = %s >\n",tc,s);
    fclose(ez);    
    return zs;
  }
//---------------------------------------------
//---------------------------------------------
void readfilename(char s[], char tc[])
  {
    FILE *ez; 
    printf("Reading configfile: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    if (ez==NULL) 
      {  
         printf("Config-File not found.\n");
         assert(0);
      }
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0) fscanf(ez,"%s",s);         
    fscanf(ez,"%s",s);
    printf(" < %s = %s >\n",tc,s);
    fclose(ez);    
  }
//---------------------------------------------
/*
void scan ()
  {
    FILE *ex,*ey;
    char s[100],ta[10]=".END",tb[10]="loop";
    double w,eps=1e-15,tx=40.0e-12;
    int c,n,zd=0,q=0,Q=1,h;

    n=100;
    c=-99;

    tx=1e-12*readvalue("timestep");

    printf("Reading data:%s\n",fa);
    ex=fopen(fa,"r+");
    ey=fopen(fb,"w+");
     
       fscanf(ex,"%s",s);
//     printf("%s\n",s);
       while (strcmp(s,ta)!=0)    // Vergleich ".end"
           fscanf(ex,"%s",s);     // Ende vom spice file und Anfang der Daten    
  
       if (output==ja) printf("%s\n",s);
       fscanf(ex,"%s",s);

       ende=0;       
       while ((ende!=-1)&&(strcmp(s,tb)!=0))    // Kopie bis "loop" oder eof
        {
          if (output==ja)  printf("#%s ",s);
	  w=atof(s);
	  if( (c<0)&&(fabs(w)>eps) )
	    {
	      n=99+c;
              c=1;
              printf("Zähler: n=%d ",n);
              q=0;
              fprintf(ey,"\n");	    
	      zd++;	      
	    }  
//	  fprintf(ey,"%s ",s);
// --------- Ausgabezeit --------------
//          if (c%n==1)
//	    {
//	      if (w>Q*tx-eps) { outx=1; Q++; }
//	       else outx=0;
//	    }   
// ------------------------------------	    
      //  if ((c%n==1)) printf("%le  %le\n",w,Q*tx);      

//	  printf("%d ",c);

             if ((c%n)==1) 
               {
	         w=w*1e12;
  	         fprintf(ey,"%lf ",w); 
	       }	 
	       else 
	         {
		  if ( ((c%n)==0)  && (voltagesw!=0.0) )
		   {
		     if (w>minonevoltage*1e-6) w=1.0; 
		       else if (fabs(w)< maxzerovoltage*1e-6) w=0.0;
		         else w=-5.0;
		    }      // Spannung am sfqdc - Erweiterung am 17.05.04
		    else
		       w=w/6.28;
		   h=(int)(w+1000.5)-1000;
	//	   r=w-h;
	//	   if ((r>0.3)&&(r<-0.3)) h=-99; // Anpassung ->22.10.01
	           fprintf(ey,"%d ",h);		   
		 }  

	  ende=fscanf(ex,"%s ",s);  // bei neuem Compiler =1 anstatt =0
          if (output==ja)  printf("(#%s )%d",s,ende);	  
	  if (c++%n==0) 
	    {
              if (output==ja) printf("\n");
              q=0;
              fprintf(ey,"\n");	    
	      zd++;
	    }   
	}          

    printf("%d lines processed!\n",c+4);        
    fclose(ey);  
    fclose(ex); 
    printf("Ready!\n");            
  }
*/

void scan2()
  {
    FILE *ex,*ey;
    char s[100],ta[10]=".END",tb[10]="loop";
    double w,eps=1e-15,tx=40.0e-12;
    int i,j,c,n,zd=0,q=0,Q=1,h;
    double werte0[100],werte1[100],x,y,r;
    double phase_a,phase_b,zeit_a,zeit_b,dt,tschalt;
    double mittel=0.0,t;
    int cmittel=0;
    int werteint[100];
    int count=1;
    int pone=0,pzero=0,pclock=0;
    int scount=0;
    double t_data, t_clock, t_output, t_x;
    double comp_delay=0.0;
    n=100;
    c=-99;
    double timesum=0,streusum=0;
    tx=1e-12*readvalue("timestep");

//-------------Eingabefile oeffnen-------------------
    printf("Reading data:%s\n",fa);
    ex=fopen(fa,"r+");
    if (ex==NULL) 
      {  
         printf("Data-File not found.\n");
         assert(0);
      }
//-------------Ausgabefile oeffnen-------------------
    ey=fopen(fb,"w+");  // wird angelegt
//---------------------------------------------------

//------bis Datenende durchlesen --------------------     
       fscanf(ex,"%s",s);
       int zeile=0,maxzeile=31313;
       while ((zeile<maxzeile)&&(strcmp(s,ta)!=0))      // Vergleich ".end"
          {
	    zeile++;
            fscanf(ex,"%s",s);     // Ende vom spice file und Anfang der Daten    
	  }
       if (zeile==maxzeile)
         {
            printf("End Label in jsim file after %d lines not found.\n",zeile);
//            assert(0);
	 }  
//---------------------------------------------------	   
   
       fscanf(ex,"%s",s);    
       w=atof(s);
       n=0;

       while (fabs(w)<eps)    // Nullen in der ersten Zeile zaehlen
        {
           fscanf(ex,"%s",s);    
           w=atof(s);
	   werte0[n]=w;       // alte Werte setzen
	   werteint[n]=0;     // integer Phasenwerte auf Null setzen
	   n++;
	}      
	
       	
       printf("number of junctions: %d \n",n-1);	    

       ende=0;       
       werte1[0]=1e12*w;  // erstem Zeitwert bereits gelesen(in ps)
       i=1;       
       t_output=-1.0;
       while ((ende!=-1)&&(strcmp(s,tb)!=0))    // Kopie bis "loop" oder eof
        {
 	   ende=fscanf(ex,"%s ",s);  // bei neuem Compiler =1 anstatt =0
           w=atof(s);
           if (i==0) w=w*1e12;  // Zeitnormierung auf ps
	   werte1[i]=w;         // neue Werte
	   i++;                 // Spaltenzaehler: 0..time, 1..n-1 restliche Werte

	   if (i==n)            // eine Spalte voll, jetzt Auswertung
	     {
	       for (i=1; i<n; i++)  // alle Kontakte durchgehen    
	         {       
		   x=werte1[i]/2/pi;         // normierter Phasenwert
		   y=(int)(x+1000.5)-1000;   // ganzzahliger Phasenwert
	   
		   if (y!=werteint[i])  // neuer Phasenwert != alter Phasenwert
		     {
		        zeit_a=werte0[0];
			zeit_b=werte1[0];
			phase_a=werte0[i];
			phase_b=werte1[i];
		        dt=(2*pi*(werteint[i]+0.5)-phase_a)*(zeit_b-zeit_a)/(phase_b-phase_a);
			tschalt=zeit_a+dt;  // linearer Fit für die genaue Bestimmung der Schaltzeit
//  		        printf("%d, %lf, %lf \n",i,y,tschalt);
			werteint[i]=(int)y;                     // Anzahl der Shaltvorgänge
			
			// --Ausgabe--
                        junction[scount]=i;
			timing[scount]=tschalt;
//------------------------------
			if (i==1) pclock++;
			bits[pclock]=-1;			
                        if (scount>0)
			  {
			    if ( (junction[scount]==2) && (junction[scount-1]==1) )
			      {  // comparator output "one"
			        pone++;   
                                comp_delay=timing[scount]-timing[scount-1];
				timesum+=comp_delay;
	                        printf("[%d %d]->",junction[scount-1],junction[scount]);
				printf("%d  %lf \n",pone, comp_delay);
				bits[pclock]=1;				
			      }
			    if ( (junction[scount]==1) && (junction[scount-1]==1)  )
			      {  // comparator output "zero"
			        pzero++;
			      }

			  }
//------------------------------	                                     
			scount++;  // printf("%d \n",scount);

			
	             }
		   count++;
		   
	         }    
	       for (i=0; i<n; i++) werte0[i]=werte1[i];         // Kopie neue auf alte Werte
	       i=0;  
	     }  
           //  printf("(#%s )%d\n",s,ende);
	 }          

    double timesumme;	 
    if (pone>0) timesumme=timesum/pone; 
    for (i=1; i<scount; i++)
      if ( (junction[i]==2) && (junction[i-1]==1) )      
        {
	   t=timesumme-timing[i]+timing[i-1];
  //         printf("# %lf\n",t);

           streusum+=t*t;       
        }    

    for (i=1; i<=pclock; i++)
      {
         printf("%2d ",bits[i]);
      }
    printf("\n");
    
    double suma=0,sumb,c1=0;

    for (i=2; i<=pclock; i++)
      {
         suma+=bits[i]*bits[i-1];
      }
    c1=suma/(pclock-1);  

    printf(    "%d %d %lf %lf %lf\n", pclock, pone, timesum, streusum , c1);   
    fprintf(ey,"%d %d %lf %lf %lf\n", pclock, pone, timesum, streusum ,c1  );
    
//    printf("t_clock - t_data = %2.5lf, output =  %d\n",t_clock-t_data, i);    


    printf("%d lines processed.\n",count);        
    fclose(ey);  
    fclose(ex); 
    printf("Ready!\n");            
  }

int main (int argc, char *argv[])
  {
    printf("---------------------------------------------------\n");
    printf("ext v 2.3  Thomas Ortlepp 11.09.2001 - 24.01.2006 \n");  
    printf("---------------------------------------------------\n");
    char s[100],ta[10]=".END",tb[10]="loop",f3[50]=".conf";

    int c,n;
    double altephase[50];
    for (n=0; n<50; n++) altephase[n]=0.0;   // alte Phasenwerte
    double r;
    printf("processing of simulation data for comparision to *.soll file.\n");

    if (argc==2)
      {
	strcpy(fconf,argv[1]);   // config-File   .conf
	strcat(fconf,f3);          
	readfilename(fa,"simout");
	readfilename(fb,"extout");
	
	
	voltagesw=readvalue("voltageswitch");
	if (voltagesw!=0.0)
	  {
	    maxzerovoltage=readvalue("maxzerovoltage");
	    minonevoltage=readvalue("minonevoltage");	    	    
	    printf("Last coloumn is a voltage (0:%lf,1:%lf)\n",
         	      maxzerovoltage,minonevoltage);
	  }
	
 //   printf("Einlesen der Datei :%s  %s\n",fa,fb);	
//	c=1-n;
      }	
    else    
      {
        printf("How to call ext: \n");      
        printf("ext config-filename \n");      	
	printf("simout, extout, timestep has to be defined in *.conf file !\n");
        printf("input file: simout  and output file: extout \n"); 
	exit(1);
      }	

  // printf("Einlesen der Datei :%s  %s\n",fa,fb);	
      
    scan2();

    return 0;
  }
