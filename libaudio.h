#ifndef lib_audio_h_
#define lib_audio_h_
void PlayAudio(int ,const char*, int);
int InitAudio(int ,int ,int);
int SetVolume(int ,int );
int PauseAudio(int );
int ResumeAudio(int );
int StopAudio(int );
void CloseAudio();
int GetChannelStatus(int );
int ClearChannelStatus(int );
int CheckTypeAudio(const char* );
#endif
