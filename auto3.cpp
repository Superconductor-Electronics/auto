//----------------------------------------------------
//    Thomas Ortlepp    14.02.2001/13.04.2003/24.07.2009
//    
//    1 Automatisch Parameter in Jsim Files setzen
//    2 Simulieren
//    3 Extrakt von Daten in Textfile
//
//----------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <fstream.h>
#include <string.h>

//------------------------------
// Technologieparameter
//------------------------------
double I0RN = 0.3e-3;    //  Werte werden mit .conf File Werten überschrieben
double betac= 1.0;       //

double Phi0 = 2.07e-15;
double Pi   = 3.14159265;

double P2;   // I0RN;
double P1;   // Phi0*betac/(2*Pi*P2*P2);
//------------------------------

//----------------------------------------------------
const int dim=70;

double parawert[dim];   // Feld für Parameterdaten
double parawert2[dim];
int    parastatus[dim]; // Parameterstatus  1..9..Skalierungsnummer  0..Fix
int    parameteraktiv=0;
double paracenter[dim];
char   paraname[dim]; // Erstatzbezeichung im jsim Netzliste
char   paralabel[dim][40];  // echte Bezeichnung
int    maxmarginstep,maxyieldstep,stepnumber;
double maxyieldstreuung,yield;
int n,maxtest,streuung,mode=0;
double para1,para2,para3,para4,para5;
char fa[250],fb[250],simcir[250],fd[250],fe[250],ff[250],fcc[250];
char f6[50]=".conf";   // Endung config-File
char para1c, para2c;
char fconf[50];
//---------------------------------------------
double readvalue(const char tc[])
  {
    FILE *ez; 
    char s[100];
    double zs;
    printf("Konfigurationsdatei einlesen: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0)    // Vergleich ".end"
       fscanf(ez,"%s",s);         
    fscanf(ez,"%s",s);
    zs=atof(s);
    printf(" < %s = %s >\n",tc,s);
    fclose(ez);    
    return zs;
  }
//---------------------------------------------
//---------------------------------------------
void readfilename(char s[], const char tc[])
  {
    FILE *ez; 
    printf("Konfigurationsdatei einlesen: %s -> ",fconf);
    ez=fopen(fconf,"r+");
    fscanf(ez,"%s",s);
    while (strcmp(s,tc)!=0) fscanf(ez,"%s",s);         
    fscanf(ez,"%s",s);
    printf(" < %s = %s >\n",tc,s);
    fclose(ez);    
  }
//---------------------------------------------


FILE *ex,*ey,*ez,*eq,*ee;

//----------------------------------------------------
void genfile ()    // Parameter musz mit @z  im Quellfile stehen
  {                // z ist dabei ein beliebiges Bezeichner des Parameters
    
    char c;
    double glob[10],pw;
    int i,j,found,count=0;
//    printf("Erzeuge Parameter-Netzliste\n");
    ex=fopen(fa,"r+");
    ez=fopen(simcir,"w+");
//------globale Parameter ermitteln----
    for (j=0;j<10;j++)
      {
       glob[j]=1.0;
       c=48+j;
       for (i=0;i<n;i++)
        if (c==paraname[i])   // globale Parameter suchen
	    glob[j]=parawert[i];
      }  
//-------------------------------------    
    while (( c=fgetc(ex))!=EOF )
      {
        if (c=='@')  // Parameterschalter gefunden
          {
             c=fgetc(ex);
	     found=0;
	     for (i=1;i<n;i++)
	      if (c==paraname[i])
	       {
	         
                  c=fgetc(ex);	    // ein Leerzeichen entfernen
		  
		  pw=parawert[i]*glob[parastatus[i]];
		  if (c==' ') 
		    {
  		     found=1; count++;
		     fprintf(ez,"%3.3lf",pw);
		    } 
		  else   // Parameter fuer RSCJ Model
		    {
		      if (c=='c')  
		        {
			  found=1;
			  fprintf(ez,"%3.3lf",1e6*P1*pw);
			}  
		      if (c=='r')  
		         {
			  found=1;
			  if (mode==2)  // Optimierung -> Rn wird mit verändert!
 			    fprintf(ez,"%2.3lf ",1e6*P2/pw);
			  else          // Marginbestimmung -> Rn wird nicht geändert!
 			    fprintf(ez,"%2.3lf ",1e6*P2/pw);			  
			 } 
                      c=fgetc(ex);	    // ein Leerzeichen entfernen		      
		    }
	       }
	     if (found==0)
	       {
	         printf("Unbekannter Bezeichner : '%c' \n",c);
	       }  
          }
        else  fprintf(ez,"%c",c);
      }    
//    printf("%d Parameter eingesetzt.\n",count);  
    fclose(ez);
    fclose(ex);
  }
//----------------------------------------------------
void ergebnis()    
  {
    FILE *ex;
    char s[200];
    int n=0,b=0;
    char ein[150]="Zeitdifferenz:";
    printf("Daten extrahieren...\n");
    ex=fopen("jtl20-slavex.pik","r+");
   
    while (b<1) 
      {
        fscanf(ex,"%s",&s);
    //    printf("> %s\n",s);
        if (strcmp(s,ein)==0)
          {
            fprintf(ez," ");
            fscanf(ex,"%s",&s);  fprintf(ez," %s\n",s);
            printf("Schreibe : %s\n",s);
            b++;
          };
        n++;
      }
    fclose(ex);    
  }
//----------------------------------------------------

int funktionstest()
  {
    char s[100];
    double w1;
    int test=0;
    eq=fopen(fd,"r+");
        {
          fscanf(eq,"%s",s);   w1=atof(s);   // Ergebnis
	}        
 
    if (fabs(w1)<1e-5) test++;	
    fclose(eq);
    if (test==1) return 1;
       else return 0;
  }

int simu()
  {
     int e;
         genfile();  // Erzeug Parameter Netzliste
//	 printf("Starte Simulation (run)...\n");

         char command[100]="qrun ";
  	 strcat(command,simcir);  // simcir
	 strcat(command," ");
	 strcat(command,ff);  // simout
	 strcat(command," ");
	 strcat(command,fcc); // conf
	 strcat(command," ");
	 strcat(command,fd);  // simtest
	 strcat(command," >> /dev/null");
         e=system(command);
//	 printf("Funktionstest: %s\n",command);
	 e=funktionstest();
//	 if (e==1) printf("O.K.\n");
//	    else  printf("Fehler\n");
	 return e;   
  }

void test1()
  {
    int i,j,e;
    int maxteiler=maxmarginstep;
    double min=0.1;
    double max=3.0;
    double alpha,beta,teiler;
    ee=fopen(fe,"w+");
    char filename[250];
//----------------------      
    readfilename(filename,"xmgrmar");  
    char s,ss[20];
    FILE *ex,*ey;
    ex=fopen(filename,"w+");
    ey=fopen("marginout1.tagr","r");
    while ( (s=fgetc(ey))!=EOF )  fputc(s,ex);
    fclose(ey);
    double x1,x2,y1,y2,px1,px2;
//----------------------          
    
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    e=simu();
    int I=0;
    if (e==1)  // Funktion ohne Parametervariation
      {
      for (i=1; i<n; i++)
        if (parastatus[i]>0)
          { 
	     I++;
	     teiler=1.0;
             alpha=0.0;
             parawert[i]=min*parawert2[i];	     
             e=simu();          // Funktion an der unteren Grenze ??
	     if (e==0)
	      {
	       for (j=1;j<maxteiler;j++)
	         {
	           teiler=teiler/2.0;
                   parawert[i]=(1.0-(alpha+teiler)*(1.0-min))*parawert2[i];
    	           e=simu();
		//   printf("P1= %2.5lf %d\n",(1.0-(alpha+teiler)*(1.0-min)),e);
		   if (e==1) alpha+=teiler;
	         }	
	      }  else alpha=1.0;
             teiler=1.0;
             beta=0.0;
             parawert[i]=max*parawert2[i];	     	     
             e=simu();          // Funktion an der unteren Grenze ??
	     if (e==0)
	      {
  	       for (j=1;j<maxteiler;j++)
	        {
                   teiler=teiler/2.0;
		   parawert[i]=(1.0+(beta+teiler)*(max-1.0))*parawert2[i];
  	           e=simu();
		//   printf("P2= %2.5lf %d\n",(1.0+(beta+teiler)*(max-1.0)),e); 
		   if (e==1) beta+=teiler;
	        }
	      } else beta=1.0;		 
         printf("Parameter :'%c'   (%2.4lf,%2.4lf) -> (%2.4lf,%2.4lf)\n", 
          paraname[i],(1.0-alpha*(1-min)),(1.0+beta*(max-1)),
	      ((1.0-alpha*(1-min))*parawert2[i]),((1.0+beta*(max-1))*parawert2[i]));
         fprintf(ee,"Parameter :'%c'   (%2.4lf,%2.4lf) -> (%2.4lf,%2.4lf)\n", 
          paraname[i],(1.0-alpha*(1-min)),(1.0+beta*(max-1)),
	      ((1.0-alpha*(1-min))*parawert2[i]),((1.0+beta*(max-1))*parawert2[i]));
//----------------------          
  fprintf(ex,"@with box\n");
  fprintf(ex,"@    box on\n");
  fprintf(ex,"@    box loctype view\n");
  fprintf(ex,"@    box ");
  int N=parameteraktiv+1; 
  y1=0.145+0.7*((N-I+0.75)/N);
  y2=0.145+0.7*(double(N-I)/N);
  
  px1=(alpha*(1.0-min));
  px2=(beta*(max-1.0));
  if (px2>0.9) px2=0.9;
  x1=0.65-0.5*px1;
  x2=0.65+0.5*px2;
  
  fprintf(ex,"%lf,",x1);
  fprintf(ex,"%lf,",y1);  
  fprintf(ex,"%lf,",x2);
  fprintf(ex,"%lf\n",y2);  
  
  fprintf(ex,"@    box linestyle 1\n");
  fprintf(ex,"@    box linewidth 1.0\n");
  fprintf(ex,"@    box color 1\n");
  fprintf(ex,"@    box fill color 1\n");
  fprintf(ex,"@    box fill pattern 0\n");
  fprintf(ex,"@box def\n");

  fprintf(ex,"@with string\n");
  fprintf(ex,"@    string on\n");
  fprintf(ex,"@    string loctype view\n");
  fprintf(ex,"@    string 0.11, ");
  fprintf(ex,"%lf\n",y2+0.01);
  fprintf(ex,"@    string color 1\n");
  fprintf(ex,"@    string rot 0\n");
  fprintf(ex,"@    string font 0\n");
  fprintf(ex,"@    string just 4\n");
  fprintf(ex,"@    string char size 0.810000\n");

  strcpy(ss,paralabel[i]);
  fprintf(ex,"@    string def \"%s\"\n",ss);
  fprintf(ex,"@r0 off\n");

//----------------------      

             fflush(ee);   // Schreibpuffer leeren
	        parawert[i]=parawert2[i];
          }
	  
      }	  else printf("Keine Funktion mit Anfangswerten!\n");  
     fclose(ee); 
//----------------------                      

    ey=fopen("marginout2.tagr","r");
    while ( (s=fgetc(ey))!=EOF )  fputc(s,ex);
    fclose(ey);
    fclose(ex); 
//----------------------                           
  }
  
void test2()
  {
    int si=int(readvalue("initrand"));
    srand(si);
    int i,j,e,zi;
//    int maxtest=5000;
    double min=1.0-streuung/100.0;
    double max=1.0+streuung/100.0;
    double zufall,m;
    int count=0;
    ee=fopen(fe,"w+");
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    for (i=1; i<n; i++) paracenter[i]=0.0;
    for (j=1; j<=maxtest; j++)
      {
        for (i=1; i<n; i++) 
	 if (parastatus[i]>0)   // nur aktive Parameter aendern
          {
	    zi=rand();  zufall=zi/double(RAND_MAX);
	    	    
            m=min+zufall*(max-min);
            parawert[i]=m*parawert2[i];
	  }  
   //     for (i=1; i<n; i++) 
   //            printf("Parameter: '%c' = %3.5lf \n",paraname[i],parawert[i]);
	e=simu();
	if (e==1) // Funktion!
	  { 
             for (i=1; i<n; i++) paracenter[i]+=parawert[i];	 
	     count++;   
	  }
        if ((j % 20)==0)
	  {
            printf("----%d----%d----%3.2lf---\n",j,count,100.0*double(count)/j); 
            fprintf(ee,"----%d----%d----%3.2lf---\n",j,count,100.0*double(count)/j); 
            if (count>0)
	      
 	      for (i=1; i<n; i++) 
  	       if (parastatus[i]>0)   // nur aktive Parameter aendern	       
	       {
                 printf("Parameter:'%c' (%3.5lf) -> (%3.5lf)\n", 
                        paraname[i],parawert2[i],paracenter[i]/count);
                 fprintf(ee,"Parameter:'%c' (%3.5lf) -> (%3.5lf)\n", 
                        paraname[i],parawert2[i],paracenter[i]/count); 
                 fflush(ee);   // Schreibpuffer leeren
	       }	 
	  }
      }
    fclose(ee);   
  }

int parameterread()
  {
    char s[200];
    int i=1,aktiv=0;
    printf("Einlesen der Parameter\n");
    fscanf(ey,"%s",&s);   // ersten Parameter lesen
    while (!(feof(ey))) 
      {
	if ( (s[0]>='0' && s[0]<='9') ||    // Paramete fuer globale Skalierung
	         (s[0]>='a' && s[0]<='z') ||   
		     (s[0]>='A' && s[0]<='Z'))
	  {
  	    paraname[i]=s[0];
            fscanf(ey,"%s",&s); // Zahlenwert Parameter
	    parawert[i]=atof(s);
	    parawert2[i]=parawert[i];
	//    printf("%c  %lf \n",paraname[i],parawert[i]);
	    fscanf(ey,"%s",&s);  // Bezeichnung Parameter
	    strcpy(paralabel[i],s);
	    fscanf(ey,"%s",&s);  // Status Parameter
            parastatus[i]=atoi(s);	    
	    if (parastatus[i]>0) aktiv++;
//	    printf("->%d \n",parastatus[i]);
	    i++;
          }  
        fscanf(ey,"%s",&s);  // naechsten Parameter lesen
      }  
    n=i;
    parameteraktiv=aktiv;
    printf("%d Parameter eingelesen, davon %d aktiv. \n",i-1,aktiv);
    return 0;
  }
  
double normalverteilung(double x)
  {
    // x gleichverteilt auf (0,1)
    double a = 1.701;
    double y = -log((1-x)/x) / a;
    return y;
  }  
  
void test3()
  {
    // steunung wird als 1 sigma in % interpretiert
    FILE *ett;
    char filename[250];
    readfilename(filename,"yield");  
    double streuung;
    srand(1564443381);
    int k,i,j,e,zi;

    double zufall,m,geht;
    int count=0;
    ee=fopen(fe,"w+");
    ett=fopen(filename,"w+");
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    for (i=1; i<n; i++) paracenter[i]=0.0;
//    printf("log(2.7)=%lf\n",log(2.7));  %  ist log = ln ?
    
  double dy=maxyieldstreuung/maxyieldstep;
  for (k=1; k<=maxyieldstep; k++)
   {
    streuung=dy*k;
    for (j=1; j<=maxtest; j++)
      {
        for (i=1; i<n; i++) 
	 if (parastatus[i]>0)   // nur aktive Parameter aendern
          {
	    zi=rand();  zufall=normalverteilung( zi/double(RAND_MAX) );
	    	    	    
            m=1.0+zufall*streuung/100.0;
	    if (m<0.1)  m=0.1;
            if (m>3.0)  m=3.0;
            parawert[i]=m*parawert2[i];
	  }  
	e=simu();
	if (e==1) // Funktion!
	  { 
             for (i=1; i<n; i++) paracenter[i]+=parawert[i];	 
	     count++;   
	  }
        if ((j % 20)==0)
	  {
            geht=100.0*double(count)/j;     
            printf("----%d----%d----%3.2lf---\n",j,count,geht); 
            fprintf(ee,"----%d----%d----%3.2lf---\n",j,count,geht);
	  }
      }
     
      count=0;
      printf("< %3.1lf , %3.2lf >\n",streuung,geht);
      fprintf(ett," %3.1lf  %3.2lf\n",streuung,geht);      
      fflush(ett);
    } 
    fclose(ee);   
    fclose(ett);
  }  

void test4()
  {
    ee=fopen(fe,"w+");
    int i=0,j=0;
    int para1,para2;    
    double firstx,firsty;
    printf("Parameter bestimmen\n");
    do
    {                          // Nummer der aktiven Parameter suchen
      if (parastatus[i]>0)
        {
         if (para1c==paraname[i])   // 1. Parameter suchen
           {
	    para1=i;
	   } 
         if (para2c==paraname[i])   // 2. Parameter suchen
           {
	    para2=i;
	   }   
	 j++;    
       }   
      i++; 
     // cout << i << endl;
     } 
    while (j<parameteraktiv);
 //   printf( "aktive Parameter : ("<<para1c<<")=" << paralabel[para1]<< 
  //     " und ("<<para2c<<")=" << paralabel[para2]<< endl;   

    int e,R,NR=stepnumber;
    int maxteiler=maxmarginstep;
    double min=0.1;
    double max=1.9;
    double beta,teiler;
    double r1,r2,wx,wy;
    
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    e=simu();
    if (e==1)  // Funktion ohne Parametervariation
      {
        for (R=0; R<NR; R++)  //  Kreis ablaufen in 12 Schritten
	  {       
 	     r1=cos(2*Pi*R/NR);
             r2=sin(2*Pi*R/NR);
	     teiler=1.0;
             beta=0.0; 
  	     parawert[para1]=(1.0+r1*(beta+teiler)*(max-1.0))*parawert2[para1];
	     parawert[para2]=(1.0+r2*(beta+teiler)*(max-1.0))*parawert2[para2];
//	     cout << parawert[para1] << "," << parawert[para2] << endl;
             e=simu();          // Funktion an der unteren Grenze ??
	     if (e==0)
	      {
  	       for (j=1;j<maxteiler;j++)
	        {
                   teiler=teiler/2.0;
		   parawert[para1]=(1.0+r1*(beta+teiler)*(max-1.0))*parawert2[para1];
		   parawert[para2]=(1.0+r2*(beta+teiler)*(max-1.0))*parawert2[para2];
 	           e=simu();
		   if (e==1) beta+=teiler;
	        }
	      } else beta=1.0;		 
	  wx=(1.0+r1*(beta)*(max-1.0))*parawert2[para1];
	  wy=(1.0+r2*(beta)*(max-1.0))*parawert2[para2];
          printf("Winkel :'%i/%i' (%2.4lf)-> (%2.4lf,%2.4lf)\n", R,NR,
	      (1.0+beta*(max-1)),wx,wy );
          fprintf(ee,"%2.4lf %2.4lf\n",wx,wy);
	  if (R==0)
	    {
	      firstx=wx;
	      firsty=wy;
	    }
	  }    
          fprintf(ee,"%2.4lf %2.4lf\n",firstx,firsty); // resten Wert zum Schluss nochmal
        } else printf("Keine Funktion mit Anfangswerten!\n");  
  }


void test6()
  {
    ee=fopen(fe,"w+");
    int i=0,j=0;
    int para1;     // Parameter
    printf("Parameter bestimmen\n");
    do
    {                          // Nummer des aktiven Parameters suchen
      if (parastatus[i]>0)
        {
         if (para1c==paraname[i])   // 1. Parameter suchen
           {
	    para1=i;
	   } 
	 j++;    
       }   
      i++; 
     // cout << i << endl;
     } 
    while (j<parameteraktiv);
    printf("aktiver Parameter : %s = %lf\n",  paralabel[para1],parawert[para1]);   

    int e,R,NR=stepnumber;
    double min=1-maxmarginstep/100.0;
    double max=1+maxmarginstep/100.0;
    double delta;
    
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    e=simu();
        delta=(max-min)/stepnumber;
        for (R=0; R<NR; R++)  // 
	  {       
  	     parawert[para1]=(min+delta*R)*parawert2[para1];

//	     cout << parawert[para1]  << endl;
             e=simu();          // Funktionstest


//          printf("wx,wy ");
//          fprintf(ee,"%2.4lf %2.4lf\n",wx,wy);
	  }    
//    fprintf(ee,"%2.4lf %2.4lf\n",firstx,firsty); // resten Wert zum Schluss nochmal

  }


void test5()
  {
    // yield versus parameter
    FILE *ett;
    char filename[250];
    readfilename(filename,"yield");  
    double streuung,multi;
    srand(1564443281);
    int k,i,j,e,zi;

    double zufall,m,geht;
    int count=0;
    ee=fopen(fe,"w+");
    ett=fopen(filename,"w+");
    for (i=1; i<n; i++) parawert[i]=parawert2[i];
    for (i=1; i<n; i++) paracenter[i]=0.0;
//    printf("log(2.7)=%lf\n",log(2.7));  %  ist log = ln ?

    int para1;  
    printf("search for parameter...\n");
    i=0,j=0;
    do
    {                          // Nummer des aktiven Parameters suchen
      if (parastatus[i]>0)
        {
         if (para1c==paraname[i])   // 1. Parameter suchen
           {
	    para1=i;
	   } 
	 j++;    
       }   
      i++; 
     // cout << i << endl;
     } 
    while (j<parameteraktiv);
    printf("active parameter : %s = %lf\n",  paralabel[para1],parawert[para1]);   
    

  count=0;
  streuung=yield;
  for (k=0; k<=20; k++)
   {
    multi=1.0+5*(k-10.0)/100;
    for (j=1; j<=maxyieldstep; j++)
      {
        for (i=1; i<n; i++) 
	 if (parastatus[i]>0)   // nur aktive Parameter aendern
          {
	    zi=rand();  zufall=normalverteilung( zi/double(RAND_MAX) );
	    	    	    
            m=1.0+zufall*streuung/100.0;
	    if (m<0.1)  m=0.1;
            if (m>3.0)  m=3.0;
            parawert[i]=m*parawert2[i];
	    if (i==para1) parawert[i]*=multi;
	  }  
	e=simu();
	if (e==1)      count++;     
        geht=100.0*double(count)/j;     
        if ((j % 20)==0)
	  {
            printf("----%d----%d----%3.2lf---\n",j,count,geht); 
	  }
      }
     
      count=0;
      printf("< %5.2lf , %3.2lf >\n",parawert2[para1]*multi,geht);
      fprintf(ett," %5.2lf  %3.2lf\n",parawert2[para1]*multi,geht);      
      fflush(ett);
    } 
    fclose(ee);   
    fclose(ett);
  }  


int main (int argc, char *argv[])
  {
    int mode;
       
    printf("\nAutomatischer Parameterfile-Generator  v 1.91 12.03.2001\n");
    printf("ortlepp<at>rsfq.de-------------------------------------------\n");
    printf("modification 22.05.2008, 24.08.2009-----------------------\n");      
    if (argc>3)
      {
	strcpy(fconf,argv[1]);
        strcpy(fcc,argv[1]);    // conf-File
	strcat(fconf,f6);
	mode=atoi(argv[2]);
      }	
    else    
      {
        printf("How to call auto3: \n");      
        printf("auto3 configfile(.conf) <1..Margins|2..Monte Carlo|3..Yield|4..2D scan|5..yield vs. para> <para1> <para2> ..\n"); 
        printf("Mode 1: <maxteilung>   z.B. 8\n");      
        printf("Mode 2: <maxtest> <streuung>    z.B   1000 30\n");      
        printf("Mode 3: <maxtest> <maxstreuung> <schritte>  z.B.  500  30  10\n"); 
        printf("Mode 4: <para1> <para2> <schritte>  z.B.  A x 10 100\n"); 
	printf("Mode 5 (yield vs. parameter): <para> <steps> <spread>\n");
	exit(1);
      }	
    
    readfilename(fa,"paracir");      
    readfilename(fb,"parain");      
    readfilename(simcir,"simcir");
    readfilename(fd,"simtest");      
    readfilename(fe,"cirmar");
    readfilename(ff,"simout");
    I0RN=readvalue("I0RN");
    P2= I0RN;
    betac=readvalue("betac");
    P1= Phi0*betac/(2*Pi*P2*P2);    
      
    ey=fopen(fb,"r+");
    parameterread();    
    switch (mode) 
     {
      case 1:   maxmarginstep=atoi(argv[3]);
                printf("Verfeinerung in %d Schritten\n",maxmarginstep);    
                test1();  break;
      
      case 2:   maxtest=atoi(argv[3]);
              	streuung=atoi(argv[4]);
                printf("%d Tests mit +-%d Prozent Streuung\n",maxtest,streuung);    		
                test2();  break;
      
      case 3:   maxtest=atoi(argv[3]);
                maxyieldstreuung=atof(argv[4]);
		maxyieldstep=atoi(argv[5]);
                printf("Ausbeuteannalyse von sigma=0..%3.1lf in %d Schritte mit %d Berechnungen je Schritt\n"
		        ,maxyieldstreuung,maxyieldstep,maxtest);    		
                test3();  break;

      case 4:   printf("2D Margin Analyse \n");    		
                para1c=argv[3][0];  // erster Parameter
                para2c=argv[4][0];  // zweiter Parameter
		maxmarginstep=atoi(argv[5]);
		stepnumber=atoi(argv[6]);

                test4();  break;


      case 5:   para1c=argv[3][0];
                maxyieldstep=atoi(argv[4]);
		yield=atof(argv[5]);
                printf("Yield vs. Parameter: %c , yield: %3.1f , trails: %d\n"
		                   ,para1c,  yield, maxyieldstep);    		
                test5();  break;

      case 6:   printf("1D Energie Analyse TFF \n");    		
                para1c=argv[3][0];  // erster Parameter
		maxmarginstep=atoi(argv[4]);
		stepnumber=atoi(argv[5]);

                test5();  break;

		
      default:  printf("ungueltiger Betriebsmodues angegeben!\n");
                printf("Programmaufruf: \n");      
                printf("auto3 configfile(.conf) <1..Margins|2..Monte Carlo Schwerpunkt|3..Ausbeute> <para1> <para2> ..\n"); 
                printf("Modus 1: <maxteilung>   z.B. 8\n");      
                printf("Modus 2: <maxtest> <streuung>    z.B   1000 30\n");      
                printf("Modus 3: <maxtest> <maxstreuung> <schritte>  z.B.  500  30  10\n");                      
                printf("Modus 4: <para1> <para2> <schritte>  z.B.  A x 10 100\n"); 
     }	    

    fclose(ey);
    return 0;
  }



