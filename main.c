/* A silly calculator -- main game code
 *
 * This software is licensed under a silly license:
 *
 * Copyright 2026 Mibi88
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * 4. If you meet one of the copyright holders in a bar and you like this
 * software, consider offering them a beer/a drink -- but you don't need to.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "gfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#define DISPLAY_LEN 9

#define DIGIT_COUNT 10
#define OPERATOR_COUNT 6

#define BALL_COUNT (DIGIT_COUNT+OPERATOR_COUNT)

static struct image balls[BALL_COUNT];
static struct image cards[DIGIT_COUNT];

static struct image white_ball;

static struct {
    float x;
    float y;

    float vx;
    float vy;
} ball_positions[BALL_COUNT];

static struct {
    float x;
    float y;

    float vx;
    float vy;
} white;

static char digits[DISPLAY_LEN];

static char *ball_images[BALL_COUNT] = {
    "ball_0.png",
    "ball_1.png",
    "ball_2.png",
    "ball_3.png",
    "ball_4.png",
    "ball_5.png",
    "ball_6.png",
    "ball_7.png",
    "ball_8.png",
    "ball_9.png",

    "ball_eq.png",
    "ball_add.png",
    "ball_sub.png",
    "ball_mul.png",
    "ball_div.png",
    "ball_pow.png",
};

static char *cards_images[DIGIT_COUNT] = {
    "card_0.png",
    "card_1.png",
    "card_2.png",
    "card_3.png",
    "card_4.png",
    "card_5.png",
    "card_6.png",
    "card_7.png",
    "card_8.png",
    "card_9.png"
};

#define ABS(x) ((x) < 0 ? -(x) : (x))
static void render(unsigned long ms, void *data) {
    int width;
    int height;

    gfx_get_size(&width, &height);

    {
        size_t i;
        int w = 0;
        int x;

        for(i=0;i<DISPLAY_LEN;i++){
            w += cards[digits[i]-'0'].w;
        }

        x = (width-w)/2;

        for(i=0;i<DISPLAY_LEN;i++){
            gfx_image(cards+digits[i]-'0', x, 0);
            x += cards[digits[i]-'0'].w;
        }
    }

    {
        size_t i;

        for(i=0;i<BALL_COUNT;i++){
            gfx_image(balls+i, ball_positions[i].x, ball_positions[i].y);

            /*if(ball_positions[i].x > width-balls[i].w){
                ball_positions[i].x = width-balls[i].w;
            }
            if(ball_positions[i].y > height-balls[i].h){
                ball_positions[i].y = height-balls[i].h;
            }*/
        }
    }

    gfx_image(&white_ball, white.x, white.y);

    if(ABS(white.vx) < 0.05 && ABS(white.vy) < 0.05){
        int x, y;
        int px = white.x+white_ball.w/2;
        int py = white.y+white_ball.h/2;

        int sx;
        int sy;

        int ex;
        int ey;

        float a;

        gfx_get_mouse_position(&x, &y);

        a = -atan2(px-x, py-y)-3.14/2;

        sx = px+cos(a)*white_ball.w/2;
        sy = py+sin(a)*white_ball.w/2;

        ex = px+cos(a)*200;
        ey = py+sin(a)*200;

        gfx_line(sx, sy, ex, ey, 150, 66, 1, 255, 10);

        if(gfx_buttondown(B_LEFT)){
            white.vx = -cos(a)*3;
            white.vy = -sin(a)*3;
        }
    }

    white.x += white.vx;
    white.y += white.vy;

    if(white.x > width-white_ball.w && white.vx > 0){
        white.vx = -white.vx;
        white.x = width-white_ball.w;
    }
    if(white.x < white_ball.w && white.vx < 0){
        white.vx = -white.vx;
        white.x = white_ball.w;
    }
    if(white.y > height-white_ball.h && white.vy > 0){
        white.vy = -white.vy;
        white.y = height-white_ball.h;
    }
    if(white.y < white_ball.h+cards->h && white.vy < 0){
        white.vy = -white.vy;
        white.y = white_ball.h+cards->h;
    }
}

int main(int argc, char **argv) {
    if(gfx_init(640, 480, "Billard calculator")){
        fprintf(stderr, "%s: Failed to open window!\n", *argv);

        return 1;
    }

    memset(digits, '0', DISPLAY_LEN);

    {
        size_t i;

        for(i=0;i<DIGIT_COUNT;i++){
            /* FIXME: Free loaded images on failure. */

            if(gfx_load_image(balls+i, ball_images[i])){
                fprintf(stderr, "%s: Failed to load image!\n", *argv);

                return 1;
            }
            if(gfx_load_image(cards+i, cards_images[i])){
                fprintf(stderr, "%s: Failed to load image!\n", *argv);

                return 1;
            }
        }

        if(gfx_load_image(&white_ball, "white_ball.png")){
            fprintf(stderr, "%s: Failed to load image!\n", *argv);

            return 1;
        }
    }

    white.x = white_ball.w;
    white.y = (480-white_ball.h-cards->h)/2;

    white.vx = 0;
    white.vy = 0;

    {
        size_t i;

        for(i=0;i<BALL_COUNT;i++){
            ball_positions[i].x = (640-4*balls->w)/2+(i%4)*balls->w;
            ball_positions[i].y = (480-4*balls->h-cards->h)+(i/4)*balls->h;
        }
    }

    gfx_clear_color(0, 127, 0);

    gfx_mainloop(render, NULL);

    gfx_free();

    return 0;
}
