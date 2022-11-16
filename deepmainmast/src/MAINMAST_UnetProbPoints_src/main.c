/*
caldep + fragment + sphere
*/
//#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <omp.h>
#include "struct.h"
#include "func.h"
#include "mrc.h"
#include "thread.h"
#include "rmsd.h"

//#include "scoring.h"

#define PDB_STRLEN 55

//Fragment

typedef struct{
 NODE Ca[200];
 int Start,End,Len;
 float RawScore,Score,Rmsd;
} RESULT_MODEL;

void malloc_error(char *a){
 fprintf(stderr,"malloc error in %s\n",a);
 exit(0);
}
double gettimeofday_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

int readlist(char *fname,char **list){
 int num=0;
 FILE *fp;
 int len;
 if((fp=fopen(fname,"r"))==NULL)
  return FALSE;
 while(fgets(list[num],LIN,fp)!=NULL){
  len=strlen(list[num]);
  list[num][len-1]='\0';//ignore terminal \n
  num++;
 }
 fclose(fp);
 return TRUE;
}

int line_num(char *fname){
 int num=0;
 FILE *fp;
 char line[LIN];
 if((fp=fopen(fname,"r"))==NULL)
  return FALSE;
 while(fgets(line,LIN,fp)!=NULL){
  num++;
 }
 fclose(fp);
 return num;
}


void CompareFragAmodel(SEQ_NODE *,SEQFG*,int,NODE *,bool);
bool CompareFragAmodelMore(SEQ_NODE *,SEQFG*,int,NODE *,bool);
bool MakeDummyFrag(SEQ_NODE *,SEQ_NODE *,SEQFG*,int,bool);
int AddModel(RESULT_MODEL *,RESULT_MODEL **, int);
int AddConnectModel(RESULT_MODEL **, int,int,SEQ *);
void TableShow(GRAPH *, SEQ *,MRC *,float);
float Dist2(NODE *,NODE *);
float CheckConnect(NODE *,NODE *,MRC *,float,float *);

CMD cmd;

int main(int argc, char **argv)
{
 double t1=gettimeofday_sec();
 double t4;
 POINTS pt;
 MRC mrc,pmrc,ori_mrc,bb_mrc;
 GRAPH g;
 TREE mst;
 PTBL ptbl;

 //Get Options
 if(chkcmdline(argc,argv,&cmd)==FALSE)
  return(0);

 //Set threads
 if(cmd.Nthr < omp_get_num_procs()){
  omp_set_num_threads(cmd.Nthr);
 }else{
  omp_set_num_threads(omp_get_num_procs());
 }

 //Loading U-net outputs=============================
 //AAs
 MRC *AA_mrc,*ATOM_mrc;

 AA_mrc=(MRC *)malloc(sizeof(MRC)*20);
 ATOM_mrc=(MRC *)malloc(sizeof(MRC)*6);

 char mapfile[1000];
 struct stat buf;
 for(int aa=0;aa<20;aa++){
	if(cmd.DummyMode==true)
	 sprintf(mapfile,"%s/%s%s.mrc",cmd.dirname,"sigmoidAA_",RES_NAMES[aa]);
	else
	 sprintf(mapfile,"%s/%s%s.mrc",cmd.dirname,"AA_",RES_NAMES[aa]);
	printf("##Loading MAP: %s\n",mapfile);
	if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
  if(readmrc(&AA_mrc[aa],mapfile))
   return(0);
 }

 //ATOMs========
 //Others
 sprintf(mapfile,"%s/atom_Others.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[0],mapfile))
   return(0);
 //N
 sprintf(mapfile,"%s/atom_N.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[1],mapfile))
   return(0);
 //CA
 sprintf(mapfile,"%s/atom_CA.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[2],mapfile))
   return(0);
 //C
 sprintf(mapfile,"%s/atom_C.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[3],mapfile))
   return(0);
 //O
 sprintf(mapfile,"%s/atom_O.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[4],mapfile))
   return(0);
 //O
 sprintf(mapfile,"%s/atom_CB.mrc",cmd.dirname);
 if(stat(mapfile,&buf)){ printf("##ERROR MAP: %s\n",mapfile); return 0; }
 if(readmrc(&ATOM_mrc[5],mapfile))
   return(0);


 //Sequence
 SEQ seq;
 if(readseq(&seq,cmd.sfilename)){
  puts("Error in seq file");
  return 0;
 }
 //Backbone Prob
 //N
 CopyMrc(&ATOM_mrc[1],&bb_mrc);
 //Add CA
 for(int i=0;i<bb_mrc.NumVoxels;i++)
  bb_mrc.dens[i]+=ATOM_mrc[2].dens[i];
 //Add C
 for(int i=0;i<bb_mrc.NumVoxels;i++)
  bb_mrc.dens[i]+=ATOM_mrc[3].dens[i];

 //Mean Shifting by CA
 if(meanshift(&ATOM_mrc[2],&pt,cmd.map_t))
  return(0);

 if(MergePoints(&ATOM_mrc[2],&pt))
  return(0);

 //Volume and density data
 AssignUnetProbToNode(ATOM_mrc,AA_mrc,&pt,&g);

 TableShow(&g,&seq, &bb_mrc,cmd.Pcut);

 t4=gettimeofday_sec();
 printf("#FINISHED TOTAL TIME= %f\n",t4-t1);
 return 0;

}



void TableShow(GRAPH *g,SEQ *s,MRC *mrc,float Pcut){

 int AAcd;
 float AAsco,ATOMsco,SSsco;
 float Ph,Pe,Pc,d;

 NODE *TBL[10000];//10k points
 int Npoints=0;

 //Show coordinates in PDB format
 for(int i=0;i<g->Nnode;i++){
  if(g->node[i].ATOMP[1] + g->node[i].ATOMP[2] + g->node[i].ATOMP[3] < Pcut){
   continue;
  }
  printf("ATOM  %5d  CA  %3s%6d    ",Npoints+1,RES_NAMES[0],Npoints+1);
  printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",g->node[i].real_cd[0],g->node[i].real_cd[1],g->node[i].real_cd[2],1.0,1.0);

  printf("NODE  %5d %8.3f %8.3f %8.3f\n",Npoints+1,g->node[i].real_cd[0],g->node[i].real_cd[1],g->node[i].real_cd[2]);

  TBL[Npoints]=&(g->node[i]);
  Npoints++;
 }
 printf("Npoints= %d Nseq= %d\n",Npoints,s->len);
 //Show SEQ
 for(int p=0;p<s->len;p++){
  printf("SEQ %d %d %s\n",1+p,s->chain_id[p],RES_NAMES[s->seq_code[p]]);
 }


 //Distance Matrix = Edge data
 int STOCK[2000];
 int Nstock=0;
 float ave=0.0;
 for(int i=0;i<Npoints;i++){
 	for(int j=i+1;j<Npoints;j++){
	 d=Dist2(TBL[i],TBL[j]);
	 d=sqrt(d);
	 if(d>10.0){ //Too far > 10A
	  printf("DMTX %d %d %.0f %.0f %.0f\n",i+1,j+1,d*10.0,0.00, 0.00);//Int Format*10
	  continue;
	 }
	 //Check Connection
	 float MinP = CheckConnect(TBL[i],TBL[j],mrc,Pcut,&ave);
	 printf("DMTX %d %d %.0f %.0f %.0f\n",i+1,j+1,d*10.0,MinP*100,ave*100);//Int Format*10
	}
 }

 //Sequence Node and LDP Node
 for(int p=0;p<s->len;p++){//Seq
 	for(int i=0;i<Npoints;i++){//Node
	 AAcd=s->seq_code[p];
	 Ph=s->Pss[p][0];
	 Pe=s->Pss[p][1];
	 Pc=s->Pss[p][2];

  	 AAsco=TBL[i]->LogAA[AAcd];
	 ATOMsco=TBL[i]->LogATOM[2];//Other,N,CA,C,O,CB
/*
	 SSsco=Ph*TBL[i]->LogSS[0];
	     +Ph*TBL[i]->LogSS[1];
	     +Pc*TBL[i]->LogSS[2];
*/
  	 //printf("SCO %d %d %f\n",p+Npoints+1,i+1,AAsco+ATOMsco+SSsco);
	 //SEQ, NODE
  	 //printf("SCO %d %d %f %f %f\n",p+1,i+1,ATOMsco,AAsco,SSsco);
  	 printf("SCO %d %d %f %f AA= %d %3s\n",p+1,i+1,ATOMsco,AAsco,AAcd,RES_NAMES[s->seq_code[p]]);
 	}
 }
}


//Compare with AF2 model
void CompareFragAmodel(SEQ_NODE *result,SEQFG *sfg,int Nsfg,NODE *mod,bool mode){
 for(int s=0;s<Nsfg;s++){
  int N=result[s].Nali;
  int posi=sfg[s].pos;
  double cd1[100][3],cd2[100][3];
  double mtx[3][3],mov_com[3],mov_ref[3];
  int Ncd;
  printf("N= %d\n",N);
  if(N==0)
   continue;
  SEQ_NODE *res=&result[s];
	for(int i=0;i<N;i++){
	 //copy coordinates
	 Ncd=0;
	 for(int j=0;j<sfg[s].l;j++){
	  if(res->ali[i][j]==NULL)
	   continue;

	   cd1[Ncd][0]=res->ali[i][j]->real_cd[0];
	   cd1[Ncd][1]=res->ali[i][j]->real_cd[1];
	   cd1[Ncd][2]=res->ali[i][j]->real_cd[2];

	   cd2[Ncd][0]=mod[posi+j].real_cd[0];
	   cd2[Ncd][1]=mod[posi+j].real_cd[1];
	   cd2[Ncd][2]=mod[posi+j].real_cd[2];

	  Ncd++;

	 }
	 if(Ncd<4){//too short
	  res->rmsd[i]=100;
	  continue;
	 }
	 //check rmsd
	 double rmsd=0;
	 fast_rmsd(cd1,cd2,Ncd,&rmsd);
	 //printf("%d %d RMSD= %f\n",s,i,rmsd);
	 res->rmsd[i]=rmsd;
	 if(!isfinite(rmsd))
	  res->rmsd[i]=100;


	 if(mode==true && res->rmsd[i]<cmd.RMSD && res->score[i] >= cmd.zcut){
	  float rotated[3],tmp[3];
		Ncd=0;
		for(int j=0;j<sfg[s].l;j++){
	 	 if(res->ali[i][j]==NULL)
	 	  continue;
	 	 cd1[Ncd][0]=res->ali[i][j]->real_cd[0];
	   	 cd1[Ncd][1]=res->ali[i][j]->real_cd[1];
	  	 cd1[Ncd][2]=res->ali[i][j]->real_cd[2];
	  	 cd2[Ncd][0]=mod[posi+j].real_cd[0];
	  	 cd2[Ncd][1]=mod[posi+j].real_cd[1];
	  	 cd2[Ncd][2]=mod[posi+j].real_cd[2];
	  	 Ncd++;
	 	}
	  	calculate_rotation_rmsd(cd1,cd2,Ncd,mov_com,mov_ref,mtx,&rmsd);
		//printf("com %f %f %f\n",mov_com[0],mov_com[1],mov_com[2]);
		//printf("ref %f %f %f\n",mov_ref[0],mov_ref[1],mov_ref[2]);
	 	printf("##POS %d RAWSCO= %f ZSCORE= %f RMSD= %f Amodel\n",sfg[s].pos,res->raw_score[i],res->score[i],res->rmsd[i]);
	 	puts("MODEL");
		//move rotate and extend
		for(int j=0;j<sfg[s].l;j++){
		 if(res->ali[i][j]==NULL)
	 	  continue;
		  //mov -> o
		  tmp[0]=mod[posi+j].real_cd[0]-mov_com[0];
		  tmp[1]=mod[posi+j].real_cd[1]-mov_com[1];
		  tmp[2]=mod[posi+j].real_cd[2]-mov_com[2];
			for(int i1=0;i1<3;i1++){
				rotated[i1]=0.0;
				for(int i2=0;i2<3;i2++)
				 rotated[i1] +=mtx[i1][i2]*tmp[i2];
			}
		  //o -> mov -> ref
		  tmp[0]=rotated[0]+mov_com[0]+mov_ref[0];
		  tmp[1]=rotated[1]+mov_com[1]+mov_ref[1];
		  tmp[2]=rotated[2]+mov_com[2]+mov_ref[2];
		 printf("ATOM  %5d  CA  %3s%6d    ",j+1,RES_NAMES[sfg[s].seq[j]],sfg[s].pos+j+1);
  		 printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",tmp[0],tmp[1],tmp[2],1.0,res->score[i]);
		}
 	 	puts("ENDMDL");
	 }
	}
 }
}

int CheckOverlap(int st1,int ed1,int st2,int ed2){
/*
     st1------ed1
st2-----------------ed2
or
     st1-----------------ed1
st2-----------------ed2
*/
 if(st2<=st1 && st1<ed2)
  return -1;
/*
st1---------------ed1
     st2------ed2
or
st1--------------ed1
     st2-----------------ed2
*/
 if(st1<=st2 && st2<ed1)
  return -1;

/*
st1----ed1
            st2------ed2
*/
 if(ed1<=st2)
  return 0;
/*
             st1----ed1
st2------ed2
*/
 if(ed2<=st1)
  return 1;
 return -1;
}

bool CheckGeo(RESULT_MODEL *f1,RESULT_MODEL *f2){

 //Check N and C-ter
 //N-ter
 int Cter=f1->Len-1;
 float d2;
 for(int p=0;p<f2->Len;p++){
	 d2=(f2->Ca[p].real_cd[0]-f1->Ca[0].real_cd[0])
	   *(f2->Ca[p].real_cd[0]-f1->Ca[0].real_cd[0])
	   +(f2->Ca[p].real_cd[1]-f1->Ca[0].real_cd[1])
	   *(f2->Ca[p].real_cd[1]-f1->Ca[0].real_cd[1])
	   +(f2->Ca[p].real_cd[2]-f1->Ca[0].real_cd[2])
	   *(f2->Ca[p].real_cd[2]-f1->Ca[0].real_cd[2]);
	if(d2 <1.0){
	 //printf("Nter-connect\n");
	 return true;
	}
	d2=(f2->Ca[p].real_cd[0]-f1->Ca[Cter].real_cd[0])
	   *(f2->Ca[p].real_cd[0]-f1->Ca[Cter].real_cd[0])
	   +(f2->Ca[p].real_cd[1]-f1->Ca[Cter].real_cd[1])
	   *(f2->Ca[p].real_cd[1]-f1->Ca[Cter].real_cd[1])
	   +(f2->Ca[p].real_cd[2]-f1->Ca[Cter].real_cd[2])
	   *(f2->Ca[p].real_cd[2]-f1->Ca[Cter].real_cd[2]);

	if(d2<1.0){
	 //printf("Cter-connect\n");
	 return true;
	}

 }
 return false;
}


float Dist2(NODE *Ca1,NODE *Ca2){
	float d2=(Ca1->real_cd[0]-Ca2->real_cd[0])
		*(Ca1->real_cd[0]-Ca2->real_cd[0])
		+(Ca1->real_cd[1]-Ca2->real_cd[1])
		*(Ca1->real_cd[1]-Ca2->real_cd[1])
		+(Ca1->real_cd[2]-Ca2->real_cd[2])
		*(Ca1->real_cd[2]-Ca2->real_cd[2]);

 return d2;
}

float CheckConnect(NODE *Ca1,NODE *Ca2, MRC *mrc,float Pcut, float *ave){

 //vec Ca1->Ca2
 float vec[3],pos[3];
 int Ipos[3];
 int xydim=mrc->xdim*mrc->ydim;
 int xdim = mrc->xdim;
 int x,y,z,ind;
 vec[0]=Ca2->real_cd[0]-Ca1->real_cd[0];
 vec[1]=Ca2->real_cd[1]-Ca1->real_cd[1];
 vec[2]=Ca2->real_cd[2]-Ca1->real_cd[2];

 if(Ca1 == Ca2){ //Single point
  pos[0]=Ca1->real_cd[0];
  pos[1]=Ca1->real_cd[1];
  pos[2]=Ca1->real_cd[2];

  x=(int)round((pos[0]-mrc->orgxyz[0])/mrc->widthx);
  y=(int)round((pos[1]-mrc->orgxyz[1])/mrc->widthx);
  z=(int)round((pos[2]-mrc->orgxyz[2])/mrc->widthx);
  ind=xydim*z+xdim*y+x;

  printf("%d %d %d dens= %f\n",x,y,z,mrc->dens[ind]);
  if(mrc->dens[ind]< Pcut)
   return true;//No-connection

  return false;
 }
 //Sample 10 points
 float MinD = 1000000.000;
 float SumD = 0.00;
 for(int i=1;i<10;i++){ //1-9
  pos[0]=Ca1->real_cd[0] + 0.10*(i)*vec[0];
  pos[1]=Ca1->real_cd[1] + 0.10*(i)*vec[1];
  pos[2]=Ca1->real_cd[2] + 0.10*(i)*vec[2];

  //check density value
  //g->node[i].real_cd[0]=pt->cd[i][0]*mrc->widthx+mrc->orgxyz[0];
  x=(int)round((pos[0]-mrc->orgxyz[0])/mrc->widthx);
  y=(int)round((pos[1]-mrc->orgxyz[1])/mrc->widthx);
  z=(int)round((pos[2]-mrc->orgxyz[2])/mrc->widthx);
  
  ind=xydim*(z)+xdim*(y)+(x);
  SumD+=mrc->dens[ind];
  if(mrc->dens[ind] < MinD)
   MinD = mrc->dens[ind];
 }
 //printf("MinD= %f\n",MinD);
 //return MinD/27.00;
 *ave = SumD/9.00;
 return MinD;
}


bool Connect(RESULT_MODEL *f1,RESULT_MODEL *f2,RESULT_MODEL *f3,SEQ *seq,bool show){
 int Npoint,Cpoint;
 float d2,D2;
 float d_ng2 = 3.0*3.0;
 Npoint=-1;
 Cpoint=-1;
 //check overlap between f1 and f2
 for(int i=0;i<f1->Len;i++){
  for(int j=0;j<f2->Len;j++){
   d2=Dist2(&(f1->Ca[i]),&(f2->Ca[j]));
   if(d2 < d_ng2)
    return false;
  }
 }
 for(int i=0;i<f3->Len;i++){
  d2=Dist2(&(f1->Ca[f1->Len-1]),&(f3->Ca[i]));
  D2=Dist2(&(f2->Ca[0]),&(f3->Ca[i]));
  if(d2<=1.0)
   Npoint=i;
  if(D2<=1.0)
   Cpoint=i;
 }
 if(Npoint <0||Cpoint<0||Npoint==Cpoint)
  return false;

 //check Nca
 int Lmiss=f2->Start - f1->End+1;
 int Lfill = Cpoint - Npoint;
 if(Lfill<0)
  Lfill*=-1;
 if(Lfill != Lmiss)
  return false;

 //Keep insert coords
 NODE ins[30];
 int pp=0;
 int Lins=0;
 if(Npoint<Cpoint){
  for(int p=Npoint+1;p<Cpoint;p++){
   ins[pp].real_cd[0]=f3->Ca[p].real_cd[0];
   ins[pp].real_cd[1]=f3->Ca[p].real_cd[1];
   ins[pp].real_cd[2]=f3->Ca[p].real_cd[2];
   pp++;
  }
 }else{
  for(int p=Npoint-1;p>Cpoint;p--){
   ins[pp].real_cd[0]=f3->Ca[p].real_cd[0];
   ins[pp].real_cd[1]=f3->Ca[p].real_cd[1];
   ins[pp].real_cd[2]=f3->Ca[p].real_cd[2];
   pp++;
  }
 }
 Lins=pp;
 //Check Clash between ins and f1,f2
 for(int i=0;i<Lins;i++){
  for(int j=0;j<f1->Len;j++){
   d2=Dist2(&(ins[i]),&(f1->Ca[j]));
   if(d2 < d_ng2)
    return false;
  }
  for(int j=0;j<f2->Len;j++){
   d2=Dist2(&(ins[i]),&(f2->Ca[j]));
   if(d2 < d_ng2)
    return false;
  }
 }

 //printf("Find!! %d-%d\n",Npoint,Cpoint);
 //Show PDB format 
 if(show==true){
  printf("f1 %d-%d\n",f1->Start,f1->End);
  printf("f2 %d-%d\n",f2->Start,f2->End);
  printf("In %d-%d\n",Npoint,Cpoint);
  printf("##POS %d RAWSCO= %f ZSCORE= %f RMSD= %f Extended\n",
  f1->Start,f1->RawScore+f2->RawScore,f1->Score+f2->Score,0.00);
  puts("MODEL");
  int Natm,Nres;
  Natm=1;
  Nres=f1->Start;
	for(int p=0;p<f1->Len;p++){
	 printf("ATOM  %5d  CA  %3s%6d    ",Natm++,RES_NAMES[f1->Ca[p].AAtype],Nres+1+p);
  	 printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",f1->Ca[p].real_cd[0],f1->Ca[p].real_cd[1],f1->Ca[p].real_cd[2],1.0,f1->Score);
	}
	//Insert
	for(int p=0;p<Lins;p++){
	 printf("ATOM  %5d  CA  %3s%6d    ",Natm++,RES_NAMES[seq->seq_code[f1->End+p]],f1->End+1+p);
  	 printf("%8.3f%8.3f%8.3f%6.2f%6.2f X\n",ins[p].real_cd[0],ins[p].real_cd[1],ins[p].real_cd[2],1.0,0.0);
	}
	//f2
  	Nres=f2->Start;
	for(int p=0;p<f2->Len;p++){
	 printf("ATOM  %5d  CA  %3s%6d    ",Natm++,RES_NAMES[f2->Ca[p].AAtype],Nres+1+p);
  	 printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",f2->Ca[p].real_cd[0],f2->Ca[p].real_cd[1],f2->Ca[p].real_cd[2],1.0,f2->Score);
	}
  puts("ENDMDL");

 }
 return true;
}



bool FindConnects(RESULT_MODEL *f1,RESULT_MODEL **f1_tbl,int n1,RESULT_MODEL *f2,RESULT_MODEL **f2_tbl,int n2,SEQ *seq,bool show){

 RESULT_MODEL *F1,*F2;
 RESULT_MODEL **F1_TBL,**F2_TBL;
 int N1,N2,Lmiss;
 if(f1->End <=f2->Start){
  F1=f1; F1_TBL=f1_tbl; N1=n1;
  F2=f2; F2_TBL=f2_tbl; N2=n2;
 }else{
  F2=f1;
  F2_TBL=f1_tbl;
  N2=n1;
  F1=f2;
  F1_TBL=f2_tbl;
  N1=n2;
 }
 Lmiss=F2->Start-F1->End+1; 
 if(Lmiss>20)
  return false;
 float Dmiss=Lmiss*3.70;
 float Dmiss2=Dmiss*Dmiss;
 float d2=Dist2(&(F1->Ca[F1->Len-1]),&(F2->Ca[0]));

 if(d2 > Dmiss2||d2==0.0)//Too far or overlap
  return false;

 for(int i=0;i<N1;i++){
  if(F1_TBL[i]->Len <Lmiss)
   continue;
  bool chk=false;
	for(int j=0;j<N2;j++){
	 if(F1_TBL[i]==F2_TBL[j]){
 	  chk=true;
	  break;
	 }
	}
  if(chk==false)
   continue;
  //printf("Check..%f\n",F1_TBL[i]->Score);
  if(Connect(F1,F2,F1_TBL[i],seq,show)==true){
	if(show==true){
   	 printf("#Connect....%d %d L=%d ",N1,N2,Lmiss);
   	 printf("%d-%d %d-%d %f : %f\n",F1->Start,F1->End,F2->Start, F2->End,d2,Dmiss2);
	}
   return true;
   break;
  }
 


 }
 return false;

}

int AddConnectModel(RESULT_MODEL **all, int N,int Ntot,SEQ *seq){
 puts("Start COmpare....");
 int Num=N;
 bool DisSim=false;
 bool FindSimilar=false;
 float d2=0;
 float d2cut=1.00;
 RESULT_MODEL *f1,*f2,*f3;
 unsigned int Nchk=0;

 RESULT_MODEL ***tmp_tbl;

 int *Ntbl;
 if((Ntbl=(int *)calloc(N,sizeof(int)))==NULL)
  return -1;
 if((tmp_tbl=(RESULT_MODEL ***)malloc(sizeof(RESULT_MODEL **)*N))==NULL)
  return -1;
 for(int i=0;i<N;i++)
  if((tmp_tbl[i]=(RESULT_MODEL **)malloc(sizeof(RESULT_MODEL *)*N))==NULL)
   return -1;

 int *NbestID,*CbestID;
 if((NbestID=(int *)malloc(sizeof(int)*N))==NULL)
  return -1;
 if((CbestID=(int*)malloc(sizeof(int)*N))==NULL)
  return -1;



 //select two fragments
 puts("Checking Connection....");
 #pragma omp parallel for schedule(dynamic,10)
 for(int fgid1=0;fgid1<Num;fgid1++){
  //for(int fgid2=fgid1+1;fgid2<Num;fgid2++){
  for(int fgid2=0;fgid2<Num;fgid2++){
	if(fgid1==fgid2)
	 continue;
	if(CheckGeo(all[fgid1],all[fgid2])==false)
	 continue;
	tmp_tbl[fgid1][Ntbl[fgid1]]=all[fgid2];
	Ntbl[fgid1]++;
	//tmp_tbl[fgid2][Ntbl[fgid2]]=all[fgid1];
	//Ntbl[fgid2]++;
  }
 }
 //Show..connections
 for(int fgid1=0;fgid1<Num;fgid1++){
  printf("#FG%d Ntbl= %d\n",fgid1,Ntbl[fgid1]);
  NbestID[fgid1]=CbestID[fgid1]=-1;
 }

 for(int fgid1=0;fgid1<Num;fgid1++){
  int Nfind=0;
  for(int fgid2=fgid1+1;fgid2<Num;fgid2++){
	int type=CheckOverlap(all[fgid1]->Start,all[fgid1]->End,all[fgid2]->Start,all[fgid2]->End);
	//ignore overlap region
	if(type==-1)
	 continue;
	
	if(FindConnects(all[fgid1],tmp_tbl[fgid1],Ntbl[fgid1],all[fgid2],tmp_tbl[fgid2],Ntbl[fgid2],seq,false)==true){
		if(type==0){
		 //Extend C-ter of fgid1
		 if(CbestID[fgid1]==-1){
		  CbestID[fgid1]=fgid2;
		 }else if(all[CbestID[fgid1]]->Score < all[fgid2]->Score){
		  CbestID[fgid1]=fgid2;
		  printf("Add %d-%d %d-%d (%d)\n",all[fgid1]->Start,all[fgid1]->End,all[fgid2]->Start,all[fgid2]->End,fgid2);
		 }
		 //Extend N-ter of fgid2
		 if(NbestID[fgid2]==-1){
		  NbestID[fgid2]=fgid1;
		 }else if(all[NbestID[fgid2]]->Score < all[fgid1]->Score){
		  NbestID[fgid2]=fgid1;
		 }
		}
		if(type==1){
		 //Extend N-ter of fgid1
		 if(NbestID[fgid1]==-1)
		  NbestID[fgid1]=fgid2;
		 else if(all[NbestID[fgid1]]->Score < all[fgid2]->Score){
		  NbestID[fgid1]=fgid2;
		 }
		 //Extend C-ter of fgid2
		 if(CbestID[fgid2]==-1){
		  CbestID[fgid2]=fgid1;
		 }else if(all[CbestID[fgid2]]->Score < all[fgid1]->Score){
		  CbestID[fgid2]=fgid1;
		 }
		}
	}
  }
 }

 //Remove redundant pairs
 for(int fgid1=0;fgid1<Num;fgid1++){
  int Nfgid = NbestID[fgid1];
  int Cfgid = CbestID[fgid1];
  if(Nfgid!=-1){
   if(CbestID[Nfgid]==fgid1){
    printf("Duplicated!!\n");
    continue;
   }

   printf("%d Nbest= %d %f ",fgid1,Nfgid,all[Nfgid]->Score);
   printf("%d-%d %d-%d\n",all[fgid1]->Start,all[fgid1]->End,all[Nfgid]->Start,all[Nfgid]->End);
   
   FindConnects(all[fgid1],tmp_tbl[fgid1],Ntbl[fgid1],all[Nfgid],tmp_tbl[Nfgid],Ntbl[Nfgid],seq,true);
  }
  if(Cfgid!=-1){
   printf("%d Cbest= %d %f ",fgid1,Cfgid,all[Cfgid]->Score);
   printf("%d-%d %d-%d\n",all[fgid1]->Start,all[fgid1]->End,all[Cfgid]->Start,all[Cfgid]->End);
   FindConnects(all[fgid1],tmp_tbl[fgid1],Ntbl[fgid1],all[Cfgid],tmp_tbl[Cfgid],Ntbl[Cfgid],seq,true);
  }
 }
 return Num;

}





//Model (NODEs) results

int AddModel(RESULT_MODEL *in,RESULT_MODEL **all, int N){
 //puts("Start COmpare....");
 int Num=N;
 bool DisSim=false;
 bool FindSimilar=false;
 float d2=0;
 float d2cut=1.00;
 for(int i=0;i<Num;i++){
  if(in->Len != all[i]->Len)
   continue;
  if(in->Start != all[i]->Start)
   continue;
  if(in->End != all[i]->End)
   continue;

  	//Check positions
	DisSim=false;
	for(int p=0;p<in->Len;p++){
	 d2=(in->Ca[p].real_cd[0]-all[i]->Ca[p].real_cd[0])
	   *(in->Ca[p].real_cd[0]-all[i]->Ca[p].real_cd[0])
	   +(in->Ca[p].real_cd[1]-all[i]->Ca[p].real_cd[1])
	   *(in->Ca[p].real_cd[1]-all[i]->Ca[p].real_cd[1])
	   +(in->Ca[p].real_cd[2]-all[i]->Ca[p].real_cd[2])
	   *(in->Ca[p].real_cd[2]-all[i]->Ca[p].real_cd[2]);

	 	if(d2>d2cut){
		 DisSim=true;
		 break;
		}
	}
	if (DisSim==false)
	 FindSimilar=true;
	
	//Compare and Swap
	if(FindSimilar==true && in->Score > all[i]->Score){
	 printf("Swap.. %d\n",i);
  	 all[i]->Start = in->Start;
  	 all[i]->End = in->End;
	 all[i]->Score = in->Score;
	 all[i]->RawScore = in->RawScore;
	 all[i]->Rmsd = in->Rmsd;
	 all[i]->Len = in->Len;
	 	for(int p=0;p<in->Len;p++){
	 	 all[i]->Ca[p].real_cd[0]=in->Ca[p].real_cd[0];
	 	 all[i]->Ca[p].real_cd[1]=in->Ca[p].real_cd[1];
	 	 all[i]->Ca[p].real_cd[2]=in->Ca[p].real_cd[2];
	 	 all[i]->Ca[p].AAtype=in->Ca[p].AAtype;
		}
	}
  	if(FindSimilar==true)
   	 break;
 }
 int i=Num;
 if(FindSimilar==false){//Add New Data
  //puts("Add New Model");
  Num++;

  all[i]->Start = in->Start;
  all[i]->End = in->End;
  all[i]->Score = in->Score;
  all[i]->RawScore = in->RawScore;
  all[i]->Rmsd = in->Rmsd;
  all[i]->Len = in->Len;

	for(int p=0;p<in->Len;p++){
	  all[i]->Ca[p].real_cd[0]=in->Ca[p].real_cd[0];
	  all[i]->Ca[p].real_cd[1]=in->Ca[p].real_cd[1];
	  all[i]->Ca[p].real_cd[2]=in->Ca[p].real_cd[2];
	  all[i]->Ca[p].AAtype=in->Ca[p].AAtype;
	}
 }
 //printf("END Comp... %d\n",Num);
 return Num;

}


//Compare with AF2 model
bool CompareFragAmodelMore(SEQ_NODE *result,SEQFG *sfg,int Nsfg,NODE *mod,bool mode){
 int MaxInter=20;

 //Malloc MODEL
 RESULT_MODEL **model,tmp_model;
 int Nmodel=0;
 int MaxModel=2000;

 if((model=(RESULT_MODEL **)malloc(sizeof(RESULT_MODEL *)*MaxModel))==NULL)
  return false;
 for(int i=0;i<MaxModel;i++)
  if((model[i]=(RESULT_MODEL *)malloc(sizeof(RESULT_MODEL)))==NULL)
   return false;





 //select two positions
 for(int s1=0;s1<Nsfg;s1++){
  int posi1=sfg[s1].pos;
  int posi1ed=sfg[s1].pos+sfg[s1].l-1;
  int N1=result[s1].Nali;
  SEQ_NODE *res1=&result[s1];
  double cd1[100][3],cd2[100][3];
  double mtx[3][3],mov_com[3],mov_ref[3];
  double rmsd;
	//Fragment
 	for(int s2=s1+1;s2<Nsfg;s2++){
	 int posi2=sfg[s2].pos;
  	 int posi2ed=sfg[s2].pos+sfg[s2].l-1;
  	 int N2=result[s2].Nali;
  	 SEQ_NODE *res2=&result[s2];
	 
	 if(posi1<posi2 && posi1ed > posi2)
	  continue;
	 if(posi2<posi1 && posi2ed > posi1)
	  continue;
	 if(posi1ed<posi2 && posi2-posi1ed > MaxInter)
	  continue;
	 if(posi2ed<posi1 && posi1-posi2ed > MaxInter)
	  continue;
	 printf("Comp %d-%d %d-%d %d vs %d\n",posi1,posi1ed,posi2,posi2ed,N1,N2);
	 //init
	 Nmodel=0;
	 	//Copy coords
		int RefSt,RefEd;
		RefSt=posi1;
		RefEd=posi1;
		if(posi2<posi1)
		 RefSt=posi2;
		for(int i1 =0;i1<N1;i1++){
		 //Zscore cutoff
		 if(res1->score[i1] < cmd.zcut)
		  continue;
		for(int i2 =0;i2<N2;i2++){
		 if(res2->score[i2] < cmd.zcut)
		  continue;
		 int Ncd=0;
		 for(int j=0;j<sfg[s1].l;j++){
	  	  if(res1->ali[i1][j]==NULL)
	   	   continue;
		  if(posi1+j>RefEd)
		   RefEd=posi1+j+1;

	   	  cd1[Ncd][0]=res1->ali[i1][j]->real_cd[0];
	   	  cd1[Ncd][1]=res1->ali[i1][j]->real_cd[1];
	   	  cd1[Ncd][2]=res1->ali[i1][j]->real_cd[2];

	   	  cd2[Ncd][0]=mod[posi1+j].real_cd[0];
	   	  cd2[Ncd][1]=mod[posi1+j].real_cd[1];
	   	  cd2[Ncd][2]=mod[posi1+j].real_cd[2];

	  	  Ncd++;
		 }
		 for(int j=0;j<sfg[s2].l;j++){
	  	  if(res2->ali[i2][j]==NULL)
	   	   continue;
		  if(posi2+j>RefEd)
		   RefEd=posi2+j+1;

	   	  cd1[Ncd][0]=res2->ali[i2][j]->real_cd[0];
	   	  cd1[Ncd][1]=res2->ali[i2][j]->real_cd[1];
	   	  cd1[Ncd][2]=res2->ali[i2][j]->real_cd[2];

	   	  cd2[Ncd][0]=mod[posi2+j].real_cd[0];
	   	  cd2[Ncd][1]=mod[posi2+j].real_cd[1];
	   	  cd2[Ncd][2]=mod[posi2+j].real_cd[2];

	  	  Ncd++;
		 }
		 //Check RMSD
		 fast_rmsd(cd1,cd2,Ncd,&rmsd);
		/*
		 res->rmsd[i]=rmsd;
	 	 if(!isfinite(rmsd))
	  	  res->rmsd[i]=100;
		*/
		 if(rmsd>=cmd.RMSD)
		  continue;
		
	 	  printf("RMSD %d-%d(%d) vs %d-%d(%d) RMSD= %.3f %d-%d\n",posi1,posi1ed,i1,posi2,posi2ed,i2,rmsd,RefSt,RefEd);
		 //Copy Coords again...then compute r&t
		 Ncd=0;
		 for(int j=0;j<sfg[s1].l;j++){
	  	  if(res1->ali[i1][j]==NULL)
	   	   continue;
	   	  cd1[Ncd][0]=res1->ali[i1][j]->real_cd[0];
	   	  cd1[Ncd][1]=res1->ali[i1][j]->real_cd[1];
	   	  cd1[Ncd][2]=res1->ali[i1][j]->real_cd[2];
	   	  cd2[Ncd][0]=mod[posi1+j].real_cd[0];
	   	  cd2[Ncd][1]=mod[posi1+j].real_cd[1];
	   	  cd2[Ncd][2]=mod[posi1+j].real_cd[2];
	  	  Ncd++;
		 }
		 for(int j=0;j<sfg[s2].l;j++){
	  	  if(res2->ali[i2][j]==NULL)
	   	   continue;
	   	  cd1[Ncd][0]=res2->ali[i2][j]->real_cd[0];
	   	  cd1[Ncd][1]=res2->ali[i2][j]->real_cd[1];
	   	  cd1[Ncd][2]=res2->ali[i2][j]->real_cd[2];
	   	  cd2[Ncd][0]=mod[posi2+j].real_cd[0];
	   	  cd2[Ncd][1]=mod[posi2+j].real_cd[1];
	   	  cd2[Ncd][2]=mod[posi2+j].real_cd[2];
	  	  Ncd++;
		 }
		 calculate_rotation_rmsd(cd1,cd2,Ncd,mov_com,mov_ref,mtx,&rmsd);
	 	 //printf("##POS %d RAWSCO= %f ZSCORE= %f RMSD= %f Amodel\n",
		 //RefSt,res1->raw_score[i1]+res2->raw_score[i2],res1->score[i1]+res2->score[i2],rmsd);


		 tmp_model.Start=RefSt;
		 tmp_model.End=RefEd;
		 tmp_model.Score=res1->score[i1]+res2->score[i2];
		 tmp_model.RawScore=res1->raw_score[i1]+res2->raw_score[i2];
		 tmp_model.Rmsd=rmsd;
		 tmp_model.Len=RefEd-RefSt;
		 //move rotate and extend
		 float rotated[3],tmp[3];
		 for(int j=RefSt;j<RefEd;j++){
		  //mov -> o
		  tmp[0]=mod[j].real_cd[0]-mov_com[0];
		  tmp[1]=mod[j].real_cd[1]-mov_com[1];
		  tmp[2]=mod[j].real_cd[2]-mov_com[2];
			for(int i1=0;i1<3;i1++){
				rotated[i1]=0.0;
				for(int i2=0;i2<3;i2++)
				 rotated[i1] +=mtx[i1][i2]*tmp[i2];
			}
		  //o -> mov -> ref
		  tmp[0]=rotated[0]+mov_com[0]+mov_ref[0];
		  tmp[1]=rotated[1]+mov_com[1]+mov_ref[1];
		  tmp[2]=rotated[2]+mov_com[2]+mov_ref[2];
		  //printf("ATOM  %5d  CA  %3s%6d    ",j+1,RES_NAMES[mod[j].AAtype],j+1);
  		  //printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",tmp[0],tmp[1],tmp[2],1.0,res1->score[i1]+res2->score[i2]);
		  tmp_model.Ca[j-RefSt].real_cd[0]=tmp[0];
		  tmp_model.Ca[j-RefSt].real_cd[1]=tmp[1];
		  tmp_model.Ca[j-RefSt].real_cd[2]=tmp[2];
		  tmp_model.Ca[j-RefSt].AAtype=mod[j].AAtype;
		 }
 	 	 //puts("ENDMDL");
		 //ADD tmp_model
		 Nmodel=AddModel(&tmp_model,model,Nmodel);
		 printf("#Nmodel= %d\n",Nmodel);
		}}
	 
		//ShowModel

		for(int m=0;m<Nmodel;m++){
	 	 printf("##POS %d RAWSCO= %f ZSCORE= %f RMSD= %f Amodel\n",
		 model[m]->Start,model[m]->RawScore,model[m]->Score,model[m]->Rmsd);
	 	 puts("MODEL");
			for(int p=0;p<model[m]->Len;p++){
		 printf("ATOM  %5d  CA  %3s%6d    ",model[m]->Start+1+p,RES_NAMES[model[m]->Ca[p].AAtype],model[m]->Start+1+p);
  		 printf("%8.3f%8.3f%8.3f%6.2f%6.2f\n",model[m]->Ca[p].real_cd[0],model[m]->Ca[p].real_cd[1],model[m]->Ca[p].real_cd[2],1.0,model[m]->Score);
			}
 	 	 puts("ENDMDL");
		}

	}
 }
}





bool MakeDummyFrag(SEQ_NODE *new,SEQ_NODE *result,SEQFG *sfg,int Nsfg,bool mode){

 int Ncd=0;
 //Remove redundant fragments
 for(int s1=0;s1<Nsfg;s1++){
  SEQ_NODE *res1=&result[s1];
  int N1=res1->Nali;
  //double cd1[100][3],cd2[100][3];
  if(N1==0)
   continue;
	for(int i1=0;i1<N1;i1++){
	 if(res1->rmsd[i1]==100)
	  continue;
		for(int s2=s1+1;s2<Nsfg;s2++){
		 SEQ_NODE *res2=&result[s2];
		 int N2=res2->Nali;
		 if(N2==0)
		  continue;
			for(int i2=0;i2<N2;i2++){
		 	 if(res2->rmsd[i2]==100)
		 	  continue;
				//Comp two fragment
			 bool flag=false;
	 			for(int j=0;j<sfg[s1].l && j<sfg[s2].l;j++){
				 if(res1->ali[i1][j]!=res2->ali[i2][j])
				  break;
	  			 if(res1->ali[i1][j]==NULL)
	  			  break;
	  			 if(res2->ali[i2][j]==NULL)
	  			  break;
				 flag=true;
				}
			 if(flag==true){
			  //printf("Same %d %d\n",s1,s2);
			  res2->rmsd[i2]=100;
			 }
			}
		}

	}
 }
 int Nnr=0;
 for(int s1=0;s1<Nsfg;s1++){
  SEQ_NODE *res1=&result[s1];
  int N1=res1->Nali;
  //double cd1[100][3],cd2[100][3];
  if(N1==0)
   continue;
	for(int i1=0;i1<N1;i1++){
	 if(res1->rmsd[i1]==100)
	  continue;
	 Ncd=0;
		for(int j=0;j<sfg[s1].l;j++){
		 if(res1->ali[i1][j]==NULL)
	  	  break;
		 Ncd++;
		}
	 if(Ncd<5){
	  res1->rmsd[i1]=100;
	  continue;
	 }
	 Nnr++;
	}
 }
 printf("#Uniq fragment= %d * %d = %d\n",Nnr,Nsfg, Nnr*Nsfg);
 //Input to a new results
 //malloc res
 for(int i=0;i<Nsfg;i++){
  if((new[i].score=(float*)malloc(sizeof(float)*Nnr))==NULL)
   return true;
  if((new[i].raw_score=(float*)malloc(sizeof(float)*Nnr))==NULL)
   return true;
  if((new[i].rmsd=(float*)malloc(sizeof(float)*Nnr))==NULL)
   return true;
  if((new[i].ali=(NODE ***)malloc(sizeof(NODE **)*Nnr))==NULL)
   return true;
  for(int j=0;j<Nnr;j++)
   if((new[i].ali[j]=(NODE **)malloc(sizeof(NODE *)*100))==NULL)
    return true;

  SEQ_NODE *new_res=&new[i];
  new[i].Nali = Nnr;
  int N=0;
  	for(int s1=0;s1<Nsfg;s1++){
  	 SEQ_NODE *res1=&result[s1];
  	 int N1=res1->Nali;
  	 if(N1==0)
  	  continue;
		for(int i1=0;i1<N1;i1++){
	 	 if(res1->rmsd[i1]==100)
	 	  continue;
		 new_res->raw_score[N]=0.0001;
		 new_res->score[N]=0.0001;
		 new_res->rmsd[N]=0.00;
		 for(int j=0;j<sfg[s1].l;j++){
		  new_res->ali[N][j]=res1->ali[i1][j];
		  if(res1->ali[i1][j]==NULL)
	  	   break;
		 }
	 	 N++;
	 	}
	}
  //printf("Nali= %d\n",new[i].Nali); 
 }
 //Input Data
 
}

