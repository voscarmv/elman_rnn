/* Wave file header data structure.
   Numbers in the file are in little-endian byte order.  */

struct wav
{
  char riff[4];    /* ASCII "RIFF"  */
  long chunksize;  /* datasize + 8 + 16 + 12  */
  char wave[4];    /* ASCII "WAVE" */
  char fmt[4];     /* ASCII "fmt " */
  long fmtchunksize;  /* fmt chunk size 16 */
  short fmttag;    /* 1 = wave format PCM */
  short channels;
  long rate;
  long avebytespsec; /* rate * channels * samsize / 8  */
  short nBlockAlign; /* channels * samsize / 8  */
  short sample_size;  /* bits per sample of one channel. */
  char data[4];     /* ASCII "data"  */
  long datasize;   /* data chunk size  channels * samples * (samsize / 8)  */
  long samples;
};



/* Prototypes for wav.c.  */

/* Read wave header into a stored struct wav.  */
int read_wav_header (int fd);

/* Write out the stored wave header to file descriptor fd.  */
int write_wav_header (int fd);

/* Print current contents of stored wave header.  */
extern void print_wav_header ();

/* Insert n_channels into the stored struct wav. */
extern void set_wav_n_channels (int n_channels);

/* Insert sample size, in bits, into the stored struct wav.  */
extern void set_wav_sample_size (int sample_size);

/* Insert sampling rate in samples per second into the stored struct wav. */
extern void set_wav_sample_rate (int sample_rate);

/* Set size of data area into the stored struct wav.  */
extern void set_wav_datasize (long n_samples);

/* Return the sample size in bits (for 1 channel)  */
extern int get_wav_sample_size ();

/* Return the sample rate of the data, in Hertz.  */
extern int get_wav_sample_rate ();

/* Return the length of the data area, measured in samples.  */
extern long get_wav_n_samples ();

/* Set the tag value representing the encoding method.  */
extern void set_wav_format_tag (int tag);
