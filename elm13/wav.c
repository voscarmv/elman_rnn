/* wav.c */
/* whatwav.c
   Decode and display Microsoft .WAV file header.

   Usage:
      whatwav inputfilename

   Steve Moshier
   Contact via moshier@na-net.ornl.gov
   May, 2000  */

#include <stdio.h>

#ifdef unix
#include <unistd.h>
#else
#ifdef __GNUC__
#include <unistd.h>
#endif
#endif

#ifdef __BORLANDC__
#include <io.h>
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* Data structure of wave file header.  */
#include "wav.h"

/* Names of all registered WAVE formats.
   Reference: "WAVE and AVI Codec Registries,"
   Eric Fleischman, Microsoft Corporoation, February 19, 1998  */

struct fmt {
  int tag;
  char *text;
};

#define N_FORMATS 105

struct fmt formats[N_FORMATS] = {
    {   0, "WAVE_FORMAT_UNKNOWN Microsoft"},
    {   1, "WAVE_FORMAT_PCM Microsoft"},
    {   2, "WAVE_FORMAT_ADPCM Microsoft"},
    {   3, "WAVE_FORMAT_IEEE_FLOAT"},
    {   4, "WAVE_FORMAT_VSELP codec for Windows CE"},
    {   5, "WAVE_FORMAT_IBM_CVSD"},
    {   6, "WAVE_FORMAT_ALAW"},
    {   7, "WAVE_FORMAT_MULAW"},
    { 0x10, "WAVE_FORMAT_OKI_ADPCM"},
    { 0x11, "WAVE_FORMAT_DVI_ADPCM Intel"},
    { 0x12, "WAVE_FORMAT_MEDIASPACE_ADPCM Videologic"},
    { 0x13, "WAVE_FORMAT_SIERRA_ADPCM"},
    { 0x14, "WAVE_FORMAT_G723_ADPCM"},
    { 0x15, "WAVE_FORMAT_DIGISTD DSP Solution"},
    { 0x16, "WAVE_FORMAT_DIGIFIX DSP Solution"},
    { 0x17, "WAVE_FORMAT_DIALOGIC_OKI_ADPCM"},
    { 0x18, "WAVE_FORMAT_MEDIAVISION_ADPCM"},
    { 0x19, "WAVE_FORMAT_CU_CODEC hp"},
    { 0x20, "WAVE_FORMAT_YAMAHA_ADPCM"},
    { 0x21, "WAVE_FORMAT_SONARC"},
    { 0x22, "WAVE_FORMAT_DSPGROUP_TRUESPEECH"},
    { 0x23, "WAVE_FORMAT_ECHOSC1"},
    { 0x24, "WAVE_FORMAT_AUDIOFILE_AF36"},
    { 0x25, "WAVE_FORMAT_APTX Audio Process Technology"},
    { 0x26, "WAVE_FORMAT_AF10 Virtual Music Inc"},
    { 0x27, "WAVE_FORMAT_PROSODY_1612 Aculab plc"},
    { 0x28, "WAVE_FORMAT_LRC Merging Technologies S.A."},
    { 0x29, "WAVE_FORMAT_DOLBY_AC2"},
    { 0x30, "WAVE_FORMAT_GSM610"},
    { 0x31, "WAVE_FORMAT_MSNAUDIO"},
    { 0x32, "WAVE_FORMAT_ANTEX_ADPCME"},
    { 0x33, "WAVE_FORMAT_ANTEX_ADPCME"},
    { 0x34, "WAVE_FORMAT_CONTROL_RES_VQLPC"},
    { 0x35, "WAVE_FORMAT_DIGIREAL DSP Solutions"},
    { 0x36, "WAVE_FORMAT_DIGIADPCM DSP Solutions"},
    { 0x37, "WAVE_FORMAT_CONTROL_RES_CR10"},
    { 0x38, "WAVE_FORMAT_NMS_VBXADPCM"},
    { 0x39, "WAVE_FORMAT_ROLAND_RDAC"},
    { 0x3A, "WAVE_FORMAT_ECHOSC3"},
    { 0x3B, "WAVE_FORMAT_ROCKWELL_ADPCM"},
    { 0x3C, "WAVE_FORMAT_ROCKWELL_DIGITALK"},
    { 0x3D, "WAVE_FORMAT_XEBEC"},
    { 0x40, "WAVE_FORMAT_G721_ADPCM"},
    { 0x41, "WAVE_FORMAT_G728_CELP"},
    { 0x42, "WAVE_FORMAT_MSG723 Microsoft"},
    { 0x50, "WAVE_FORMAT_MPEG"},
    { 0x52, "WAVE_FORMAT_RT24"},
    { 0x53, "WAVE_FORMAT_PAC"},
    { 0x55, "WAVE_FORMAT_MPEGLAYER3"},
    { 0x59, "WAVE_FORMAT_LUCENT_G723"},
    { 0x60, "WAVE_FORMAT_CIRRUS"},
    { 0x61, "WAVE_FORMAT_ESPCM"},
    { 0x62, "WAVE_FORMAT_VOXWARE"},
    { 0x63, "WAVE_FORMAT_CANOPUS_ATRAC"},
    { 0x64, "WAVE_FORMAT_G726_ADPCM"},
    { 0x65, "WAVE_FORMAT_G722_ADPCM"},
    { 0x66, "WAVE_FORMAT_DSAT"},
    { 0x67, "WAVE_FORMAT_DSAT_DISPLAY"},
    { 0x69, "WAVE_FORMAT_VOXWARE_BYTE_ALIGNED"},
    { 0x70, "WAVE_FORMAT_VOXWARE_AC8"},
    { 0x71, "WAVE_FORMAT_VOXWARE_AC10"},
    { 0x72, "WAVE_FORMAT_VOXWARE_AC16"},
    { 0x73, "WAVE_FORMAT_VOXWARE_AC20"},
    { 0x74, "WAVE_FORMAT_VOXWARE_RT24"},
    { 0x75, "WAVE_FORMAT_VOXWARE_RT29"},
    { 0x76, "WAVE_FORMAT_VOXWARE_RT29HW"},
    { 0x77, "WAVE_FORMAT_VOXWARE_VR12"},
    { 0x78, "WAVE_FORMAT_VOXWARE_VR18"},
    { 0x79, "WAVE_FORMAT_VOXWARE_TQ40"},
    { 0x80, "WAVE_FORMAT_SOFTSOUND"},
    { 0x81, "WAVE_FORMAT_VOXWARE_TQ60"},
    { 0x82, "WAVE_FORMAT_VOXWARE_MSRT24"},
    { 0x83, "WAVE_FORMAT_G729A"},
    { 0x84, "WAVE_FORMAT_MVI_MV12"},
    { 0x85, "WAVE_FORMAT_DF_G726"},
    { 0x86, "WAVE_FORMAT_DF_GSM610"},
    { 0x88, "WAVE_FORMAT_ISIAUDIO"},
    { 0x89, "WAVE_FORMAT_ONLIVE"},
    { 0x91, "WAVE_FORMAT_SBC24"},
    { 0x92, "WAVE_FORMAT_DOLBY_AC3_SPDIF"},
    { 0x97, "WAVE_FORMAT_ZYXEL_ADPCM"},
    { 0x98, "WAVE_FORMAT_PHILIPS_LPCBB"},
    { 0x99, "WAVE_FORMAT_PACKED Studer"},
    { 0x100, "WAVE_FORMAT_RHETOREX_ADPCM"},
    { 0x101, "WAVE_FORMAT_IRAT"},
    { 0x111, "WAVE_FORMAT_VIVO_G723"},
    { 0x112, "WAVE_FORMAT_VIVO_SIREN"},
    { 0x123, "WAVE_FORMAT_DIGITAL_G723"},
    { 0x200, "WAVE_FORMAT_CREATIVE_ADPCM"},
    { 0x202, "WAVE_FORMAT_CREATIVE_FASTSPEECH8"},
    { 0x203, "WAVE_FORMAT_CREATIVE_FASTSPEECH10"},
    { 0x220, "WAVE_FORMAT_QUARTERDECK"},
    { 0x300, "WAVE_FORMAT_FM_TOWNS_SND"},
    { 0x400, "WAVE_FORMAT_BTV_DIGITAL"},
    { 0x680, "WAVE_FORMAT_VME_VMPCM"},
    { 0x1000, "WAVE_FORMAT_OLIADPCM"},
    { 0x1002, "WAVE_FORMAT_OLICELP"},
    { 0x1003, "WAVE_FORMAT_OLISBC"},
    { 0x1004, "WAVE_FORMAT_OLIOPR"},
    { 0x1100, "WAVE_FORMAT_LH_CODEC"},
    { 0x1400, "WAVE_FORMAT_NORRIS"},
    { 0x1401, "WAVE_FORMAT_ISIAudio"},
    { 0x1500, "WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS"},
    { 0x2000, "WAVE_FORMAT_DVM"},
    { 0xffff, "don't know"},
  };

/* I/O buffer  */ 
#define BSIZE 256
char data[BSIZE];


/* Write 4-byte long NUM to file F in little-endian byte order.  */

void
wllong (int f, long num)
{
  unsigned char c[4];

  c[0] = num & 0xff;
  c[1] = (num >> 8) & 0xff;
  c[2] = (num >> 16) & 0xff;
  c[3] = (num >> 24) & 0xff;
  write (f, c, 4);
}


/* Read 4-byte long NUM from file F in little-endian byte order.  */

void
rllong (int f, long *num)
{
  unsigned char c[4];
  long x;

  read (f, c, 4);
  x = (long) c[3];  /* high order byte */
  x = (x << 8) | c[2];
  x = (x << 8) | c[1];
  x = (x << 8) | c[0];
  *num = x;
}


/* Write 2-byte short NUM to file F in little-endian byte order.  */

void
wlshort (int f, int num)
{
  unsigned char c[2];

  c[0] = num & 0xff;
  c[1] = (num >> 8) & 0xff;
  write (f, c, 2);
}


/* Read 2-byte short NUM from file F in little-endian byte order.  */

void
rlshort (int f, short *num)
{
  unsigned char c[2];
  short x;

  read (f, c, 2);
  x = (short) c[1];
  x = (x << 8) | c[0];
  *num = x;
}

/* Structure filled in by read_wav_header.  */
static struct wav wav_header;
static int header_valid;

/* Read WAV header from current position in file FD (should be the beginning),
   parse the contents into HEADER, and return the number of bytes read
   from the file.  The actual audio data starts after that many bytes.
   Return -1 on error.  */

int
read_wav_header (int fd)
{
  int chunkbytes;
  int chars_read;

  header_valid = 0;
  read (fd, &wav_header.riff[0], 4);
  if (strncmp( &wav_header.riff[0], "RIFF", 4))
    goto headerr;
  chars_read = 4;
  rllong (fd, &wav_header.chunksize);
  chars_read += 4;
  read (fd, &wav_header.wave[0], 4);
  if (strncmp( &wav_header.wave[0], "WAVE", 4))
    goto headerr;
  chars_read += 4;
  read (fd, &wav_header.fmt[0], 4);
  if (strncmp( &wav_header.fmt[0], "fmt ", 4))
    goto headerr;
  chars_read += 4;
  rllong (fd, &wav_header.fmtchunksize);
  chars_read += 4;
  chunkbytes = wav_header.fmtchunksize;
  rlshort (fd, &wav_header.fmttag);
  chars_read += 2;
  chunkbytes -= 2;
  rlshort (fd, &wav_header.channels);
  chars_read += 2;
  chunkbytes -= 2;
  rllong (fd, &wav_header.rate);
  chars_read += 4;
  chunkbytes -= 4;
  rllong (fd, &wav_header.avebytespsec);
  chars_read += 4;
  chunkbytes -= 4;
  rlshort (fd, &wav_header.nBlockAlign);
  chars_read += 2;
  chunkbytes -= 2;
  rlshort (fd, &wav_header.sample_size);
  chars_read += 2;
  chunkbytes -= 2;
  if (chunkbytes > 0)
    {
      /* printf("Skipping %d unused format chunk bytes.\n", chunkbytes); */
      read (fd, data, chunkbytes);
      chars_read += chunkbytes;
    }
  read (fd, &wav_header.data[0], 4);
  if (strncmp( &wav_header.data[0], "data", 4))
    goto headerr;
  chars_read += 4;
  rllong (fd, &wav_header.datasize);
  chars_read += 4;
  wav_header.samples = wav_header.datasize
    / (wav_header.channels * (wav_header.sample_size / 8));
  header_valid = 1;
  return chars_read;;

headerr:
  header_valid = 0;
  return -1;
}


int
write_wav_header (int fd)
{
  int chunkbytes;
  int chars_read;
  /*
  if (!header_valid)
    return -1;
  */
  /* Move to start of file.  */
  /*  lseek (fd, 0, SEEK_SET);*/
  if (wav_header.fmtchunksize == 0)
    wav_header.fmtchunksize = 16;
  write (fd, "RIFF", 4);
  chars_read = 4;
  wllong (fd, wav_header.chunksize);
  chars_read += 4;
  write (fd, "WAVE", 4);
  chars_read += 4;
  write (fd, "fmt ", 4);
  chars_read += 4;
  wllong (fd, wav_header.fmtchunksize);
  chars_read += 4;
  chunkbytes = wav_header.fmtchunksize;
  wlshort (fd, wav_header.fmttag);
  chars_read += 2;
  chunkbytes -= 2;
  wlshort (fd, wav_header.channels);
  chars_read += 2;
  chunkbytes -= 2;
  wllong (fd, wav_header.rate);
  chars_read += 4;
  chunkbytes -= 4;
  wllong (fd, wav_header.avebytespsec);
  chars_read += 4;
  chunkbytes -= 4;
  wlshort (fd, wav_header.nBlockAlign);
  chars_read += 2;
  chunkbytes -= 2;
  wlshort (fd, wav_header.sample_size);
  chars_read += 2;
  chunkbytes -= 2;
  if (chunkbytes > 0)
    {
      /* printf("Skipping %d unused format chunk bytes.\n", chunkbytes); */
      write (fd, data, chunkbytes);
      chars_read += chunkbytes;
    }
  write (fd, "data", 4);
  chars_read += 4;
  wllong (fd, wav_header.datasize);
  chars_read += 4;
  return chars_read;;
}


/* Print the contents of HEADER.  */

void
print_wav_header ()
{
  int i;

//  printf ("Chunk size = %ld\n", wav_header.chunksize);
//  printf ("Format chunk size = %ld\n", wav_header.fmtchunksize);
//  printf ("Format tag is %d = ", wav_header.fmttag);
  for (i = 0; i < N_FORMATS; i++)
    {
      if (formats[i].tag == wav_header.fmttag)
	goto found_tag;
    }
  i = N_FORMATS - 1;
found_tag:
printf("\n");
//  printf ("%s\n", formats[i].text);
//  printf ("Number of channels = %d\n", wav_header.channels);
//  printf ("Sample rate = %ld\n", wav_header.rate);
//  printf ("Average bytes per second = %ld\n", wav_header.avebytespsec);
//  printf ("BlockAlign = %d\n", wav_header.nBlockAlign);
//  printf ("Sample size = %d\n", wav_header.sample_size);
//  printf ("Data size = %ld\n", wav_header.datasize);
}


#if 0
/* Test program.  */

int
main (argc, argv)
     int argc;
     char **argv;
{
  int i;
  int fin;
  struct wav header;

  if (argc < 2)
    {
      printf ("Usage: whatwav inputfile\n");
      exit (1);
    }

  /* Input file.  */
  fin = open (argv[1], O_BINARY | O_RDONLY, S_IREAD);
  if (fin <= 0)
    {
      printf ("? can't find <%s>\n", argv[1]);
      return (-1);
    }

  i = read_wav_header (fin, &header);
  if (i > 0)
    {
      printf ("Read %d bytes from header.\n", i);
      print_wav_header (&header);
    }
  else
    {
      printf ("? Error reading wav header.\n");
    }
  close (fin);
  exit (0);
}
#endif


/* Insert sample size, in bits, into the stored struct wav.  */

void
set_wav_sample_size (int sample_size)
{
  wav_header.sample_size = sample_size;
  wav_header.avebytespsec = wav_header.rate * wav_header.channels
    * (wav_header.sample_size / 8);
  wav_header.nBlockAlign = wav_header.channels * (wav_header.sample_size / 8);
  wav_header.datasize = wav_header.samples * wav_header.channels
    * (wav_header.sample_size / 8);
}


/* Insert n_channels into the stored struct wav. */

void
set_wav_n_channels (int n_channels)
{
  wav_header.channels = n_channels;
  wav_header.avebytespsec = wav_header.rate * wav_header.channels
    * (wav_header.sample_size / 8);
  wav_header.nBlockAlign = wav_header.channels * (wav_header.sample_size / 8);
}


/* Insert sampling rate in samples per second into the stored struct wav. */

void
set_wav_sample_rate (int sample_rate)
{
  wav_header.rate = sample_rate;
  wav_header.avebytespsec = sample_rate * wav_header.channels
    * (wav_header.sample_size / 8);
}


/* Set size of data area into the stored struct wav.  */

void
set_wav_datasize (long n_samples)
{
  wav_header.samples = n_samples;
  wav_header.datasize = n_samples * wav_header.channels
    * (wav_header.sample_size / 8);
  if (wav_header.fmtchunksize == 0)
    wav_header.fmtchunksize = 16;
  wav_header.chunksize = wav_header.datasize + wav_header.fmtchunksize + 20;
}


/* Set the tag value representing the encoding method.
   Refer to the struct fmt formats, above.  */

void
set_wav_format_tag (int tag)
{
  wav_header.fmttag = tag;
}


/* Return the sample size in bits (for 1 channel)  */

int
get_wav_sample_size ()
{
  if (!header_valid)
    return -1;

  return wav_header.sample_size;
}


/* Return the sample rate of the data, in Hertz.  */

int
get_wav_sample_rate ()
{
  if (!header_valid)
    return -1;

  return wav_header.rate;
}


/* Return the length of the data area, measured in samples.  */

long
get_wav_n_samples ()
{
  if (!header_valid)
    return -1;

  return (wav_header.datasize
	  / (wav_header.channels * (wav_header.sample_size / 8)));
}
