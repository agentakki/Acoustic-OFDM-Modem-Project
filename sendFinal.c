#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/stat.h>

void convertToString(char s[], int n);
void capture(char filename[]);
void transfer(int writeport, char* filename);
int open_port(char str[]);

int main(int argc, char** argv)
{
    char* portName = argv[1];
    int counter = atoi(argv[2]);

    //writing
    int writeport = open_port(portName);

   //capture image
   int i = 0;
   char filename[14];
   sleep(2);
   while(tcflush(writeport, TCIOFLUSH) != 0);
   sleep(2);
   if(tcflush(writeport, TCIOFLUSH) == 0)
      printf("Buffer Flushed\n");

   for (i = 0; i < counter; i++)
   {
   	//create file name based on date time
   	/*time_t t = time(0);
   	struct tm  tstruct;
   	tstruct = *localtime(&t);
	strftime(filename, sizeof(filename), "%Y%m%d%H%M", &tstruct); */
	sleep(10);
	snprintf(filename, 14, "%d.jpg", i);
	capture(filename);
	transfer(writeport, filename);
   }
   //closing ports
   close(writeport);
}

void capture(char filename[])
{
  char command[256];
  char* str1 = "avconv -f video4linux2 -s 64x48 -i /dev/video0 -ss 0:0:2 -frames 1 ./";
  strcpy(command, str1);
  strcat(command, filename);
  //printf("%s \n", command);
  system(command);
}

void transfer(int writeport, char* filename)
{

	    //printf("%s \n", filename);
	    // open file; get file size
	    FILE *ifp = fopen(filename, "r");
	    char tmp[100];
	    struct stat st;
            stat(filename, &st);
            int filesize = st.st_size;
	    char size[6];
	    convertToString(size,filesize); 
	    //printf("%s \n", size);
	    int n = write(writeport, size , 6);
	    while(!read(writeport, tmp, 100));
	    sleep(3);
	    int w;
	    char image[600];
	    while(!feof(ifp))
	    {
		n = fread(image,1,600,ifp); 
		w = write(writeport, image, n);
		//printf("%d\n", w);
		while(!read(writeport, tmp, 100)); //transmission verify for us
		sleep(8);
	    }

	    fclose(ifp);
}


int open_port(char str[])
{
    int fd = open(str, O_RDWR | O_NONBLOCK); // ?? NDELAY or NONBLOCK?

  if (fd == -1)
  {
                    perror("open_port: Unable to open /dev/ttyS0 - ");
  }
  else
                    fcntl(fd, F_SETFL, 0);

      struct termios options;
      tcgetattr(fd, &options); //this gets the current options set for the port

      // setting the options

      cfsetispeed(&options, B9600); //input baudrate
      cfsetospeed(&options, B9600); // output baudrate
      options.c_cflag |= (CLOCAL | CREAD); // ?? enable receicer and set local mode
      //options.c_cflag &= ~CSIZE; /* mask the character size bits */
      options.c_cflag |= CS8;    /* select 8 data bits */
      options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // choosing raw input
      options.c_iflag &= ~INPCK; // disable parity check
      options.c_iflag &= ~(IXON | IXOFF | IXANY); // disable software flow control
      options.c_oflag |= OPOST; // ?? choosing processed output
      options.c_cc[VMIN] = 0; // Wait until x bytes read (blocks!)
      options.c_cc[VTIME] = 0; // Wait x * 0.1s for input (unblocks!)

      // settings for no parity bit
      options.c_cflag &= ~PARENB;
      options.c_cflag &= ~CSTOPB;
      options.c_cflag &= ~CSIZE;
      options.c_cflag |= CS8;

      tcsetattr(fd, TCSANOW, &options); //set the new options ... TCSANOW specifies all option changes to occur immediately

  return (fd);
}

void convertToString(char s[], int n)
{
   int i = 5;
   for(; i >= 0; i--)
   {
	s[i] = (n % 10) + '0';
	n /= 10;
   }
}
