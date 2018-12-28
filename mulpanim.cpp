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

#define RAD WIDTH/2.2
#define N 400
#define MULP 2

void must_init (bool test, const char *description){
    if(test)return;
    printf("Couldn't Initialize %s\n", description);
    exit(1);
}

struct Point{
    int x;
    int y;
};

void getPt(float n, int maxN, Point * p){
    float th = 2 * M_PI * (n/maxN);
    p->x = RAD * cos(th) + WIDTH/2;
    p->y = RAD * sin(th) + HEIGHT/2; 
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
    char disp[50];

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
                    multiplier = (multiplier==0) ? 0 : multiplier - 0.1 ;
                }  
                if(key[ALLEGRO_KEY_RIGHT] & KEY_RELEASED == KEY_RELEASED){
                    multiplier += 0.1;
                }

                if(key[ALLEGRO_KEY_UP] & KEY_RELEASED == KEY_RELEASED){
                    num_pt = (num_pt==0) ? 0 : num_pt - 10 ;
                }  
                if(key[ALLEGRO_KEY_DOWN] & KEY_RELEASED == KEY_RELEASED){
                    num_pt += 10;
                }

                for( int i = 0; i < ALLEGRO_KEY_MAX; i++) key[i] &= 0;

                redraw = true;

                multiplier = (multiplier > num_pt) ? multiplier = 1 : multiplier + 0.05;

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

            snprintf(disp, 50, "Multiplier = %2f Number of Points = %d", multiplier, num_pt);
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, disp);

            // draw our circle
            al_draw_circle(WIDTH/2, HEIGHT/2, RAD, al_map_rgb(255, 0, 255), 2);

            for (int i = 0; i < num_pt; i++){
                // draw each circle
                Point p;
                getPt(i, num_pt, &p);
                al_draw_circle(p.x, p.y, 2, al_map_rgb(255, 255, 255), 1);

                // calculate the multiple modulo
                float ni = fmod(i*multiplier,N);
                // draw the line between the two
                Point np;
                getPt(ni, num_pt, &np);
                
                al_draw_line(p.x,p.y,np.x,np.y,al_map_rgb(multiplier,255,255),1);
            }

            al_flip_display();
            redraw = false;
        }
    }
    al_destroy_display(main_disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    // End the game
    return 0;
}
