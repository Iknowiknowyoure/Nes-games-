#include "neslib.h"

#define PADDLE_HEIGHT 24

#define LEFT_PADDLE_X 20

#define RIGHT_PADDLE_X 232

#define PADDLE_SPEED 2

#define TOP_WALL 24

#define BOTTOM_WALL 216

#define WINNING_SCORE 9

#define AI_SPEED 2

#define STATE_TITLE 0

#define STATE_PLAYING 1

#define STATE_SCORED 2

#define STATE_GAMEOVER 3

#define T_BLANK  0

#define T_SOLID  1

#define T_CENTER 2

#define T_DIG0   3

#define T_LP  13

#define T_LO  14

#define T_LN  15

#define T_LG  16

#define T_LR  17

#define T_LE  18

#define T_LS  19

#define T_LT  20

#define T_LA  21

#define T_LI  22

#define T_LW  23

#define ST_SOLID 1

#define ST_BALL  2

static unsigned char paddle1_y;

static unsigned char paddle2_y;

static unsigned char ball_x;

static unsigned char ball_y;

static signed char ball_dx;

static signed char ball_dy;

static unsigned char score1;

static unsigned char score2;

static unsigned char game_state;

static unsigned char frame_count;

static unsigned char serve_timer;

static unsigned char pad;

static unsigned char j;

const unsigned char pal_data[32]={

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00,

  0x0F,0x30,0x10,0x00

};

const unsigned char tiles_bg[] = {

  // Tile 0: blank

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  // Tile 1: solid block

  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

  // Tile 2: center dashes

  0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,

  0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,

  // Tile 3: digit 0

  0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00,

  0x7C,0xC6,0xCE,0xD6,0xE6,0xC6,0x7C,0x00,

  // Tile 4: digit 1

  0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00,

  0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00,

  // Tile 5: digit 2

  0x7C,0xC6,0x0E,0x3C,0x70,0xC0,0xFE,0x00,

  0x7C,0xC6,0x0E,0x3C,0x70,0xC0,0xFE,0x00,

  // Tile 6: digit 3

  0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00,

  0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00,

  // Tile 7: digit 4

  0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x00,

  0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x00,

  // Tile 8: digit 5

  0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00,

  0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00,

  // Tile 9: digit 6

  0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00,

  0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00,

  // Tile 10: digit 7

  0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00,

  0xFE,0x06,0x0C,0x18,0x30,0x30,0x30,0x00,

  // Tile 11: digit 8

  0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00,

  0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00,

  // Tile 12: digit 9

  0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00,

  0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00,

  // Tile 13: P

  0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00,

  0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00,

  // Tile 14: O

  0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,

  0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,

  // Tile 15: N

  0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00,

  0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00,

  // Tile 16: G

  0x7C,0xC6,0xC0,0xDE,0xC6,0xC6,0x7C,0x00,

  0x7C,0xC6,0xC0,0xDE,0xC6,0xC6,0x7C,0x00,

  // Tile 17: R

  0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00,

  0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00,

  // Tile 18: E

  0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xFE,0x00,

  0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xFE,0x00,

  // Tile 19: S

  0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00,

  0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00,

  // Tile 20: T

  0xFE,0x30,0x30,0x30,0x30,0x30,0x30,0x00,

  0xFE,0x30,0x30,0x30,0x30,0x30,0x30,0x00,

  // Tile 21: A

  0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,

  0x38,0x6C,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,

  // Tile 22: I

  0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,

  0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,

  // Tile 23: W

  0xC6,0xC6,0xC6,0xD6,0xD6,0xEE,0xC6,0x00,

  0xC6,0xC6,0xC6,0xD6,0xD6,0xEE,0xC6,0x00

};

const unsigned char tiles_spr[] = {

  // Sprite Tile 0: blank

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  // Sprite Tile 1: solid (paddle)

  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

  // Sprite Tile 2: ball

  0x00,0x3C,0x7E,0x7E,0x7E,0x7E,0x3C,0x00,

  0x00,0x3C,0x7E,0x7E,0x7E,0x7E,0x3C,0x00

};

void upload_chr(void) {

  vram_adr(0x0000);

  vram_write((unsigned char*)tiles_bg, sizeof(tiles_bg));

  vram_adr(0x1000);

  vram_write((unsigned char*)tiles_spr, sizeof(tiles_spr));

}

void draw_playfield(void) {

  vram_adr(NAMETABLE_A);

  vram_fill(T_BLANK, 960);

  vram_fill(0x00, 64);

  vram_adr(NAMETABLE_A + 2*32);

  for(j=0;j<32;j++) vram_put(T_SOLID);

  vram_adr(NAMETABLE_A + 27*32);

  for(j=0;j<32;j++) vram_put(T_SOLID);

  for(j=3;j<27;j++) {

    if(j & 1) {

      vram_adr(NAMETABLE_A + j*32 + 15);

      vram_put(T_CENTER);

    }

  }

  vram_adr(NAMETABLE_A + 1*32 + 11);

  vram_put(score1 + T_DIG0);

  vram_adr(NAMETABLE_A + 1*32 + 20);

  vram_put(score2 + T_DIG0);

}

void draw_title(void) {

  vram_adr(NAMETABLE_A);

  vram_fill(T_BLANK, 960);

  vram_fill(0x00, 64);

  vram_adr(NAMETABLE_A + 10*32 + 14);

  vram_put(T_LP);

  vram_put(T_LO);

  vram_put(T_LN);

  vram_put(T_LG);

  vram_adr(NAMETABLE_A + 16*32 + 11);

  vram_put(T_LP);

  vram_put(T_LR);

  vram_put(T_LE);

  vram_put(T_LS);

  vram_put(T_LS);

  vram_put(T_BLANK);

  vram_put(T_LS);

  vram_put(T_LT);

  vram_put(T_LA);

  vram_put(T_LR);

  vram_put(T_LT);

}

void draw_gameover(void) {

  if(score1 >= WINNING_SCORE) {

    vram_adr(NAMETABLE_A + 14*32 + 11);

    vram_put(T_LP);

    vram_put(T_DIG0 + 1);

    vram_put(T_BLANK);

    vram_put(T_LW);

    vram_put(T_LI);

    vram_put(T_LN);

    vram_put(T_LS);

  } else {

    vram_adr(NAMETABLE_A + 14*32 + 11);

    vram_put(T_LA);

    vram_put(T_LI);

    vram_put(T_BLANK);

    vram_put(T_LW);

    vram_put(T_LI);

    vram_put(T_LN);

    vram_put(T_LS);

  }

  vram_adr(NAMETABLE_A + 18*32 + 11);

  vram_put(T_LP);

  vram_put(T_LR);

  vram_put(T_LE);

  vram_put(T_LS);

  vram_put(T_LS);

  vram_put(T_BLANK);

  vram_put(T_LS);

  vram_put(T_LT);

  vram_put(T_LA);

  vram_put(T_LR);

  vram_put(T_LT);

}

void reset_ball(void) {

  ball_x = 124;

  ball_y = 120;

  ball_dx = (frame_count & 1) ? 2 : -2;

  ball_dy = (frame_count & 2) ? 1 : -1;

  serve_timer = 60;

}

void init_game(void) {

  paddle1_y = 108;

  paddle2_y = 108;

  score1 = 0;

  score2 = 0;

  frame_count = 0;

  reset_ball();

}

void draw_sprites(void) {

  static unsigned char spr_id;

  oam_clear();

  spr_id = 0;

  // Ball - oam_spr returns next free id

  spr_id = oam_spr(ball_x, ball_y, ST_BALL, 0, spr_id);

  // Left paddle

  spr_id = oam_spr(LEFT_PADDLE_X, paddle1_y,      ST_SOLID, 0, spr_id);

  spr_id = oam_spr(LEFT_PADDLE_X, paddle1_y + 8,  ST_SOLID, 0, spr_id);

  spr_id = oam_spr(LEFT_PADDLE_X, paddle1_y + 16, ST_SOLID, 0, spr_id);

  // Right paddle

  spr_id = oam_spr(RIGHT_PADDLE_X, paddle2_y,      ST_SOLID, 0, spr_id);

  spr_id = oam_spr(RIGHT_PADDLE_X, paddle2_y + 8,  ST_SOLID, 0, spr_id);

  spr_id = oam_spr(RIGHT_PADDLE_X, paddle2_y + 16, ST_SOLID, 0, spr_id);

}

void update_player(void) {

  pad = pad_poll(0);

  if((pad & PAD_UP) && paddle1_y > TOP_WALL + 8) {

    paddle1_y -= PADDLE_SPEED;

  }

  if((pad & PAD_DOWN) && paddle1_y + PADDLE_HEIGHT < BOTTOM_WALL - 8) {

    paddle1_y += PADDLE_SPEED;

  }

}

void update_ai(void) {

  unsigned char pc;

  unsigned char tgt;

  pc = paddle2_y + 12;

  if(ball_dx > 0) {

    tgt = ball_y + 4;

    if(pc + 2 < tgt && paddle2_y + PADDLE_HEIGHT < BOTTOM_WALL - 8) {

      paddle2_y += AI_SPEED;

    } else if(pc > tgt + 2 && paddle2_y > TOP_WALL + 8) {

      paddle2_y -= AI_SPEED;

    }

  } else {

    if(pc < 118) paddle2_y += 1;

    else if(pc > 122) paddle2_y -= 1;

  }

}

void update_ball(void) {

  unsigned char nx, ny;

  signed char ho;

  if(serve_timer > 0) {

    --serve_timer;

    return;

  }

  nx = ball_x + ball_dx;

  ny = ball_y + ball_dy;

  if(ny <= TOP_WALL + 8) {

    ny = TOP_WALL + 8;

    ball_dy = -ball_dy;

  }

  if(ny >= BOTTOM_WALL - 16) {

    ny = BOTTOM_WALL - 16;

    ball_dy = -ball_dy;

  }

  if(ball_dx < 0 && nx <= LEFT_PADDLE_X + 8 && ball_x > LEFT_PADDLE_X + 8) {

    if(ny + 8 > paddle1_y && ny < paddle1_y + PADDLE_HEIGHT) {

      nx = LEFT_PADDLE_X + 8;

      ball_dx = -ball_dx;

      ho = (signed char)((ny + 4) - (paddle1_y + 12));

      if(ho < -8) ball_dy = -3;

      else if(ho < -3) ball_dy = -2;

      else if(ho < 3) ball_dy = (ball_dy > 0) ? 1 : -1;

      else if(ho < 8) ball_dy = 2;

      else ball_dy = 3;

    }

  }

  if(ball_dx > 0 && nx + 8 >= RIGHT_PADDLE_X && ball_x + 8 < RIGHT_PADDLE_X) {

    if(ny + 8 > paddle2_y && ny < paddle2_y + PADDLE_HEIGHT) {

      nx = RIGHT_PADDLE_X - 8;

      ball_dx = -ball_dx;

      ho = (signed char)((ny + 4) - (paddle2_y + 12));

      if(ho < -8) ball_dy = -3;

      else if(ho < -3) ball_dy = -2;

      else if(ho < 3) ball_dy = (ball_dy > 0) ? 1 : -1;

      else if(ho < 8) ball_dy = 2;

      else ball_dy = 3;

    }

  }

  if(nx < 2) {

    score2++;

    game_state = STATE_SCORED;

    reset_ball();

    return;

  }

  if(nx > 246) {

    score1++;

    game_state = STATE_SCORED;

    reset_ball();

    return;

  }

  ball_x = nx;

  ball_y = ny;

}

void main(void) {

  ppu_off();

  upload_chr();

  pal_all(pal_data);

  bank_spr(1);

  bank_bg(0);

  game_state = STATE_TITLE;

  draw_title();

  oam_clear();

  ppu_on_all();

  while(1) {

    ppu_wait_nmi();

    frame_count++;

    switch(game_state) {

      case STATE_TITLE:

        oam_clear();

        pad = pad_poll(0);

        if(pad & PAD_START) {

          init_game();

          game_state = STATE_PLAYING;

          ppu_off();

          draw_playfield();

          ppu_on_all();

        }

        break;

      case STATE_PLAYING:

        update_player();

        update_ai();

        update_ball();

        draw_sprites();

        if(score1 >= WINNING_SCORE || score2 >= WINNING_SCORE) {

          game_state = STATE_GAMEOVER;

          ppu_off();

          draw_playfield();

          draw_gameover();

          ppu_on_all();

        }

        break;

      case STATE_SCORED:

        draw_sprites();

        ppu_off();

        vram_adr(NAMETABLE_A + 1*32 + 11);

        vram_put(score1 + T_DIG0);

        vram_adr(NAMETABLE_A + 1*32 + 20);

        vram_put(score2 + T_DIG0);

        ppu_on_all();

        game_state = STATE_PLAYING;

        break;

      case STATE_GAMEOVER:

        oam_clear();

        pad = pad_poll(0);

        if(pad & PAD_START) {

          game_state = STATE_TITLE;

          ppu_off();

          draw_title();

          ppu_on_all();

        }

        break;

    }

  }

}
