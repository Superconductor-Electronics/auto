*===============================================
* Beispiel DFF5
* Thomas Ortlepp 08.10.2007
*=============================================

*=============================================
.SUBCKT DCSFQ  0  30  2

L10  30    3        0.1pH
L1   3     0        3.904pH
L2a  3     4        0.604pH
L2b  11    5        1.126pH
L3   5     7        4.484pH
L4   7     2        2.080pH

B1   11    4        jdcsfq1
RB1  4     51       1.111   
Lp1  51    11        1.037pH

B2   5     8        jdcsfq2
RB2  5     81       1.111   
Lp2  81    8        1.037pH
L6   8     0        0.198pH

B3   7      9       jdcsfq3
RB3  7     91       1.000 
Lp3  91     9       1.0pH
L7   9      0       0.110pH

ib1  0     11       pwl (0 0 5p 275.000uA 100n 275.000uA)   
ib2  0     7       pwl (0 0 5p 175.000uA 100n 175.000uA)   

.MODEL jdcsfq1 JJ(RTYPE=0, ICRIT= 225.000uA CAP= 1.132PF RN=90)
.MODEL jdcsfq2 JJ(RTYPE=0, ICRIT= 225.000uA CAP= 1.132PF RN=90)
.MODEL jdcsfq3 JJ(RTYPE=0, ICRIT= 250.000uA CAP= 1.262PF RN=90)
.ENDS
*=========================================
*  JTL2c  JeSEF Technology 
*  27.05.2004 Thomas Ortlepp
*  01.07.2004 resistor shift
*  15.11.2005 Rel-O 
*=========================================
.SUBCKT JTL  0  1  2

L1  1      4        2.082pH
L2  4      8        2.060pH
L3  8      5        2.060pH
L4  5      2        2.082pH

B1   4      6       jjtl1
RB1  4      9       1.02  
Lp1  9      6       1.0pH
L6   6      0       0.214pH

B2   5      7       jjtl2
RB2  5     10       1.02  
Lp2 10      7       1.0pH
L7   7      0       0.214pH

* Bias source 2.5mV and 7.14 Ohm
* ib1  0     8        pwl (0 0 5p 350uA 100n 350uA)   

rvb1 100 101      7.14  
lvb1 101   8      10pH
vb1  100   0      pwl (0 0 5p @X mV 100ns @X mV)   
 
.MODEL jjtl1 JJ(RTYPE=0, ICRIT= 250uA CAP= 1.32PF RN=90)
.MODEL jjtl2 JJ(RTYPE=0, ICRIT= 250uA CAP= 1.32PF RN=90)
.ENDS
*========================================


*========================================
* DFF with 7 junctions
* Thomas Ortlepp 
* 8.10.2007
*========================================
.SUBCKT DFF   0    1    11    8
*            GND  D_IN  CLK  D_OUT

L1   1     2         @A pH
L2a  2     23        @B pH
L2b  23    3         @I pH
L3   4     5         @C pH
L4   5     6         @D pH
L5a  6     67        @E pH
L5b  67    7         @J pH
L6   7     8         @F pH
L7   9     10        @G pH
L8   11    10        @H pH

B1   2     16        jjtr1
RB1  2     101       @ar  
LB1  101   16        1.0pH
Lp1  16    0         0.22pH

B2   3     4         jjtr2
RB2  3     201       @br   
LB2  201   4         0.977pH

B3   4     17        jjtr3
RB3  4     301       @cr  
LB3  301   17        1.093pH
Lp3  17    0         0.049pH

B4   6     18        jjtr4
RB4  6     401       @dr   
LB4  401   18        1.093pH
Lp4  18    0         0.181pH

B5   9     6         jjtr5
RB5  9     501       @er   
LB5  501   6         1.235pH

B6   10    20        jjtr6
RB6  10    601       @fr  
LB6  601   20        1.0pH
Lp6  20    0         0.115pH

B7   7     19        jjtr7
RB7  7     701       @gr  
LB7  701   19        1.0pH
Lp7  19     0        0.155pH

*ib1  0     23        pwl (0 0 5p 260.000uA 100n 260.000uA)   
*ib2  0     5         pwl (0 0 5p 96.000uA 100n 96.000uA)   
*ib3  0     67        pwl (0 0 5p 163.000uA 100n 163.000uA)   
*ib4  0     10        pwl (0 0 5p 143.000uA 100n 143.000uA)   

rvb1 100 191      @u 
lvb1 191   23      10pH
rvb2 100 291      @v 
lvb2 291   5      10pH
rvb3 100 391      @w  
lvb3 391   67      10pH
rvb4 100 491      @x 
lvb4 491   10      10pH

vb1  100   0      pwl (0 0 5p @X mV 100ns @X mV)   


.MODEL jjtr1 JJ(RTYPE=1, ICRIT= @a uA CAP= @ac PF RN=90)
.MODEL jjtr2 JJ(RTYPE=1, ICRIT= @b uA CAP= @bc PF RN=90)
.MODEL jjtr3 JJ(RTYPE=1, ICRIT= @c uA CAP= @cc PF RN=90)
.MODEL jjtr4 JJ(RTYPE=1, ICRIT= @d uA CAP= @dc PF RN=90)
.MODEL jjtr5 JJ(RTYPE=1, ICRIT= @e uA CAP= @ec PF RN=90)
.MODEL jjtr6 JJ(RTYPE=1, ICRIT= @f uA CAP= @fc PF RN=90)
.MODEL jjtr7 JJ(RTYPE=1, ICRIT= @g uA CAP= @gc PF RN=90)

.ENDS

*========================================

XDCSFQIN   DCSFQ 0   1000  100
XJTL1      JTL   0   100   200
XDCSFQCLK  DCSFQ 0   2000  300
XJTL2      JTL   0   300   400
XJTL3      JTL   0   400   500
XDFF       DFF   0   200   500   600
XJTL4      JTL   0   600   700
XJTL5      JTL   0   700   800 
R100   800   0   2.0


*=== Testmuster fuer 25 GHz ==============
iib1a  0  1000       pulse( 0 600uA 100p 5p 5p 20p 800p)
iib1b  0  1000       pulse( 0 600uA 300p 5p 5p 20p 800p)
iib1c  0  1000       pulse( 0 600uA 700p 5p 5p 20p 800p)

iib2a  0  2000       pulse( 0 600uA 500p 5p 5p 20p 800p)
iib2b  0  2000       pulse( 0 600uA 600p 5p 5p 20p 800p)
iib2c  0  2000       pulse( 0 600uA 700p 5p 5p 20p 800p)


*============================= Ausgabe fuer Simulationsbild
*.PRINT DEVV XJTL1_B1
*.PRINT DEVV XJTL2_B1
*.PRINT DEVV XJTL4_B1

*============================= Ausgabe fuer Test
.PRINT PHASE XJTL1_B1
.PRINT PHASE XJTL2_B1
.PRINT PHASE XDFF_B1
.PRINT PHASE XDFF_B2
.PRINT PHASE XDFF_B3
.PRINT PHASE XDFF_B4
.PRINT PHASE XDFF_B5
.PRINT PHASE XDFF_B6
.PRINT PHASE XDFF_B7
.PRINT PHASE XJTL4_B1


.tran 1ps 801ps 0.0ps 0.5ps
.end

