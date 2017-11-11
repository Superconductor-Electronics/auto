//----------------------------------------------------
//    Thomas Ortlepp    30.05.2009 
//    Error function fit for BER and Gray zone
//    Version 1.2  28.01.2010 San Francisco
//----------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <fstream.h>
#include <string.h>

const int max=110000;

char fcc[250],fdata[50];
double strom[max],zeit[max],delay[max],lesen[max];
double xwert[max],ywert[max];
double sortdelay[max],sortread[max],mitte,vorzeichen=1.0;
const double Pi= 3.14159265358979;
const double sqrtpi=sqrt(Pi);
int   output[max],sortout[max];

int count;

// read all parameters
void readvalues()
  {
    char s[100];
    FILE *fd;
    double w1,w2,w3,w4;
    int test=0,c=0,e;
    fd=fopen(fdata,"r+");
    printf("Date file read: %s\n",fdata);
    do
        {
          fscanf(fd,"%s",s);   w1=atof(s);   // x-Wert
          e=fscanf(fd,"%s",s);   w2=atof(s);   // y-Wert

          e=fscanf(fd,"%s",s);      // dummy Wert
          e=fscanf(fd,"%s",s);      // dummy Wert	  
          e=fscanf(fd,"%s",s);      // dummy Wert	  

	  xwert[c]=w1;
	  ywert[c]=w2;
	  c++;
//	  printf("%d %lf %lf\n",c,w1,w2);
	}         
    while (e!=EOF);
    fclose(fd);
    count=c-2;
    printf("%d values scanned.\n",count);
    mitte=xwert[0];
//    if (ywert[0]<ywert[count]) vorzeichen=1.0;
    if (ywert[0]>ywert[count])     
      {
        vorzeichen=-1.0;
	for (e=0; e<=count; e++) ywert[e]=1.0-ywert[e];
      }
    c=0;
    do
      {
	c++;
        mitte=xwert[c];
      }
    while ((ywert[c]<0.5)&&(c<count));
    printf("First value: (x,y)=(%lf,%lf) \n",xwert[0],ywert[0]);
    printf("Middle  value: (x,y)=(%lf,%lf) \n",xwert[c],ywert[c]);    
    printf("Last  value: (x,y)=(%lf,%lf) \n",xwert[count],ywert[count]);
  }



//-----------------------------------------------
//------- Berechnung der error-Funktion ---------
//------- Thomas Ortlepp ------------------------
//------- 10.07.2006 ----------------------------
//--------max failure of y value is +-5e-7 ------
double erf(double x)
  {
    double y=-5.0,h,s=0.0;   
    int i,N=200;
    if ((x>-5.0) && (x<=5.0))
      {
        h=(x+5.0)/(2*N);
	s+=exp(-y*y);
	y+=h;
        for (i=0; i<N-1; i++)
          {
            s+=4.0*exp(-y*y)+2.0*exp(-(y+h)*(y+h));
	    y+=h+h;
          }	  
	s+=4.0*exp(-y*y)+exp(-(y+h)*(y+h));  
      }
    s=2.0*h*s/3.0/sqrt(Pi)-1.0;  
    if (x>5.0) s=1.0;  	  
    return s;     
  }
//-----------------------------------------------

void sort()
  {
     int i,j,index,t,zahl;
     int N=30;
     double w,h,dd,dx;
     double test,xsd,xmd;
     double mind,maxd;
     double mittel[200];
     int c;
     for (i=0; i<=count; i++)
       {
         sortdelay[i]=delay[i];
	 sortout[i]=output[i];
	 sortread[i]=lesen[i];
       }
     for (i=0; i<count-1; i++)
       {
         index=-1;
         w=sortdelay[i];
         for (j=i+1; j<count; j++)
  	   if (sortdelay[j]<w) {  index=j;  w=sortdelay[j];  }

	 if (index>-1)
	  {   	   
            w=sortdelay[index];
  	    sortdelay[index]=sortdelay[i];
   	    sortdelay[i]=w;

            h=sortread[index];
  	    sortread[index]=sortread[i];
   	    sortread[i]=h;
	    
	    t=sortout[index];
	    sortout[index]=sortout[i];
	    sortout[i]=t;
	  }  
       }	 
    mind=sortdelay[0]; maxd=sortdelay[count-1];
    printf("Values sorted, min=%lf, max=%lf \n",mind,maxd);        

//----------------------------

    FILE *fy;
    fy=fopen("sort-tmp-a.dat","w+");
    c=0;
    dx=(maxd-mind)/(N-1);
    dd=mind+dx;
    xmd=0.0;
    zahl=0;
    for (i=0; i<count; i++)
      {
         if (sortdelay[i]<dd) 
	   {
	     if (sortout[i]==1) 
	       {
      	         xmd+=sortread[i]; zahl++;
	       }
	    }     
	    else
	      {
	         mittel[c]=xmd/zahl;
		 xmd=0.0;
		 zahl=0;
		 dd+=dx;
		 c++;
	      }
      }
    xsd=0.0;  
    c=0;
    dd=mind+dx;
    for (i=0; i<count; i++)
      {
         if (sortdelay[i]<dd) 
	   {
	     if (sortout[i]==1) 
	       {
      	         xsd+=(sortread[i]-mittel[c])*(sortread[i]-mittel[c]); 
		 zahl++;
	       }
	    }     
	    else
	      {
	         fprintf(fy,"%lf %lf %lf %d\n",dd-dx/2.0, mittel[c], sqrt(xsd/(zahl-1)), zahl);         
		 xmd=0.0;
		 zahl=0;
		 dd+=dx;
		 c++;
	      }
      }

      
      
    fclose(fy);

//---------------------------------------

    FILE *fx;
    fx=fopen("sort-tmp.dat","w+");

     for (i=0; i<count; i++)
       {
//         printf("%lf %d\n",sortdelay[i],sortout[i]);
//         printf("%lf %d\n",delay[i],output[i]);
           if (sortout[i]==1)
	     {
               fprintf(fx,"%lf %lf\n",sortdelay[i],sortread[i]);
	     }  
       }
    fclose(fx);   

  }


void test_erf()
  {
    double x=-4.3;
    int e;
    for (e=1; e<25; e++)
      {
        printf("[%1.10lf , %1.15lf],\n",x,erf(x));
	x=x+0.38;
      }	
  }

double f(double x,double mu,double si)
  {
    return (0.5+0.5*erf(sqrtpi*(x-mu)/si));
  }
  
double error(double mu, double si)
  {
    int i;
    double e,h;
    e=0.0;
    for (i=0; i<count; i++)
      {
        h=f(xwert[i],mu,si)-ywert[i];
        e+=h*h;
      }          
    return e;         
  }
  
void opti( double mu, double si)
  {
    double A,A1,B,C,D1,D2;
    double mu0=mu,mu1,ok;
    double si0=si,si1;
    double h1=0.1;
    double h2=0.02;
    double H=1.05;
    double delay=0.0,eps=1e-9,delta;
    int k=0,M=0,NMAX=10000;
    char fe[250]="gray.data";    
    FILE *ee; 
    A=error(mu0,si0);
    do
      {
        B=error(mu0+h1,si0);    
        C=error(mu0,si0+h2); 
        D1=(B-A)/h1;
        D2=(C-A)/h2;     
        H=H*1.2;
	k=0;
        do
          {
	     H=H/1.2;
	     mu1=mu0-H*D1;
	     si1=si0-H*D2;
	     A1=error(mu1,si1);
	     ok=A1-A;
	     k++;
	     if (k>10) ok=-1;
  	  }
	while (ok==-1);  
        A=A1;
        mu0=mu1;
        si0=si1; 
        delta=fabs(H*D1)+fabs(H*D2);  
        printf("A= %lf    delta= %e   mu=%lf  si=%lf  H=%lf D1=%lf, D2=%lf\n",A,delta,mu0,si0, H, D1,D2);    
//        printf(".");
	M++;
      } 
    while ((delta>eps)&&(M<NMAX));
    printf("\nOptimization runs: %d\n",M);
    printf("mu= %lf,  si=%lf,  A=%lf\n",mu0,vorzeichen*si0,A);    
   
    ee=fopen(fe,"w+");    
    fprintf(ee,"%lf %lf %lf\n",mu0,vorzeichen*si0,A);    
    fclose(ee);   

  }  

void streu()
  {
     int i,j,k,c;
     double w,xstrom;
     double delay_m,xm;
     double sm;
     
     delay_m=0.0;
     xstrom=strom[0];
     c=0;
     k=0;
     for (i=0; i<=count; i++)
       {
         if (strom[i]==xstrom)
	   {
	     delay_m+=delay[i];
	     c++;
	   }
	 else  // Auswertung fuer diesen Wert
	   {
	     xm=delay_m/c;
	     sm=0.0;
	     for (j=k; j<i; j++)  sm+=(delay[j]-xm)*(delay[j]-xm);
	     sm=sqrt(sm/(c-1));	       
	     
	     k=i;
	     delay_m=delay[i];     

//	     printf("%d  Strom: %lf, delay_m: %lf, sig: %lf \n",c, xstrom,xm,sm);
//	     printf("%lf  %lf  %lf \n", xstrom,xm,sm);
	     c=1;	    
	     xstrom=strom[i];	     
	   }  
       }
  
  }



int main (int argc, char *argv[])
  {
    int e,mode;
    double x,y;
    printf("\n===========================================================\n");             
      printf("=== Error function fit   v 1.2  30.05.2009, 28.02.2010 ====\n");
      printf("=== Thomas Ortlepp, RSFQ desing group, Ilmenau ============\n");      
      printf("=== ortlepp@rsfq.de =======================================\n");      
      printf("===================================**=*====================\n");      
    if (argc==2)
      {
	strcpy(fdata,argv[1]);
//	mode=atoi(argv[2]);
      }	
    else    
      {
        printf("How to run: \n");      
        printf("errorfit file.dat\n");        
	exit(1);
      }	
      readvalues();  // Dat file x y

//    test_erf();

//      y=error(0.0,1.0);  printf("error(0,1)=%lf\n",y);
//      y=error(0.1,1.0);  printf("error(1.7,0.1)=%lf\n",y);    
//      y=error(2.6,0.4);  printf("error(2.6,0.4)=%lf\n",y);    
      opti(mitte,2);  


//      sort();
//      streu();

      
    return 0;
  }
  
/* Ausgabe der erf(x) Funktion
      for (e=0; e<600; e++)
        {
	  x=-6.0+e/50.0;
          y=erf(x);
	  printf("%lf %lf\n",x,y);
	}
*/
