/*
 * swappoff - turning off swapspace and scramble the data on the device
 *
 * $Id: swapoff.c,v 1.2 2000/04/11 08:54:24 drt Exp $
 *
 * Based on mkswap and swapon
 *
 * (C) 1991 Linus Torvalds. This file may be redistributed as per
 * the Linux copyright.
 *
 * mkswap and swapon where hacked by:
 * Linus Torvalds, jaggy@purplet.demon.co.uk (Mike Jagdis),
 * jj@ultra.linux.cz (Jakub Jelinek), aeb, Arkadiusz Mi¶kiewicz
 * <misiek@misiek.eu.org>, jrs, <Vincent.Renardias@waw.com>, Arnaldo
 * Carvalho de Melo <acme@conectiva.com.br>, jj
 *    
 * swapoff was hacked 1999 by Doobee R. Tzeck <drt@ailis.de>
 *
 * $Log: swapoff.c,v $
 * Revision 1.2  2000/04/11 08:54:24  drt
 * swapoff 0.01a
 *
 * Revision 1.1  2000/02/19 15:33:48  drt
 * Initial revision
 *
 *
 */ 

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <mntent.h> 
#include <signal.h>
#include <sys/ioctl.h>		/* for _IO */
#include <sys/utsname.h>
#include <sys/stat.h>
#include <asm/page.h>		/* for PAGE_SIZE and PAGE_SHIFT */
				/* we also get PAGE_SIZE via getpagesize() */
#include <sys/mman.h>
#include <sys/swap.h>

/* Definitions for Tiger */
void tiger(unsigned char *, unsigned long long int, unsigned long long int*);  

/* Definitions for Rijandael */
#define MAXKC			(256/32)
#define MAXROUNDS               14
typedef unsigned char		word8;	
typedef unsigned short		word16;	
typedef unsigned int		word32;
extern int rijndaelKeySched ( unsigned char *, int keyBits, word8 rk[MAXROUNDS+1][4][4]);
extern int rijndaelKeyEnctoDec (int keyBits, word8 W[MAXROUNDS+1][4][4]);
extern int rijndaelEncrypt (word8 a[16], word8 b[16], word8 rk[MAXROUNDS+1][4][4]);
extern int rijndaelDecrypt (word8 a[16], word8 b[16], word8 rk[MAXROUNDS+1][4][4]);


/* definitions for the Mersenne Twister*/
void seedMT(unsigned long seed);
void blockMT(void * mem, size_t len);
void blockMTxor(void * mem, size_t len);
unsigned long randomMT(void);

#define BLKGETSIZE _IO(0x12,96)
#define SWAPVERSION = 1;
#define MAX_BADPAGES    ((pagesize-1024-128*sizeof(int)-10)/sizeof(int)) 
#define _PATH_FSTAB     "/etc/fstab"
#define PROC_SWAPS      "/proc/swaps" 

static char *version = "0.01 alpha";
static char *rcsid = "$Id: swapoff.c,v 1.2 2000/04/11 08:54:24 drt Exp $";
static char *superparanoidmode = "MRTj0m1m2m3m4m5m6m7m8m9mambmcmdmemfu";
static char *paranoidmode = "Mumtmrm3m9mcm";

// our global configuration
int verbose = 0;
int quiet = 0;
int noO_SYNC = 0;
int nosync = 0; 
int all = 0;
int debug = 3;
off_t reseed = 1024;
char *overwritemode = "mmmmm";
int shutdowndevice = -1;

/* The definition of the union swap_header uses the constant PAGE_SIZE.
 * Unfortunately, on some architectures this depends on the hardware model,
 * and can only be found at run time -- we use getpagesize().
 */

static int pagesize;
static int *signature_page;
long pages;

struct swap_header_v1 {
  char         bootbits[1024];    /* Space for disklabel etc. */
  unsigned int version;
  unsigned int last_page;
  unsigned int nr_badpages;
  unsigned int padding[125];
  unsigned int badpages[1];
} *p;

#define MAKE_VERSION(p,q,r)	(65536*(p) + 256*(q) + (r))

/* The maximum number of pages in a swapfile. 
 * See mkswap.c for an explanation 
 */
#if defined(__alpha__)
#define V1_MAX_PAGES         ((1 << 24) - 1)
#elif defined(__mips__)
#define V1_MAX_PAGES         ((1 << 17) - 1)
#elif defined(__sparc_v9__)
#define V1_MAX_PAGES         ((3 << 29) - 1)
#elif defined(__sparc__)
#define V1_MAX_PAGES         (pagesize == 8192 ? ((3<<29)-1) : ((1<<18)-1))
#else
#define V1_MAX_PAGES         ((0x7fffffff / pagesize) - 1) 
#endif

/* show usage instructions */
void usage(void)
{
 	  fprintf(stderr, "   swapoff (paranoia edition) [-h] [-v] [-V] [-q]");
	  fprintf(stderr, "[-r <blocks>] [-l] [-p]\n");
	  fprintf(stderr, "       [-P] [-m <methods>] [-s] [-S] [-a | directory/mountpoint ...]\n");      
	  fprintf(stderr, "       -h     show help\n");
	  fprintf(stderr, "       -v     verbose mode\n");
	  fprintf(stderr, "       -V     print version.\n");
	  fprintf(stderr, "       -q     quiet - do not brabble\n");
	  fprintf(stderr, "       -r     reseed our internal  entropy  pool  every  <blocks>\n");
	  fprintf(stderr, "              blocks\n");
	  fprintf(stderr, "       -l     total low security mode. The same as -m m\n");
	  fprintf(stderr, "       -p     paranoid mode - you should use this. The same as -m\n");
	  fprintf(stderr, "              %s\n", paranoidmode);
	  fprintf(stderr, "       -P     extra   paranoid   mode.    The    same    as    -m\n");
	  fprintf(stderr, "              %s\n", superparanoidmode);
	  fprintf(stderr, "       -m     use the methods given in <methods> to overwrite the\n");
	  fprintf(stderr, "              partitions. See below for valid Methods\n");
	  fprintf(stderr, "       -s     do not open with O_SYNC\n");
	  fprintf(stderr, "       -S     do not call sync() all the time\n");
	  fprintf(stderr, "       -a     unmount and overwrite all swapdevices\n\n");
	  fprintf(stderr, "Methods for overwriting\n");
	  fprintf(stderr, "       0      write 0x00, `1'  write 0x11, `f' write 0xff,\n"); 
	  fprintf(stderr, "              guess what 2-9 and a-e do ...\n");
	  fprintf(stderr, "       r      write data from  /dev/random.  beware,  /dev/random\n");
	  fprintf(stderr, "              will  block  if  you don't have some gadget feeding\n");
	  fprintf(stderr, "              your entropy pool with something so this  will  get\n");
	  fprintf(stderr, "              very slow\n");
	  fprintf(stderr, "       u      write data from /dev/urandom\n");
	  fprintf(stderr, "       j      write data generated with rijndael\n");
	  fprintf(stderr, "       t      write data generated with tiger\n");
	  fprintf(stderr, "       m      write data generated with the Mersenne Twister\n");
	  fprintf(stderr, "       Y      encrypt  disk  contents  with rijandel and a random\n");
	  fprintf(stderr, "              key\n");
	  fprintf(stderr, "       R, U, J, T, M\n");
	  fprintf(stderr, "              the same as r, u, j, t, m but xor the  date  with  the\n");
	  fprintf(stderr, "              old data on the disk\n");
	  
}

/* get linux kernel version */
int linux_version_code(void) 
{
  struct utsname my_utsname;
  int p, q, r;
  
  if (uname(&my_utsname) == 0) 
    {
      p = atoi(strtok(my_utsname.release, "."));
      q = atoi(strtok(NULL, "."));
      r = atoi(strtok(NULL, "."));
      return MAKE_VERSION(p,q,r);
    }
  return 0;
}

/* init the datastructures for the first block of the swapfile */
static void init_signature_page() 
{
  pagesize = getpagesize();
  
#ifdef PAGE_SIZE
  if (pagesize != PAGE_SIZE)
    fprintf(stderr, "Assuming pages of size %d\n", pagesize);
#endif
  signature_page = (int *) malloc(pagesize);
  memset(signature_page,0,pagesize);
  p = (struct swap_header_v1 *) signature_page;
}

/* copy singature in our first page */
static void write_signature(char *sig) 
{
  char *sp = (char *) signature_page;
  
  strncpy(sp+pagesize-10, sig, 10);
}


static long valid_offset (int fd, int offset)
{
  char ch;
  
  if (lseek (fd, offset, 0) < 0)
    return 0;
  if (read (fd, &ch, 1) < 1)
    return 0;
  return 1;
}

static int find_size (int fd)
{
  unsigned int high, low;
  
  low = 0;
  for (high = 1; high > 0 && valid_offset (fd, high); high *= 2)
    low = high;
  while (low < high - 1)
    {
      const int mid = (low + high) / 2;
      
      if (valid_offset (fd, mid))
	low = mid;
      else
	high = mid;
    }
  return (low + 1);
}

/* return size in pages, to avoid integer overflow */
static long get_size(const char  *file)
{
  int	fd;
  long	size;
  
  fd = open(file, O_RDONLY);
  if (fd < 0) 
    {
      perror(file);
      exit(1);
    }
  if (ioctl(fd, BLKGETSIZE, &size) >= 0) 
    {
      int sectors_per_page = pagesize/512;
      size /= sectors_per_page;
    } 
  else 
    {
      size = find_size(fd) / pagesize;
    }
  close(fd);
  return size;
}


/* xor len1 bytes of buf1 with len2 bytes of buf2
 * the result is saved in buf1
 *
 * - len must be a multiple of 4 
 * - if len1 and len2 aren't equal the shorter buffer is warped
 *   arround until it is long enough.
 */
void XorBuf( unsigned char *buf1, size_t len1, unsigned char *buf2, size_t len2)
{
  size_t end, count = 0;
  unsigned long *p1, *p2;

  assert((len1 % 4 == 0) && (len2 % 4 == 0));

  p1 = (unsigned long*)buf1;
  p2 = (unsigned long*)buf2;

  if(len1 > len2) 
    {
      end = len1;
    }
  else
    {
      end = len2;
    }

  do {
    *p1 ^= *p2;
    
    p1++;
    if((unsigned char*)p1 - buf1 > len1)
      {
	p1 =  (unsigned long*)buf1;
      }
    
    p2++;
    if((unsigned char*)p1 - buf1 > len1)
      {
	p2 =  (unsigned long*)buf2;
      }

    count+=4;
  } while ( count < end );
}


/* Fill buf with len bytes of random Data 
 * 
 * len must be a multiple of 4
 */

void getRandomData(unsigned char *buf, size_t len) 
{
  int fd;
  long i, bytesread;
  unsigned char *buf2;
   
  /* this ist the array of files where we try to get some entropy */
  /* suggestions for other places to look for entropy are highly welcome */
  static char *randomSources[]= \
  { "/dev/random",              /* blocking random device on some Unices*/
    "/etc/ssh/ssh_random_seed", /* random seed for sshd                 */
    "/var/run/random-seed",     /* Kernel random seed on RedHat Linux   */
    "/dev/vbi",                 /* v4l non-video data (Videotext)       */
                                /* very nice source for some entropy,   */
                                /* since it carries more or less the    */
                                /* some raw on-the-air data             */
    "/dev/audio",               /* audio input                          */
    "/dev/sndstat",             /* which audio modules are loaded       */
    "/dev/isdninfo",            /* isdn subsystem                       */
    "/dev/isdnctrl",
    "/proc/interrupts",         /* try to gather as much entropy from   */
    "/proc/stat",               /* the linux procfs as we might get     */
    "/proc/swaps",
    "/proc/uptime",
    "/proc/loadavg",
    "/proc/locks",
    "/proc/meminfo",
    "/proc/net/dev",
    "/proc/net/arp",
    "/proc/net/sockstat",
    "/proc/net/unix",
    "/proc/net/tcp",
    "/proc/rtc",
    "/proc/scsi/scsi",
    "/proc/slabinfo",
    "/proc/stat",
    "/proc/swaps",
    "/proc/uptime",
    "/proc/version",
    "/dev/urandom",             /* nonblocking random device on some    */
                                /* Unices. We read this at last becaouse*/ 
                                /* the entropy pool of the device should*/ 
                                /* have filled a bit while we were      */
                                /* reading the other devices            */
    NULL };

  assert(len % 4 == 0);

  /* seed some entropy into the MT */
  seedMT((long long) getpid () *
	 (long long) time(0) *
	 (long long) getppid() * 
	 (long long) clock());
  
  /* try to seed some entropy into the c library 
   * we use multiply and modulo instead of addition or xor 
   * to get a better spreading of our entropy in the 
   * (unsigned int) we feed to sramdom(3)
   *
   * We don't use random() ourselve, but we can't know where the C
   * library uses it, so we feed some entropy in it, just to be sure
   */
  srandom((unsigned int)(((long long) getpid () *
			  (long long) time(0) *
			  (long long) getppid() * 
			  (long long) clock() *
			  (long long) randomMT()) % 0xffff)); 


  /* first we xor buf with MT random data - this ensures
   * that we get at least some entropy if the succeeding
   * tries to gather entropy fail
   */
  blockMTxor(buf, len);
 
  /* try to allocate a second buffer */
  buf2 = malloc(len);
  if (!buf2)
    {
      /* Since we filled buf already with some MT randomness we
	 continue, wven if we can't read from disk because of 
	 memory shortness.
      */
      perror("Out of memory");
    }
  else
    {
      /* we've got our memory, so try to read entropy wherever 
       * we might find some
       */
      
      for(i = 0; randomSources[i] != NULL; i++)
	{
	  if(debug > 4)
	    fprintf(stderr, "reading entropy from %s ... ", randomSources[i]);
	  
	  fd = open(randomSources[i], O_RDONLY|O_NONBLOCK);          
	  
	  if (fd < 0)
	    {
	      if(debug > 4)
		{
		  /* We don't really care, because we do try and error to 
		   * get some entropy
		   */
		  perror("couldn't open");
		}
	    }           
	  else
	    {
	      bytesread = read(fd, buf2, len);
	      
	      if(debug > 4)
		fprintf(stderr, "%ld bytes\n", bytesread);
	      
	      close(fd);
	      
	      if(bytesread > 4)
		{
		  /* we have to change bytesread to a multiple of 4 */
		  bytesread = bytesread & 0xfffffffc;
		  
		  XorBuf(buf, len, buf2, bytesread);
		}
	    }
	}
      free(buf2);
    }
}

/* Write random data to the device */

int write_random(int device, unsigned long total_pages, char mode)
{
  unsigned long long int res[3]; 
  unsigned int current_page;
  unsigned long int c;
  char *buffer, *buffer2;
  int xor = 0;
  int fd = -1;
  int byte = -1;
  int status = 1;
  char IV[16];
  word8 rKeySched[MAXROUNDS+1][4][4];

  buffer2 = NULL;

  /* go to the beginning of the device */
  if (lseek(device,0,SEEK_SET) != 0)
    {
      perror("seek failed in write_random");
      status++;
    }
    
  /* try to malloc a buffer */
  buffer = malloc(pagesize);

  /* if there isn't enoug memory, try a buffer half as big */
  if(buffer)
    {
      /* there is a buffer so we can proceed */
      
      current_page = 0; 
      
      /* fill buffer with random data to initialize our entropypool */
      getRandomData(buffer, pagesize);

      /* Initialisation depending on the method */
      switch(mode)
	{
	case '0': byte = 0x0; break;
	case '1': byte = 0x11; break;
	case '2': byte = 0x22; break;
	case '3': byte = 0x33; break;
	case '4': byte = 0x44; break;
	case '5': byte = 0x55; break;
	case '6': byte = 0x66; break;
	case '7': byte = 0x77; break;
	case '8': byte = 0x88; break;
	case '9': byte = 0x99; break;
	case 'a': byte = 0xaa; break;
	case 'b': byte = 0xbb; break;
	case 'c': byte = 0xcc; break;
	case 'd': byte = 0xdd; break;
	case 'e': byte = 0xee; break;
	case 'f': byte = 0xff; break;

	case 'A': xor=1; byte = 0xaa; break;
	case 'B': xor=1; byte = 0xbb; break;
	case 'C': xor=1; byte = 0xcc; break;
	case 'D': xor=1; byte = 0xdd; break;
	case 'E': xor=1; byte = 0xee; break;
	case 'F': xor=1; byte = 0xff; break;

	case 'R':
	  /* get entropy from /dev/random */
	  xor=1;
	case 'r':
	  fd = open("/dev/random", O_RDONLY);
	  if (fd < 0) 
	    {
	      perror("/dev/random");
	      if(debug)
		{
		  fprintf(stderr, "fallback to Mersenne Twister\n");
		}		  
	      if(mode == 'R')
		{
		  mode = 'M';
		  xor = 1;
		}
	      else
		{
		  mode = 'm';
		}
	    }	
	  break;
	case 'U':
	  /* get entropy from /dev/random */
	  xor=1;
	case 'u':
	  fd = open("/dev/urandom", O_RDONLY);
	  if (fd < 0) 
	    {
	      perror("/dev/urandom");
	      if(debug)
		{
		  fprintf(stderr, "fallback to Mersenne Twister\n");
		}		  
	      if(mode == 'R')
		{
		  mode = 'M';
		  xor = 1;
		}
	      else
		{
		  mode = 'm';
		}
	    }	     

	  break;
 	case 'J':
	  xor = 1;
	case 'j':
	case 'Y':
	  /* using rijandael */

	  /* we generating our random/keystream by encrypting a random buffer 
	     with a random key */

	  /* get a random "IV" */
	  blockMT(IV, 16);

	  /* initialize  rijndael with this key */
	  rijndaelKeySched((unsigned char*) res, 196, rKeySched);

	  break;
	case 'T':
	  /* use the tiger hash */
	  xor=1;
	case 't'
	  /* nothing to inizialize */:
	  break;
	case 'M':
	  /* use the MT PRNG */
	  xor = 1;
	case 'm':
	  /* get a 196 Bit hash of buffer */
	  tiger(buffer, pagesize, res); 
	  /* seed randomgenerator */
	  seedMT(((long long) res[0] *
		 (long long) res[1] *
		 (long long) res[2]) % 0xffffffff);

	  break;
	default:
	      fprintf(stderr, " unknown mode %c, defaulting to the Mersenne Twister\n", (int) mode);
	      mode = 0;
	}

      if(byte >= 0)
	{
	  memset(buffer, byte, pagesize);
	}
      
      if(xor)
	{
	  /* the user wantes to xor everything with the disk contents
	     so we need an extra buffer
	     try to malloc a buffer */ 
	  if((buffer2 = malloc(pagesize))== NULL)
	    {
	      /* We didn't get the memory, so we can't do the xor stuff */
	      xor = 0;
	      fprintf(stderr, "can't allocate xor buffer!\n");
	      status++;
	    }
	}

      /* now we are iterating over the partition */
      while (current_page <  total_pages) 
	{
	  if(verbose)
	    {
	      printf(" %07u\b\b\b\b\b\b\b\b", current_page);
	      //	      fflush(stdout);
	    }

	  /* act depending on the mode */
	  switch(mode)
	    {
	    case '0': 
	    case '1': 
	    case '2': 
	    case '3': 
	    case '4': 
	    case '5': 
	    case '6': 
	    case '7': 
	    case '8': 
	    case '9': 
	    case 'a': 
	    case 'b': 
	    case 'c': 
	    case 'd': 
	    case 'e': 
	    case 'f': 
	    case 'A': 
	    case 'B': 
	    case 'C': 
	    case 'D': 
	    case 'E': 
	    case 'F': 
	      /* we are writing fixed data, so nothing do do here */
	      break;
	    case 'r':
	    case 'R':
	    case 'u':
	    case 'U':
	      /* we are asked to encrypt original disk contents using rijandael */
	      /* we are reading random data from fd */
	      if ((pagesize) != read(fd, buffer, pagesize)) 
		{
		  perror("can't read");
		  status++;
		}
	      break;
	    case 'Y':
	      /* read original disk content in buffer */
	      if ((pagesize) != read(device, buffer, pagesize)) 
		{
		  perror("can't read");
		  status++;
		}
	      /* rewind to beginning of block */ 
	      if (lseek(device, current_page*pagesize, SEEK_SET) != current_page*pagesize)
		{
		  perror("a real problem ->\a seek failed in write_random");
		  status++;
		}
	    case 'J':
	    case 'j':
	      /* using rijandel */
	      /* randomize buffer by encrypting it - work it in 128 bit/16 byte steps */
	      for( c = 0; c < pagesize; c += 16)
		{
		  /* xor plaintext with IV(=chiphertext from the last block) */
		  XorBuf(buffer+c, 16, IV, 16);
		  /* encrypt plaintext and store it in IV */
		  rijndaelEncrypt (buffer+c, IV, rKeySched);
		}    
	      break;
	    case 'T':
	    case 't':
	      /* get a 196 Bit hash of buffer */
	      /* randomize buffer by hashing it with itself 
		 - work it in 196 bit/24 byte steps */
	      for( c = 0; c < pagesize; c += 24)
		{
		  /* hash the buffer */
		  tiger(buffer, pagesize, (unsigned long long int *)(buffer+c));
		  
		  /* unsigned long long int res[3]; */

		}
	      break;
	    case 'M':
	    case 'm':
	      blockMTxor(buffer, pagesize);
	      break;
	    default:
	      //	      fprintf(stderr, "unknown mode %c, defaulting to the Mersenne Twister\n", (int) mode);
	      mode = 0;
	    }

	  if(xor)
	    {
	      /* Some ueber-paranoid user asked us to xor new with old Data */
	      
	      if ((pagesize) != read(device, buffer2, pagesize)) 
		{
		  perror("can't read");
		}
	      /* rewind to beginning of block */ 
	      if (lseek(device, current_page*pagesize, SEEK_SET) != current_page*pagesize)
		{
		  perror("a real problem ->\a seek failed in write_random");
		  status++;
		}

	      XorBuf(buffer, pagesize, buffer2, pagesize);
	    }

	  /* write data to disk */
	  if ((pagesize) != write(device, buffer, pagesize)) 
	    {
	      current_page++;
	      perror("can't write\a  ");
	      status++;
	    }
	  current_page++;
	  
	  /* make sure modifications are written to disk */
	  if(!nosync)
	    {
	      if (fsync(device))
		{
		  perror("sync failed");
		  status++;
		}
	    }
	}
      
      if(verbose)
	{
	  printf("        \b\b\b\b\b\b\b\b");
	}
            
      /* free our buffer */
      free(buffer);
      if(xor)
	{
	  free(buffer2);
	}

      /* free fd */
      if(fd == -1)
	{
	  close(fd);
	}
    }
  else
    {
      /* we are low on memory, try to write anything on the device */
      for(current_page = 0; current_page < total_pages; current_page++)
	{
	  c = randomMT();
	  if(write(device, (char *) &c, 4) != 4)
	    {
	      perror("can't write even 4 bytes!");
	      status++;
	    }
	}
    }
  return status;
}

int mkswap(int device)
{  
  int goodpages, status = 0;
  int offset; 

  /* create new swapspace */
  p->version = 1;
  p->last_page = pages-1;
  p->nr_badpages = 0;
  
  goodpages = pages - 1;
  
  printf(" size = %ld bytes",
	 (long)(goodpages*pagesize));
  
  write_signature("SWAPSPACE2");
  
  offset = (1024);
  if (lseek(device, offset, SEEK_SET) != offset)
    {
      perror("unable to rewind swap-device");
      status++;
    }
  else
    {
      if (write(device,(char*)signature_page+offset, pagesize-offset)
	  != pagesize-offset)
	{
	  perror("unable to write signature page");
	  status++;
	}
    }
  return status;
}

int handle_disk(char *name)
{
  struct stat statbuf;
  int device = -1;
  int status = 0;
  char *stage;
  
  if(!quiet)
    {
      printf("%s ", name);
      fflush(stdout);
    }
  
  /* we have to unmount first */
  if(!quiet)
    {
      printf("unmounting ");
      fflush(stdout);
    }

  if((status = swapoff(name)) != 0)
    {
      perror("can' turn off swapspace");

      /* hmmm, this patition wasn't mounted as swapspace
	 so we should check if it is a valid swappartition 
	 to keep the user from killing regular partitions */ 
    }  

  pages = get_size(name);  
  
  if ((pages < 10) && !quiet) 
    {
      fprintf(stderr, "warning: swap area needs to be at least %ldkB, device is %ldkB\n",
	      (long)(10 * pagesize / 1024), (long)(pages * pagesize / 1024));
      status++;
    }
  
  if (pages > V1_MAX_PAGES) 
    {
      pages = V1_MAX_PAGES;
      fprintf(stderr, "warning: truncating swap area to %ldkB\n",
	      pages * pagesize / 1024);
      status++;
    }   
  
  if(noO_SYNC)
    {
      device = open(name,O_RDWR);
    }
  else
    {
      device = open(name,O_RDWR| O_SYNC);
    }
  
  if (device < 0 || fstat(device, &statbuf) < 0) 
    {
      perror(name);
      status++;
    }
  else
    {
      shutdowndevice = device;

      if(!quiet)
	{
	  printf("\b cleaning ");
	  fflush(stdout);
	}
      
      for(stage = overwritemode; *stage; stage++)
	{
	  if(verbose)
	    {
	      printf("%c", *stage);
	      fflush(stdout);
	    }
	  /* write random data to the disk */
	  status |= write_random(device, pages, *stage);
	}
      
      if(!quiet)
	{
	  printf(", mkfswap ");
	  fflush(stdout);
	}
      
      status |= mkswap(device);
      
      /*
       * A subsequent swapon() will fail if the signature
       * is not actually on disk. (This is a kernel bug.)
       */
      
      if (fsync(device))
	{
	  perror("fsync failed");
	  status++;
	}

      close(device);
      shutdowndevice = -1;
      
      if(!quiet)
	{
	  printf("\n");
	  fflush(stdout);
	}
    }

  return status;
}


void cleanup() 
{
  int status = 1; 
  fprintf(stderr,"\nTerminated by signal. Clean exit.\n");
  if(shutdowndevice != -1)
    {
      /* we need to setup swapspace zu keep the partition intact
	 before exiting */
      
      status |= mkswap(shutdowndevice);      
      fsync(shutdowndevice);
      sync();
      close(shutdowndevice);
    }
  fflush(stdout);
  fflush(stderr);
  sync();
  exit(status);
}


int main(int argc, char ** argv)
{
  int c, status = 0;
  struct mntent *fstab; 
  FILE *fp;

  /* Parse Commandline */
  while ((c = getopt(argc, argv, "hVvqr:lpPm:sSad")) !=EOF) 
    {
      switch (c)
	{        
	case 'V':
	  fprintf(stderr, "swapoff (paranoia edition) %s\n  %s\n", version, rcsid);
	  fprintf(stderr, "  Doobee R. Tzeck <drt@ailis.de>\n");
	  fprintf(stderr, "  based on mkswap(8) 1999-02-22 /swapoff(8) 0.99 from util-linux-2.9w\n");
	  exit(0);
	  break;
	case 'h': ;
	  usage();
	  exit(0);
	  break;
	case 'v':
	  verbose++;
	  break;
	case 'd':
	  debug++;
	  break;
	case 'q':
	  quiet++;
	  break;
	case 'r':
	  reseed = strtol(optarg, NULL, 10);
	case 'l':
	  overwritemode = "m";
	  break;
	case 'p':
	  overwritemode = paranoidmode;
	  break;
	case 'P':
	  overwritemode = superparanoidmode;
	  break;
	case 'm':
	  overwritemode = optarg;
	  break;
	case 's':
	  noO_SYNC = 1;
	  break;
	case 'S':
	  nosync = 1;
	case 'a':
	  all = 1;
	  break; 
	}
    }

  /* Check if we have e recent Kernel. 
     Since we write V1 Swap-Space, we need a kernel > 2.2
  */
  if (linux_version_code() < MAKE_VERSION(2,2,1))
    {
      fprintf(stderr, "i was born to work with a kernel > 2.2.0");
      exit(1);
    }

  init_signature_page();	/* get pagesize */
  
  if(((argc - optind) < 1) && all == 0)
    {
      fprintf(stderr, "no swapdevice selected\n");
      exit(1);
    }

  /* unbufferd stdout */
  setvbuf(stdout, NULL, _IONBF, 0);

  /* install signal handlers for clean shutdown */
  signal(SIGINT, cleanup);
  signal(SIGTERM, cleanup);
  signal(SIGHUP, cleanup);

  // iterate through the remaining arguments
  for(c = optind; c < argc; c++)
    {
      status |= handle_disk(argv[c]);
    }

  /* now do the rest if the user asked for -a(ll) */
  if(all)
    {
      fp = setmntent(_PATH_FSTAB, "r");   
      if (fp == NULL) 
	{
	int errsv = errno;
	fprintf(stderr, "cannot open %s: %s\n",
		_PATH_FSTAB, strerror(errsv));
	exit(2);
       }
      while ((fstab = getmntent(fp)) != NULL) 
	{
	  if (strcmp(fstab->mnt_type, MNTTYPE_SWAP) == 0) 
	    {
	      status |= handle_disk(fstab->mnt_fsname); 
            }  
	}                    
    }

  exit(status);
}
