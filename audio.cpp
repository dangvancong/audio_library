#include<iostream>
#include<string.h>
#include <unistd.h>
using namespace std;
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "libaudio.h"

//Get status channel of handle play audio
static int channel_sts[10]; 
 
//Sound Effect
Mix_Chunk* chunk = NULL;

/********************************************************************/
/* Function  : InitAudio                                            */
/* Description : Initial audio file                                 */
/* Parameter : int frequency, int channels, int chunksize           */
/*     frequency: Output sampling frequency in samples per second   */
/*     channels : Output sample format                              */
/*     chunksize: Bytes used per output sample                      */
/* Return value : int                                               */
/*     0: success                                                   */
/*     1: eror Initialize subsystem                                 */
/*     2: eror setup audio stream                                   */
/********************************************************************/
int InitAudio(int frequency, int channels, int chunksize)
{
    int result;
    // Initialize the SDL library with the Video subsystem
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO );
    if( result < 0)
	return 1;
    
    // Set up the audio stream
    result = Mix_OpenAudio(frequency, AUDIO_S16SYS, channels, chunksize);
    if( result < 0 )
	return 2;
    return 0;
}
/********************************************************************/
/* Function  : PlayAudio                                            */
/* Description : Play a audio file (.wav,.mp3 file)                 */
/* Parameter : int type, const char* path, int loop                 */
/*   channel : channel to play audio file ( 0, 1,2 ...)             */
/*   path : path of audio file                                      */
/*   loop : Number of loops, -1 is infinite loops                   */
/* Return value : void                                              */
/********************************************************************/
void PlayAudio(int channel, const char* path, int loop)
{
    if((channel < 0) || (Mix_Playing(channel) != 0))
    {
      channel_sts[channel] = 2;
      return;
    }

    int checktype = CheckTypeAudio(path);
    if(checktype != 0)
    {
      channel_sts[channel] = 3;
      return;
    }

    chunk = Mix_LoadWAV(path);
    if( chunk == NULL )
    {
      channel_sts[channel] = 4;
      return;
    }
    Mix_VolumeChunk(chunk, 3);
    Mix_PlayChannel(channel, chunk, loop);
    while (Mix_Playing(channel))
    {}
    channel_sts[channel] = 1;
}

/********************************************************************/
/* Function  : SetVolume                                            */
/* Description : Setup volume of audio                              */
/* Parameter : int channel, int volume                              */
/*    channel to pause audio(0, 1,2 ...)                            */
/*    volume : value to setup volume of audio (0-100)%              */
/*             if volume > 100, set is 100                          */
/*             if volume < 0, volume will not be set                */
/* Return value : int                                               */
/*    0: success                                                    */
/*   -1: eror                                                       */
/********************************************************************/
int SetVolume(int channel, int volume)
{
  int vol = (volume * 128) / 100;
  if(channel >= 0)
  {
    Mix_Volume(channel, vol);
    return 0;
  }
  else 
    return -1; 
}

/********************************************************************/
/* Function  : PauseAudio                                           */
/* Description : Pause audio                                        */
/* Parameter : int channel                                          */
/*    channel to pause audio(0, 1,2 ...)                            */
/* Return value : int                                               */
/*    0: success                                                    */
/*   -1: eror                                                       */
/********************************************************************/
int PauseAudio(int channel)
{
  if((Mix_Playing(channel) != 0) && (channel >= 0))
  {
    Mix_Pause(channel);
    return 0;
  }
  else
    return -1;
}

/********************************************************************/
/* Function  : ResumeAudio                                          */
/* Description : Resum audio                                        */
/* Parameter : int channel                                          */
/*    channel to resume audio(0, 1,2 ...)                           */
/* Return value : int                                               */
/*    0: success                                                    */
/*   -1: eror                                                       */
/********************************************************************/
int ResumeAudio(int channel)
{
  if((Mix_Paused(channel) != 0) && (channel >= 0))
  {
    Mix_Resume(channel);
    return 0;
  }
  else
    return -1;
}

/********************************************************************/
/* Function  : StopAudio                                            */
/* Description : Stop audio                                         */
/* Parameter : int channel                                          */
/*    channel to stop audio (0, 1,2 ...)                            */
/* Return value : int                                               */
/*    0: success                                                    */
/*   -1: eror                                                       */
/********************************************************************/
int StopAudio(int channel)
{
  if((Mix_Playing(channel) != 0) && (channel >= 0))
  {
    Mix_HaltChannel(channel);
    return 0;
  }
  else
    return -1;
}

/********************************************************************/
/* Function  : CloseAudio                                           */
/* Description : Close audio                                        */
/* Parameter : void                                                 */
/* Return value : void                                              */
/********************************************************************/
void CloseAudio()
{
    Mix_CloseAudio();
    SDL_Quit();
}

/********************************************************************/
/* Function  : GetChannelStatus                                     */
/* Description : Get status of channel play audio                   */
/* Parameter : int channel                                          */
/*    channel to stop audio (0, 1,2 ...)                            */
/* Return value : int                                               */
/* Return value : int                                               */
/*     -1: channel is playing audio                                 */
/*      0: reserve channnel or not play                             */
/*      1: play audio success                                       */
/*      2: eror channel( this channel is playing or channel < 0 )   */
/*      3: eror type of audio file                                  */
/*      4: eror load audio file(read data)                          */
/*      5: channel is paused                                        */
/********************************************************************/
int GetChannelStatus(int channel)
{
  if(channel < 0)
    return 2;
  if(Mix_Paused(channel) != 0)
    channel_sts[channel] = 5;
  else if(Mix_Playing(channel) != 0)
    channel_sts[channel] = -1;
  return channel_sts[channel];
}

/********************************************************************/
/* Function  : ClearChannelStatus                                   */
/* Description : Clear status of channel                            */
/* Parameter : int channel                                          */
/*         channel to clear status(0, 1,2 ...)                      */
/* Return value : int                                               */
/*       -1: clear eror                                             */
/*        0: clear success                                          */
/********************************************************************/
int ClearChannelStatus(int channel)
{
    if(channel < 0)
      return -1;
    channel_sts[channel] = 0;
    return 0;
}

/********************************************************************/
/* Function  : CheckTypeAudio                                       */
/* Description : Check type of audio file                           */
/* Parameter : const char* path                                     */
/*      path: path of audio file                                    */
/* Return value : int                                               */
/*      0: if path has format ".mp3" or ".wav"                      */
/*     -1: if path has other format                                 */
/********************************************************************/
int CheckTypeAudio(const char* path)
{
  string str1(path);
  int len = strlen(path);
  string str2 = str1.substr(len-4);
  char* ch = &str2[0];
  if((strcmp(".mp3", ch) == 0) || (strcmp(".wav", ch) == 0))
     return 0;
  else return -1;
}
