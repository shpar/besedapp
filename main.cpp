//
//  besedapp
//
//  scrolling random archaic slovenian words on screen
//  the ultimate inspirational tool for writing rap lyrics in 18. century slovenian
//
//  created by Dan on 04.06.17.
//

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#define MAX_WORDS 4064
#define MAX_WORDS_ON_SCREEN 20
#define MAX_VELOCITY 8

using namespace std;

struct Word
{
    int x;
    int y;
    int velocity;
    const char * word;
};

int getRandomNumber(int min, int max);
void getNewWords(vector<Word>& words, string besede[], int height, int& numWords);

int main(int argc, char **argv) {
     
    // load words
    string besede[MAX_WORDS];
    ifstream inData;
    inData.open("besede.txt");
    for (int i = 0; i < MAX_WORDS; i++){
        inData >> besede[i];
    }
    inData.close();

    // set up display
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_DISPLAY_MODE disp_data;
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_get_display_mode(0, &disp_data);

    display = al_create_display(640, 480);
    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, 1);

    ALLEGRO_FONT* font = al_load_font("/Library/Fonts/Futura.ttc", 40, NULL);
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *event_queue;
     
    timer = al_create_timer(1.0/60);
    event_queue = al_create_event_queue();
    al_start_timer(timer);
    al_install_keyboard();
    al_set_app_name("Besedapp");

    int WIDTH = al_get_display_width(display);
    int HEIGHT = al_get_display_height(display);
    
    int numWords = 0;
    vector<Word> words;
    getNewWords(words, besede, HEIGHT, numWords);
    
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
     
    int fps = 0, fps_accum = 0;
    double fps_time = 0;
    bool doexit = false;
    bool redraw = false;
    
    // initial color levels
    float h = 36.0;
    float s = 1.0;
    float l = 0.5;
    
    while(!doexit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
         
        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            for (int i = 0; i < numWords; i++)
            {
                if(words[i].x > WIDTH)
                {
                    words.erase(words.begin() + i);
                    numWords--;
                    getNewWords(words, besede, HEIGHT, numWords);
                }
                else words[i].x += words[i].velocity;
                {
                    redraw = true;
                }
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_RIGHT:
                {
                    if (h < 360.0)
                    {
                        h += 1;
                    }
                }
                break;
                case ALLEGRO_KEY_LEFT:
                {
                    if (h > 0)
                    {
                        h -= 1;
                    }
                }
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                doexit = true;
                break;
            }
        }
         
        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            al_clear_to_color(al_color_hsl(h, s, l));
            double t = al_get_time();
            for (int i = 0; i < numWords; i++)
            {
                al_draw_text(font, al_color_html("#EFEFEF"), words[i].x, words[i].y, 0,
                         words[i].word);
            }
            al_flip_display();
            fps_accum++;
            if(t - fps_time >= 1)
            {
                fps = fps_accum;
                fps_accum = 0;
                fps_time = t;
            }
        }
    }
    
    al_destroy_font(font);
    al_destroy_display(display);
    return 0;
}

// get new words from dictionary
void getNewWords(vector<Word>& words, string besede[], int height, int& numWords)
{
    int newNumWords = getRandomNumber(numWords + 1, MAX_WORDS_ON_SCREEN);
    for (int i = numWords; i < newNumWords ; i++)
    {
        words.push_back(Word());
        words[i].x = getRandomNumber(-400, -200);
        words[i].y = getRandomNumber(40, height - 40);
        words[i].velocity = getRandomNumber(1, MAX_VELOCITY);
        words[i].word = besede[getRandomNumber(0, MAX_WORDS - 1)].c_str();
    }
    numWords = newNumWords;
}

// get random number
int getRandomNumber(int min, int max)
{
    mt19937 rng;
    rng.seed(random_device()());
    uniform_int_distribution<mt19937::result_type> dist6(min, max);
    return dist6(rng);
}
