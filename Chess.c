#include "neslib.h"

#include <string.h>

/*Piece encoding*/

#define EMPTY   0

#define WPAWN   1

#define WKNIGHT 2

#define WBISHOP 3

#define WROOK   4
 
#define WQUEEN  5

#define WKING   6

#define BPAWN   9

#define BKNIGHT 10

#define BBISHOP 11

#define BROOK   12

#define BQUEEN  13

#define BKING   14

#define IS_W(p) ((p)>=1&&(p)<=6)

#define IS_B(p) ((p)>=9&&(p)<=14)

#define PTYPE(p) ((p)<=6?(p):((p)-8))

#define ISSIDE(p,s) (((s)==0)?IS_W(p):IS_B(p))

/*Game states*/

#define GS_MENU   0

#define GS_PLAY   1

#define GS_SEL    2

#define GS_AI     3

#define GS_OVER   4

/*Constants*/

#define MAXM 50

#define BX 4

#define BY 3

/*Globals*/

static unsigned char brd[64];

static unsigned char cx,cy,sx,sy;

static unsigned char gstate;

static unsigned char plside,aiside,curside;

static unsigned char castl;

static unsigned char epsq;

static unsigned char lastf,lastt;

static unsigned char mflag;

static unsigned char mchoice;

static unsigned char fct;

static unsigned char redraw;

static unsigned char mf[MAXM],mt[MAXM],mfl[MAXM];

static unsigned char mc;

/* Highlight */

static unsigned char hl[64];

/* AI buffers shared */

static unsigned char ab[64]; /* work board */

static unsigned char af[MAXM],at[MAXM],afl[MAXM];

/* 2 level AI buffers */

static unsigned char bf[MAXM],bt[MAXM],bfl[MAXM];

static unsigned char bb[64];

static unsigned char sprid;

/* Piece values */

static const signed int pval[15]={

  0,100,320,330,500,900,20000,0,

  0,-100,-320,-330,-500,-900,-20000

};

/* PST pawn (white) */

static const signed char ppst[64]={

   0, 0, 0, 0, 0, 0, 0, 0,

  50,50,50,50,50,50,50,50,

  10,10,20,30,30,20,10,10,

   5, 5,10,25,25,10, 5, 5,

   0, 0, 0,20,20, 0, 0, 0,

   5,-5,-10, 0, 0,-10,-5, 5,

   5,10,10,-20,-20,10,10, 5,

   0, 0, 0, 0, 0, 0, 0, 0

};

static const signed char npst[64]={

 -50,-40,-30,-30,-30,-30,-40,-50,

 -40,-20, 0, 0, 0, 0,-20,-40,

 -30, 0,10,15,15,10, 0,-30,

 -30, 5,15,20,20,15, 5,-30,

 -30, 0,15,20,20,15, 0,-30,

 -30, 5,10,15,15,10, 5,-30,

 -40,-20, 0, 5, 5, 0,-20,-40,

 -50,-40,-30,-30,-30,-30,-40,-50

};

static const signed char kpst[64]={

 -30,-40,-40,-50,-50,-40,-40,-30,

 -30,-40,-40,-50,-50,-40,-40,-30,

 -30,-40,-40,-50,-50,-40,-40,-30,

 -30,-40,-40,-50,-50,-40,-40,-30,

 -20,-30,-30,-40,-40,-30,-30,-20,

 -10,-20,-20,-20,-20,-20,-20,-10,

  20,20, 0, 0, 0, 0,20,20,

  20,30,10, 0, 0,10,30,20

};

/* Palette */

const unsigned char paldata[32]={

  0x0F,0x27,0x07,0x30,

  0x0F,0x27,0x07,0x30,

  0x0F,0x27,0x07,0x30,

  0x0F,0x27,0x07,0x30,

  0x0F,0x16,0x2A,0x30,

  0x0F,0x16,0x2A,0x30,

  0x0F,0x16,0x2A,0x30,

  0x0F,0x16,0x2A,0x30

};

/* Piece bitmaps 8x8 */

static const unsigned char pbmp[7][8]={

  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

  {0x00,0x18,0x3C,0x3C,0x18,0x18,0x3C,0x7E},

  {0x00,0x0C,0x1E,0x3F,0x37,0x06,0x0F,0x1F},

  {0x00,0x18,0x3C,0x7E,0x3C,0x18,0x3C,0x7E},

  {0x00,0x66,0x7E,0x3C,0x3C,0x3C,0x3C,0x7E},

  {0x42,0x66,0x7E,0x3C,0x3C,0x3C,0x7E,0x7E},

  {0x18,0x18,0x7E,0x7E,0x3C,0x3C,0x7E,0x7E}

};

/* font data */

static const unsigned char fdat[20][8]={

  {0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00},/*A0*/

  {0xFC,0xC6,0xFC,0xC6,0xC6,0xC6,0xFC,0x00},/*B1*/

  {0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00},/*C2*/

  {0xFE,0xC0,0xFC,0xC0,0xC0,0xC0,0xFE,0x00},/*E3*/

  {0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00},/*H4*/

  {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00},/*I5*/

  {0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00},/*K6*/

  {0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x00},/*L7*/

  {0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00},/*M8*/

  {0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00},/*O9*/

  {0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00},/*P10*/

  {0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00},/*R11*/

  {0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00},/*S12*/

  {0xFE,0x30,0x30,0x30,0x30,0x30,0x30,0x00},/*T13*/

  {0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00},/*W14*/

  {0xC6,0xC6,0x6C,0x38,0x18,0x18,0x18,0x00},/*Y15*/

  {0x00,0x18,0x30,0x60,0x30,0x18,0x00,0x00},/*>16*/

  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*sp17*/

  {0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00},/*N18*/

  {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00} /*!19*/

};

#define F_A 0

#define F_B 1

#define F_C 2

#define F_E 3

#define F_H 4

#define F_I 5

#define F_K 6

#define F_L 7

#define F_M 8

#define F_O 9

#define F_P 10

#define F_R 11

#define F_S 12

#define F_T 13

#define F_W 14

#define F_Y 15

#define F_GT 16

#define F_SP 17

#define F_N 18

#define F_EX 19

/* CHR tile */

#define T_LIGHT 0

#define T_DARK  1

/* 2-7: white on light, 8-13: white on dark */

/* 14-19: black on light, 20-25: black on dark */

#define T_HL_L  26

#define T_HL_D  27

#define T_LM_L  28

#define T_LM_D  29

#define T_FONT  30

/* sprite: 0=blank, 1=cursor */

static unsigned char tb[16];

#define SQ(r,c) (((r)<<3)|(c))

#define ROW(s) ((s)>>3)

#define COL(s) ((s)&7)

void mk_empty(unsigned char bg){

  unsigned char i;

  if(bg==0){for(i=0;i<8;i++)tb[i]=0xFF;for(i=8;i<16;i++)tb[i]=0x00;}

  else{for(i=0;i<8;i++)tb[i]=0x00;for(i=8;i<16;i++)tb[i]=0xFF;}

}

void mk_piece(unsigned char pt,unsigned char bg,unsigned char white){

  unsigned char i,s,inner;

  mk_empty(bg);

  if(pt==0)return;

  for(i=0;i<8;i++){

    s=pbmp[pt][i];

    if(white){tb[i]|=s;tb[i+8]|=s;}

    else{

      inner=(s>>1)&(s<<1);

      if(i>0&&i<7)inner&=pbmp[pt][i-1]&pbmp[pt][i+1];

      else inner=0;

      if(bg==0){tb[i]|=s;tb[i+8]=(tb[i+8]&~s)|(s&~inner);}

      else{tb[i]=(tb[i]&~s)|(s&~inner);tb[i+8]|=s;}

    }

  }

}

void mk_hl(unsigned char bg){

  mk_empty(bg);

  tb[0]|=0xE7;tb[8]|=0xE7;

  tb[1]|=0xC3;tb[9]|=0xC3;

  tb[6]|=0xC3;tb[14]|=0xC3;

  tb[7]|=0xE7;tb[15]|=0xE7;

}

void mk_lm(unsigned char bg){

  unsigned char i;

  mk_empty(bg);

  tb[0]|=0xFF;tb[8]|=0xFF;

  tb[7]|=0xFF;tb[15]|=0xFF;

  for(i=0;i<8;i++){tb[i]|=0x81;tb[i+8]|=0x81;}

}

void upload_chr(void){

  unsigned char pt;

  ppu_off();

  vram_adr(0x0000);

  /* 0,1: empty light/dark */

  mk_empty(0);vram_write(tb,16);

  mk_empty(1);vram_write(tb,16);

  /* 2-7: white pieces on light */

  for(pt=1;pt<=6;pt++){mk_piece(pt,0,1);vram_write(tb,16);}

  /* 8-13: white pieces on dark */

  for(pt=1;pt<=6;pt++){mk_piece(pt,1,1);vram_write(tb,16);}

  /* 14-19: black pieces on light */

  for(pt=1;pt<=6;pt++){mk_piece(pt,0,0);vram_write(tb,16);}

  /* 20-25: black pieces on dark */

  for(pt=1;pt<=6;pt++){mk_piece(pt,1,0);vram_write(tb,16);}

  /* 26,27: highlight */

  mk_hl(0);vram_write(tb,16);

  mk_hl(1);vram_write(tb,16);

  /* 28,29: last move */

  mk_lm(0);vram_write(tb,16);

  mk_lm(1);vram_write(tb,16);

  /* 30+: font */

  {unsigned char i,k;

  for(i=0;i<20;i++){

    for(k=0;k<8;k++)tb[k]=fdat[i][k];

    for(k=0;k<8;k++)tb[k+8]=fdat[i][k];

    vram_write(tb,16);

  }}

  /* sprite chr at $1000 */

  vram_adr(0x1000);

  {unsigned char i;for(i=0;i<16;i++)tb[i]=0;vram_write(tb,16);}

  tb[0]=0xFF;tb[1]=0x81;tb[2]=0x81;tb[3]=0x81;

  tb[4]=0x81;tb[5]=0x81;tb[6]=0x81;tb[7]=0xFF;

  tb[8]=0xFF;tb[9]=0x81;tb[10]=0x81;tb[11]=0x81;

  tb[12]=0x81;tb[13]=0x81;tb[14]=0x81;tb[15]=0xFF;

  vram_write(tb,16);

}

unsigned char sq_light(unsigned char r,unsigned char c){return((r+c)&1)==0;}

unsigned char get_tile(unsigned char r,unsigned char c){

  unsigned char p,sq,li;

  sq=SQ(r,c);li=sq_light(r,c);

  if(hl[sq]==1)return li?T_HL_L:T_HL_D;

  p=brd[sq];

  if(p==EMPTY){

    if(sq==lastf||sq==lastt)return li?T_LM_L:T_LM_D;

    return li?T_LIGHT:T_DARK;

  }

  if(IS_W(p))return li?(2+p-1):(8+p-1);

  else{unsigned char t=p-8;return li?(14+t-1):(20+t-1);}

}

void init_board(void){

  unsigned char i;

  for(i=0;i<64;i++)brd[i]=EMPTY;

  brd[0]=BROOK;brd[1]=BKNIGHT;brd[2]=BBISHOP;brd[3]=BQUEEN;

  brd[4]=BKING;brd[5]=BBISHOP;brd[6]=BKNIGHT;brd[7]=BROOK;

  for(i=0;i<8;i++)brd[SQ(1,i)]=BPAWN;

  for(i=0;i<8;i++)brd[SQ(6,i)]=WPAWN;

  brd[56]=WROOK;brd[57]=WKNIGHT;brd[58]=WBISHOP;brd[59]=WQUEEN;

  brd[60]=WKING;brd[61]=WBISHOP;brd[62]=WKNIGHT;brd[63]=WROOK;

  castl=0x0F;epsq=0xFF;lastf=0xFF;lastt=0xFF;curside=0;mflag=0;

}

void put_text(unsigned int addr,const unsigned char *s,unsigned char len){

  unsigned char i;vram_adr(addr);

  for(i=0;i<len;i++)vram_put(T_FONT+s[i]);

}

void draw_board(void){

  unsigned char r,c,dr,dc;

  vram_adr(NAMETABLE_A);vram_fill(0x00,960);vram_fill(0x00,64);

  for(r=0;r<8;r++){

    if(plside)dr=7-r;else dr=r;

    vram_adr(NAMETABLE_A+(BY+r)*32+BX);

    for(c=0;c<8;c++){

      if(plside)dc=7-c;else dc=c;

      vram_put(get_tile(dr,dc));

    }

  }

  /* turn */

  {static const unsigned char wt[5]={F_W,F_H,F_I,F_T,F_E};

   static const unsigned char bt[5]={F_B,F_L,F_A,F_C,F_K};

   vram_adr(NAMETABLE_A+1*32+16);

   if(curside==0){unsigned char k;for(k=0;k<5;k++)vram_put(T_FONT+wt[k]);}

   else{unsigned char k;for(k=0;k<5;k++)vram_put(T_FONT+bt[k]);}

  }

  if(mflag==1){

    static const unsigned char cm[4]={F_M,F_A,F_T,F_E};

    put_text(NAMETABLE_A+14*32+16,cm,4);

  }else if(mflag==2){

    static const unsigned char sm[4]={F_S,F_T,F_A,F_L};

    put_text(NAMETABLE_A+14*32+16,sm,4);

  }

}

void draw_menu(void){

  static const unsigned char ti[5]={F_C,F_H,F_E,F_S,F_S};

  static const unsigned char pa[4]={F_P,F_L,F_A,F_Y};

  static const unsigned char wt[5]={F_W,F_H,F_I,F_T,F_E};

  static const unsigned char bt[5]={F_B,F_L,F_A,F_C,F_K};

  vram_adr(NAMETABLE_A);vram_fill(0x00,960);vram_fill(0x00,64);

  put_text(NAMETABLE_A+6*32+14,ti,5);

  put_text(NAMETABLE_A+12*32+13,pa,4);

  vram_adr(NAMETABLE_A+15*32+11);

  vram_put(mchoice==0?T_FONT+F_GT:T_FONT+F_SP);

  put_text(NAMETABLE_A+15*32+13,wt,5);

  vram_adr(NAMETABLE_A+17*32+11);

  vram_put(mchoice==1?T_FONT+F_GT:T_FONT+F_SP);

  put_text(NAMETABLE_A+17*32+13,bt,5);

}

/* threat detection */

unsigned char attacked(unsigned char *b,unsigned char sq,unsigned char by){

  signed char dr,dc;

  unsigned char r,c,tr,tc,i,p;

  r=ROW(sq);c=COL(sq);

  /* knights */

  {static const signed char kd[8]={-2,-2,-1,-1,1,1,2,2};

   static const signed char kc[8]={-1,1,-2,2,-2,2,-1,1};

   for(i=0;i<8;i++){tr=r+kd[i];tc=c+kc[i];

     if(tr<8&&tc<8){p=b[SQ(tr,tc)];if(PTYPE(p)==2&&ISSIDE(p,by))return 1;}}}

  /* king */

  for(dr=-1;dr<=1;dr++)for(dc=-1;dc<=1;dc++){

    if(dr==0&&dc==0)continue;tr=r+dr;tc=c+dc;

    if(tr<8&&tc<8){p=b[SQ(tr,tc)];if(PTYPE(p)==6&&ISSIDE(p,by))return 1;}}

  /* pawns */

  {signed char pd=by?1:-1;

   if(c>0){tr=r+pd;if(tr<8){p=b[SQ(tr,c-1)];if(PTYPE(p)==1&&ISSIDE(p,by))return 1;}}

   if(c<7){tr=r+pd;if(tr<8){p=b[SQ(tr,c+1)];if(PTYPE(p)==1&&ISSIDE(p,by))return 1;}}}

  /* bishop/queen diag */

  {static const signed char bd[4]={-1,-1,1,1};static const signed char bc[4]={-1,1,-1,1};

   for(i=0;i<4;i++){dr=bd[i];dc=bc[i];tr=r+dr;tc=c+dc;

     while(tr<8&&tc<8){p=b[SQ(tr,tc)];

       if(p!=EMPTY){if(ISSIDE(p,by)&&(PTYPE(p)==3||PTYPE(p)==5))return 1;break;}

       tr+=dr;tc+=dc;}}}

  /* rook/queen straight */

  {static const signed char rd[4]={-1,1,0,0};static const signed char rc[4]={0,0,-1,1};

   for(i=0;i<4;i++){dr=rd[i];dc=rc[i];tr=r+dr;tc=c+dc;

     while(tr<8&&tc<8){p=b[SQ(tr,tc)];

       if(p!=EMPTY){if(ISSIDE(p,by)&&(PTYPE(p)==4||PTYPE(p)==5))return 1;break;}

       tr+=dr;tc+=dc;}}}

  return 0;

}

unsigned char findking(unsigned char *b,unsigned char s){

  unsigned char i,tgt;tgt=s?BKING:WKING;

  for(i=0;i<64;i++)if(b[i]==tgt)return i;

  return 0xFF;

}

unsigned char incheck(unsigned char *b,unsigned char s){

  unsigned char ks=findking(b,s);if(ks==0xFF)return 0;return attacked(b,ks,s^1);

}

/* Move execution */

void domove(unsigned char *b,unsigned char f,unsigned char t,unsigned char fl,unsigned char *cp,unsigned char *ep){

  unsigned char pc=b[f];

  b[t]=pc;b[f]=EMPTY;

  if(fl==1){/*ep*/unsigned char ec;if(IS_W(pc))ec=t+8;else ec=t-8;b[ec]=EMPTY;}

  if(fl==2){/*castle K*/if(COL(t)==6){b[SQ(ROW(t),5)]=b[SQ(ROW(t),7)];b[SQ(ROW(t),7)]=EMPTY;}}

  if(fl==3){/*castle Q*/if(COL(t)==2){b[SQ(ROW(t),3)]=b[SQ(ROW(t),0)];b[SQ(ROW(t),0)]=EMPTY;}}

  if(fl==4){b[t]=IS_W(pc)?WQUEEN:BQUEEN;}

  if(fl==5){*ep=IS_W(pc)?t+8:t-8;}else{*ep=0xFF;}

  if(pc==WKING)*cp&=~3;if(pc==BKING)*cp&=~12;

  if(f==56||t==56)*cp&=~2;if(f==63||t==63)*cp&=~1;

  if(f==0||t==0)*cp&=~8;if(f==7||t==7)*cp&=~4;

}

/* Move Generation */

void addm(unsigned char f,unsigned char t,unsigned char fl){

  if(mc<MAXM){mf[mc]=f;mt[mc]=t;mfl[mc]=fl;mc++;}}

void gen_pawn(unsigned char *b,unsigned char sq,unsigned char s,unsigned char ep){

  unsigned char r=ROW(sq),c=COL(sq),tr,ts;

  signed char d=s?1:-1;unsigned char pr=s?7:0,sr=s?1:6;

  signed char dc;

  tr=r+d;if(tr<8){ts=SQ(tr,c);if(b[ts]==EMPTY){

    addm(sq,ts,tr==pr?4:0);

    if(r==sr){tr=r+d+d;ts=SQ(tr,c);if(b[ts]==EMPTY)addm(sq,ts,5);}}}

  for(dc=-1;dc<=1;dc+=2){if((unsigned char)(c+dc)<8){tr=r+d;if(tr<8){

    ts=SQ(tr,c+dc);

    if(b[ts]!=EMPTY&&!ISSIDE(b[ts],s))addm(sq,ts,tr==pr?4:0);

    if(ts==ep)addm(sq,ts,1);}}}

}

void gen_knight(unsigned char *b,unsigned char sq,unsigned char s){

  static const signed char kd[8]={-2,-2,-1,-1,1,1,2,2};

  static const signed char kc[8]={-1,1,-2,2,-2,2,-1,1};

  unsigned char i,tr,tc,ts;

  for(i=0;i<8;i++){tr=ROW(sq)+kd[i];tc=COL(sq)+kc[i];

    if(tr<8&&tc<8){ts=SQ(tr,tc);if(b[ts]==EMPTY||!ISSIDE(b[ts],s))addm(sq,ts,0);}}

}

void gen_slide(unsigned char *b,unsigned char sq,unsigned char s,signed char dr,signed char dc){

  unsigned char tr=ROW(sq)+dr,tc=COL(sq)+dc,ts;

  while(tr<8&&tc<8){ts=SQ(tr,tc);

    if(b[ts]==EMPTY)addm(sq,ts,0);

    else{if(!ISSIDE(b[ts],s))addm(sq,ts,0);break;}

    tr+=dr;tc+=dc;}

}

void gen_bishop(unsigned char *b,unsigned char sq,unsigned char s){

  gen_slide(b,sq,s,-1,-1);gen_slide(b,sq,s,-1,1);

  gen_slide(b,sq,s,1,-1);gen_slide(b,sq,s,1,1);

}

void gen_rook(unsigned char *b,unsigned char sq,unsigned char s){

  gen_slide(b,sq,s,-1,0);gen_slide(b,sq,s,1,0);

  gen_slide(b,sq,s,0,-1);gen_slide(b,sq,s,0,1);

}

void gen_king(unsigned char *b,unsigned char sq,unsigned char s,unsigned char ca){

  signed char dr,dc;unsigned char tr,tc,ts,r=ROW(sq),c=COL(sq);

  for(dr=-1;dr<=1;dr++)for(dc=-1;dc<=1;dc++){

    if(dr==0&&dc==0)continue;tr=r+dr;tc=c+dc;

    if(tr<8&&tc<8){ts=SQ(tr,tc);if(b[ts]==EMPTY||!ISSIDE(b[ts],s))addm(sq,ts,0);}}

  if(s==0){

    if((ca&1)&&c==4&&r==7&&b[61]==EMPTY&&b[62]==EMPTY

       &&!attacked(b,60,1)&&!attacked(b,61,1)&&!attacked(b,62,1))addm(sq,62,2);

    if((ca&2)&&c==4&&r==7&&b[59]==EMPTY&&b[58]==EMPTY&&b[57]==EMPTY

       &&!attacked(b,60,1)&&!attacked(b,59,1)&&!attacked(b,58,1))addm(sq,58,3);

  }else{

    if((ca&4)&&c==4&&r==0&&b[5]==EMPTY&&b[6]==EMPTY

       &&!attacked(b,4,0)&&!attacked(b,5,0)&&!attacked(b,6,0))addm(sq,6,2);

    if((ca&8)&&c==4&&r==0&&b[3]==EMPTY&&b[2]==EMPTY&&b[1]==EMPTY

       &&!attacked(b,4,0)&&!attacked(b,3,0)&&!attacked(b,2,0))addm(sq,2,3);

  }

}

void gen_moves(unsigned char *b,unsigned char s,unsigned char ca,unsigned char ep){

  unsigned char i,p,pt;mc=0;

  for(i=0;i<64;i++){p=b[i];if(p==EMPTY||!ISSIDE(p,s))continue;pt=PTYPE(p);

    switch(pt){

      case 1:gen_pawn(b,i,s,ep);break;case 2:gen_knight(b,i,s);break;

      case 3:gen_bishop(b,i,s);break;case 4:gen_rook(b,i,s);break;

      case 5:gen_bishop(b,i,s);gen_rook(b,i,s);break;

      case 6:gen_king(b,i,s,ca);break;

    }

  }

}

/* Filter to legal moves into mf/mt/mfl, returns count... Hopefully */

unsigned char gen_legal(unsigned char *b,unsigned char s,unsigned char ca,unsigned char ep){

  unsigned char i,cnt,tc2,te;

  gen_moves(b,s,ca,ep);cnt=mc;

  /* copy to temp */

  memcpy(af,mf,cnt);memcpy(at,mt,cnt);memcpy(afl,mfl,cnt);

  mc=0;

  for(i=0;i<cnt;i++){

    memcpy(ab,b,64);tc2=ca;te=ep;

    domove(ab,af[i],at[i],afl[i],&tc2,&te);

    if(!incheck(ab,s)){mf[mc]=af[i];mt[mc]=at[i];mfl[mc]=afl[i];mc++;}

  }

  return mc;

}

void gen_legal_sq(unsigned char sq){

  unsigned char i,cnt,tc2,te;

  gen_moves(brd,curside,castl,epsq);cnt=mc;

  memcpy(af,mf,cnt);memcpy(at,mt,cnt);memcpy(afl,mfl,cnt);

  mc=0;

  for(i=0;i<cnt;i++){

    if(af[i]==sq){

      memcpy(ab,brd,64);tc2=castl;te=epsq;

      domove(ab,af[i],at[i],afl[i],&tc2,&te);

      if(!incheck(ab,curside)){mf[mc]=af[i];mt[mc]=at[i];mfl[mc]=afl[i];mc++;}

    }

  }

}

/* Evaluation */

signed int evaluate(unsigned char *b){

  signed int sc=0;unsigned char i,p,pt,r,c,mi;

  for(i=0;i<64;i++){p=b[i];if(p==EMPTY)continue;

    sc+=pval[p];pt=PTYPE(p);r=ROW(i);c=COL(i);

    if(IS_W(p)){

      if(pt==1)sc+=ppst[i];else if(pt==2)sc+=npst[i];else if(pt==6)sc+=kpst[i];

    }else{mi=SQ(7-r,c);

      if(pt==1)sc-=ppst[mi];else if(pt==2)sc-=npst[mi];else if(pt==6)sc-=kpst[mi];

    }

  }

  return sc;

}

/* AI depth-2 minimax with alpha-beta algorithm */

signed int negamax(unsigned char *b,unsigned char s,unsigned char d,

                   signed int alpha,signed int beta,unsigned char ca,unsigned char ep){

  unsigned char i,cnt,tc2,te;

  signed int val,child;

  signed int sign=s==0?1:-1;

  if(d==0)return evaluate(b);

  gen_moves(b,s,ca,ep);cnt=mc;

  memcpy(bf,mf,cnt);memcpy(bt,mt,cnt);memcpy(bfl,mfl,cnt);

  /* Filter legal */

  {unsigned char j=0;

  for(i=0;i<cnt;i++){

    memcpy(bb,b,64);tc2=ca;te=ep;

    domove(bb,bf[i],bt[i],bfl[i],&tc2,&te);

    if(!incheck(bb,s)){bf[j]=bf[i];bt[j]=bt[i];bfl[j]=bfl[i];j++;}

  }cnt=j;}

  if(cnt==0){

    if(incheck(b,s))return s==0?-30000:30000;

    return 0;

  }

  if(s==0){val=-30000;

    for(i=0;i<cnt;i++){

      memcpy(bb,b,64);tc2=ca;te=ep;

      domove(bb,bf[i],bt[i],bfl[i],&tc2,&te);

      child=negamax(bb,1,d-1,alpha,beta,tc2,te);

      if(child>val)val=child;if(val>alpha)alpha=val;if(beta<=alpha)break;

    }

  }else{val=30000;

    for(i=0;i<cnt;i++){

      memcpy(bb,b,64);tc2=ca;te=ep;

      domove(bb,bf[i],bt[i],bfl[i],&tc2,&te);

      child=negamax(bb,0,d-1,alpha,beta,tc2,te);

      if(child<val)val=child;if(val<beta)beta=val;if(beta<=alpha)break;

    }

  }

  return val;

}

void ai_move(void){

  unsigned char i,cnt,tc2,te;

  signed int val,best;

  unsigned char bestf,bestt,bestfl;

  cnt=gen_legal(brd,aiside,castl,epsq);

  if(cnt==0)return;

  /* copy legal moves */

  memcpy(af,mf,cnt);memcpy(at,mt,cnt);memcpy(afl,mfl,cnt);

  bestf=af[0];bestt=at[0];bestfl=afl[0];

  best=aiside==0?-32000:32000;

  for(i=0;i<cnt;i++){

    memcpy(ab,brd,64);tc2=castl;te=epsq;

    domove(ab,af[i],at[i],afl[i],&tc2,&te);

    val=negamax(ab,aiside^1,1,-32000,32000,tc2,te);

    if(aiside==0){if(val>best){best=val;bestf=af[i];bestt=at[i];bestfl=afl[i];}}

    else{if(val<best){best=val;bestf=af[i];bestt=at[i];bestfl=afl[i];}}

  }

  /* Execute */

  domove(brd,bestf,bestt,bestfl,&castl,&epsq);

  lastf=bestf;lastt=bestt;curside^=1;

  {unsigned char n=gen_legal(brd,curside,castl,epsq);

   unsigned char ck=incheck(brd,curside);

   if(n==0){mflag=ck?1:2;}else mflag=0;}

}

void exec_move(unsigned char f,unsigned char t,unsigned char fl){

  domove(brd,f,t,fl,&castl,&epsq);

  lastf=f;lastt=t;curside^=1;

  {unsigned char n=gen_legal(brd,curside,castl,epsq);

   unsigned char ck=incheck(brd,curside);

   if(n==0){mflag=ck?1:2;}else mflag=0;}

}

void draw_cursor(void){

  unsigned char vr,vc,sx2,sy2;

  oam_clear();

  if(gstate!=GS_PLAY&&gstate!=GS_SEL)return;

  if(plside){vr=7-cy;vc=7-cx;}else{vr=cy;vc=cx;}

  sx2=(BX+vc)*8;sy2=(BY+vr)*8-1;

  sprid=0;sprid=oam_spr(sx2,sy2,1,1,sprid);

}

void main(void){

  unsigned char pd,pold,pnew;

  ppu_off();upload_chr();

  pal_all(paldata);bank_spr(1);bank_bg(0);

  gstate=GS_MENU;mchoice=0;

  cx=4;cy=6;lastf=0xFF;lastt=0xFF;

  draw_menu();oam_clear();ppu_on_all();

  pold=0;

  while(1){

    ppu_wait_nmi();fct++;

    pold=pd;pd=pad_poll(0);pnew=pd&~pold;

    switch(gstate){

    case GS_MENU:

      oam_clear();

      if(pnew&PAD_UP)mchoice=0;

      if(pnew&PAD_DOWN)mchoice=1;

      if(pnew&(PAD_START|PAD_A)){

        plside=mchoice;aiside=plside^1;

        init_board();memset(hl,0,64);

        cx=4;cy=plside?1:6;

        ppu_off();draw_board();ppu_on_all();

        gstate=(curside==aiside)?GS_AI:GS_PLAY;

      }else{ppu_off();draw_menu();ppu_on_all();}

      break;

    case GS_PLAY:

      if(pnew&PAD_UP&&cy>0)cy--;

      if(pnew&PAD_DOWN&&cy<7)cy++;

      if(pnew&PAD_LEFT&&cx>0)cx--;

      if(pnew&PAD_RIGHT&&cx<7)cx++;

      if(pnew&PAD_A){

        unsigned char sq=SQ(cy,cx),p=brd[sq];

        if(p!=EMPTY&&ISSIDE(p,plside)){

          sx=cx;sy=cy;memset(hl,0,64);

          gen_legal_sq(sq);

          {unsigned char k;for(k=0;k<mc;k++)hl[mt[k]]=1;}

          gstate=GS_SEL;redraw=1;

        }

      }

      if(pnew&PAD_B){memset(hl,0,64);redraw=1;}

      if(redraw){ppu_off();draw_board();ppu_on_all();redraw=0;}

      draw_cursor();

      break;

    case GS_SEL:

      if(pnew&PAD_UP&&cy>0)cy--;

      if(pnew&PAD_DOWN&&cy<7)cy++;

      if(pnew&PAD_LEFT&&cx>0)cx--;

      if(pnew&PAD_RIGHT&&cx<7)cx++;

      if(pnew&PAD_B){memset(hl,0,64);gstate=GS_PLAY;redraw=1;}

      if(pnew&PAD_A){

        unsigned char tsq=SQ(cy,cx);

        if(brd[tsq]!=EMPTY&&ISSIDE(brd[tsq],plside)&&(cx!=sx||cy!=sy)){

          sx=cx;sy=cy;memset(hl,0,64);

          gen_legal_sq(tsq);

          {unsigned char k;for(k=0;k<mc;k++)hl[mt[k]]=1;}

          redraw=1;

        }else if(hl[tsq]){

          unsigned char fsq=SQ(sy,sx),k,fl=0;

          for(k=0;k<mc;k++){if(mf[k]==fsq&&mt[k]==tsq){fl=mfl[k];break;}}

          memset(hl,0,64);

          exec_move(fsq,tsq,fl);redraw=1;

          gstate=mflag?GS_OVER:GS_AI;

        }

      }

      if(redraw){ppu_off();draw_board();ppu_on_all();redraw=0;}

      draw_cursor();

      break;

    case GS_AI:

      oam_clear();

      ai_move();

      memset(hl,0,64);

      ppu_off();draw_board();ppu_on_all();

      gstate=mflag?GS_OVER:GS_PLAY;

      draw_cursor();

      break;

    case GS_OVER:

      draw_cursor();

      if(pnew&PAD_START){

        gstate=GS_MENU;mchoice=0;

        ppu_off();draw_menu();ppu_on_all();

      }

      break;

    }

  }

}
 
