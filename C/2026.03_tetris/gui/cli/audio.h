#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

Mix_Music* init_audio(const char* filename);
void play_music_once(Mix_Music* music);
int is_music_playing(const Mix_Music* music);
void free_audio(Mix_Music* music);

#endif