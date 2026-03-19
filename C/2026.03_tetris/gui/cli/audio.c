#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

Mix_Music* init_audio(const char* filename) {
    Mix_Music* music = NULL;
    int sdl_ok = 0;
    int mixer_ok = 0;
    int load_ok = 0;

    if (SDL_Init(SDL_INIT_AUDIO) >= 0) {
        sdl_ok = 1;
    }

    if (sdl_ok) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0) {
            mixer_ok = 1;
        }
    }

    if (mixer_ok) {
        music = Mix_LoadMUS(filename);
        if (music) {
            load_ok = 1;
        }
    }

    if (!(sdl_ok && mixer_ok && load_ok)) {
        if (music) {
            Mix_FreeMusic(music);
            music = NULL;
        }
        if (mixer_ok) {
            Mix_CloseAudio();
        }
        if (sdl_ok) {
            SDL_Quit();
        }
    }
    return music;
}

void play_music_once(Mix_Music* music) {
    if (music) {
        Mix_PlayMusic(music, 1);  // один раз
    }
}

int is_music_playing(const Mix_Music* music) {
    int playing = 0;
    if (music) {
        if (Mix_PlayingMusic() == 1) {
            playing = 1;
        }
    }
    return playing;
}

void free_audio(Mix_Music* music) {
    if (music) {
        Mix_FreeMusic(music);
    }
    Mix_CloseAudio();
    SDL_Quit();
}