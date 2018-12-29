/*
 #
 #  File        : mulpanim.cpp
 #                ( C++ source file )
 #
 #  Description : Creates an animation of modulo multiplication tables around a circle
 #
 #  Copyright   : Mandy Wilkens
 #
 #  License     : CeCILL v2.0
 #                ( http://www.cecill.info/licences/Licence_CeCILL_V2-en.html )
 #
 #  This software is governed by the CeCILL  license under French law and
 #  abiding by the rules of distribution of free software.  You can  use,
 #  modify and/ or redistribute the software under the terms of the CeCILL
 #  license as circulated by CEA, CNRS and INRIA at the following URL
 #  "http://www.cecill.info".
 #
 #  As a counterpart to the access to the source code and  rights to copy,
 #  modify and redistribute granted by the license, users are provided only
 #  with a limited warranty  and the software's author,  the holder of the
 #  economic rights,  and the successive licensors  have only  limited
 #  liability.
 #
 #  In this respect, the user's attention is drawn to the risks associated
 #  with loading,  using,  modifying and/or developing or reproducing the
 #  software by the user in light of its specific status of free software,
 #  that may mean  that it is complicated to manipulate,  and  that  also
 #  therefore means  that it is reserved for developers  and  experienced
 #  professionals having in-depth computer knowledge. Users are therefore
 #  encouraged to load and test the software's suitability as regards their
 #  requirements in conditions enabling the security of their systems and/or
 #  data to be ensured and,  more generally, to use and operate it in the
 #  same conditions as regards security.
 #
 #  The fact that you are presently reading this means that you have had
 #  knowledge of the CeCILL license and that you accept its terms.
 #
*/

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <math.h>

#define KEY_SEEN 1
#define KEY_RELEASED 2

#define WIDTH  1500
#define HEIGHT 1500

// comment out if you don't want debug text
#define DEBUG

#define RAD WIDTH/2.2
#define N 200

void must_init (bool test, const char *description){
    if(test)return;
    printf("Couldn't Initialize %s\n", description);
    exit(1);
}

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

struct Point{
    int x;
    int y;
};

void getPt(float n, int maxN, int radius, Point * p){
    float th = 2 * M_PI * (n/maxN);
    p->x = radius * cos(th) + WIDTH/2;
    p->y = radius * sin(th) + HEIGHT/2; 
}

// Main
// -----
int main(int argc, char **argv) {

    // The dimentions of the screen
    const unsigned int Wi = WIDTH, Hi = HEIGHT;
   
    must_init(al_init(),"allegro");
    must_init(al_install_keyboard(),"keyboard");
    must_init(al_init_primitives_addon(), "primitives");

    ALLEGRO_TIMER *timer = al_create_timer(1.0/30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue,"queue");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    // Main program window
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR); 
    ALLEGRO_DISPLAY *main_disp = al_create_display(Wi,Hi);
    must_init(main_disp, "display");

    // Configuring events

    bool redraw = true;
    bool running = true;

    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key,0,sizeof(key));

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(main_disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    int num_pt = N;
    double multiplier = 1;
    double speed = 0.05;
    bool anim = true;

    char fn[20]; int file = 0;

    // ---------------------
    // The main loop
    // ---------------------
    al_start_timer(timer);
    while (running) {
        al_wait_for_event(queue,&event);

        switch(event.type){
            case ALLEGRO_EVENT_TIMER:

                // Key events
                if(key[ALLEGRO_KEY_ESCAPE] || key[ALLEGRO_KEY_Q]){
                    running = false; // quit the game
                }

                if(key[ALLEGRO_KEY_LEFT] & KEY_RELEASED == KEY_RELEASED){
                    multiplier = (multiplier==0) ? 0 : multiplier - 1 ;
                }  
                if(key[ALLEGRO_KEY_RIGHT] & KEY_RELEASED == KEY_RELEASED){
                    multiplier += 1;
                }

                if(key[ALLEGRO_KEY_DOWN] & KEY_RELEASED == KEY_RELEASED){
                    num_pt = (num_pt==0) ? 0 : num_pt - 10 ;
                }  
                if(key[ALLEGRO_KEY_UP] & KEY_RELEASED == KEY_RELEASED){
                    num_pt += 10;
                }

                if(key[ALLEGRO_KEY_N] & KEY_RELEASED == KEY_RELEASED){
                    speed = (speed==0) ? 0 : speed * 0.5 ;
                }  
                if(key[ALLEGRO_KEY_M] & KEY_RELEASED == KEY_RELEASED){
                    speed *= 2;
                }

                if(key[ALLEGRO_KEY_R] & KEY_RELEASED == KEY_RELEASED){
                    speed = -speed;
                }

                if(key[ALLEGRO_KEY_B] & KEY_RELEASED == KEY_RELEASED){
                    multiplier = round(multiplier);
                }

                if(key[ALLEGRO_KEY_SPACE] & KEY_RELEASED == KEY_RELEASED){
                    anim = !anim;
                }

                for( int i = 0; i < ALLEGRO_KEY_MAX; i++) key[i] &= 0;

                redraw = true;

                if( anim ) multiplier = (multiplier > num_pt) ? multiplier = 1 : multiplier + speed;

                break;

            // dont worry about what this means lol
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN;
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] |= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                break;
        }

        if (redraw && al_is_event_queue_empty(queue)){
            // main event loop
            al_clear_to_color(al_map_rgb(0,0,0));

#ifdef DEBUG
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Multiplier = %2f", multiplier);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 15, 0, "Number of Points = %d", num_pt);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 30, 0, "Animating? %d", anim);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 45, 0, "Speed = %f", speed);
#endif


            // draw our circle
            al_draw_circle(WIDTH/2, HEIGHT/2, RAD, al_map_rgb(50, 50, 50), 6);

            for (int i = 0; i < num_pt; i++){
                // draw each circle
                Point p;

                #ifdef DEBUG_LABELS
                // Label each point on the circle
                getPt(i, num_pt, RAD+30, &p);
                int flag = (i < (num_pt/4) || i > (3*num_pt/4)) ? ALLEGRO_ALIGN_LEFT : ALLEGRO_ALIGN_RIGHT;
                al_draw_textf(font, al_map_rgb(255, 255, 255), p.x, p.y, flag, "%3d", i);
                #endif

                getPt(i, num_pt, RAD, &p);
                al_draw_filled_circle(p.x, p.y, 3, al_map_rgb(255, 255, 255));

                // calculate the multiple modulo
                float ni = fmod(i*multiplier,num_pt);
                // draw the line between the two
                Point np;
                getPt(ni, num_pt, RAD, &np);

                // Calculate color
                hsv col_map;
                col_map.h = 360.0 * fmod(multiplier,50) / 50;
                col_map.s = 0.5; col_map.v = 1.0;
                rgb col = hsv2rgb(col_map);
                
                al_draw_line(p.x,p.y,np.x,np.y,al_map_rgb(255*col.r,255*col.g,255*col.b),2);
            }

            // attempted backbuffer saving for making gifs, failed though
            /*ALLEGRO_BITMAP * bmp = al_get_backbuffer(main_disp);
            snprintf(fn,20,"gif/%d.jpg",file);
            if ( !al_save_bitmap(fn,bmp) ) printf("Saving file %s failed!!!\n", fn);
            file++;*/

            al_flip_display();
            redraw = false;
        }
    }
    al_destroy_display(main_disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);

    // End the game
    return 0;
}

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}