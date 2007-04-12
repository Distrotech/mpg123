/*
	audio_esd.c: audio output for ESounD (highly untested nowadays (?))

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.de
	initially written by Eric B. Mitchell ("esd port" should be this file...)
*/

#include <esd.h>
#include <errno.h>
#include <assert.h>

#include "config.h"
#include "mpg123.h"

#ifdef SOLARIS
#include <stropts.h>
#include <sys/conf.h>
#endif

static unsigned esd_rate = 0, esd_format = 0, esd_channels = 0;
static char *esdserver = NULL;

int audio_open(audio_output_t *ao)
{
  esd_format_t format = ESD_STREAM | ESD_PLAY;

  if (!esd_rate)
  {
    int esd;
    esd_server_info_t *info;
    esd_format_t fmt;

    if ((esd = esd_open_sound(NULL)) >= 0)
    {
      info = esd_get_server_info(esd);
      esd_rate = info->rate;
      fmt = info->format;
      esd_free_server_info(info);
      esd_close(esd);
    }
    else
    {
      esd_rate = esd_audio_rate;
      fmt = esd_audio_format;
    }
    esd_format = AUDIO_FORMAT_UNSIGNED_8;
    if ((fmt & ESD_MASK_BITS) == ESD_BITS16)
      esd_format |= AUDIO_FORMAT_SIGNED_16;
    esd_channels = fmt & ESD_MASK_CHAN;
  }

  if (ao->format == -1)
    ao->format = esd_format;
  else if (!(ao->format & esd_format))
  {
    fprintf(stderr, "audio: Unsupported audio format: %d\n", ao->format);
    errno = EINVAL;
    return -1;
  }
  if (ao->format & AUDIO_FORMAT_SIGNED_16)
    format |= ESD_BITS16;
  else if (ao->format & AUDIO_FORMAT_UNSIGNED_8)
    format |= ESD_BITS8;
  else
    assert(0);

  if (ao->channels == -1)
    ao->channels = 2;
  else if (ao->channels <= 0 || ao->channels > esd_channels)
  {
    fprintf(stderr, "audio: Unsupported no of channels: %d\n", ao->channels);
    errno = EINVAL;
    return -1;
  }
  if (ao->channels == 1)
    format |= ESD_MONO;
  else if (ao->channels == 2)
    format |= ESD_STEREO;
  else
    assert(0);
  
  if (ao->rate == -1)
    ao->rate = esd_rate;
  else if (ao->rate > esd_rate)
    return -1;

  ao->fn = esd_play_stream_fallback(format, ao->rate, ao->device, "mpg123");
  return (ao->fn);
}

int audio_get_formats(audio_output_t *ao)
{
  if (0 < ao->channels && ao->channels <= esd_channels
      && 0 < ao->rate && ao->rate <= esd_rate)
    return esd_format;
  else
    return -1;
}

int audio_play_samples(audio_output_t *ao,unsigned char *buf,int len)
{
  return write(ao->fn,buf,len);
}

int audio_close(audio_output_t *ao)
{
  close (ao->fn);
  return 0;
}

#ifdef SOLARIS
void audio_queueflush (audio_output_t *ao)
{
        ioctl (ao->fn, I_FLUSH, FLUSHRW);
}
#else
#ifdef NETBSD
void audio_queueflush (audio_output_t *ao)
{
        ioctl (ao->fn, AUDIO_FLUSH, 0);
}
#else
/* Dunno what to do on Linux and Cygwin, but the func must be at least defined! */
void audio_queueflush (audio_output_t *ao)
{
}
#endif
#endif
