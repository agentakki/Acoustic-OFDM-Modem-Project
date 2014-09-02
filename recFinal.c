#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <stdlib.h>
void receive(int readport, char* filename);
int open_port(char str[]);
int convert(char *str);
void display(char filename[]);

int main(int argc, char** argv)
{
   char* portName = argv[1];
   int readport = open_port(portName);
   //capture image
   int i = 0;
   int cnt = atoi(argv[2]);
   char filename[14];
   // clear buffer
   sleep(2);
   while (tcflush(readport, TCIOFLUSH) != 0);
   sleep(2);
   if (tcflush(readport, TCIOFLUSH) == 0)
	printf("Buffer flushed\n");
   for (;i<cnt;i++)
   {
	snprintf(filename, 14, "%d.jpg", i);
	receive(readport, filename);
	sleep(5);
	display(filename);
   }
   //closing ports
   close(readport);
}

void display(char filename[])
{
	char exec[256];
	char* cmd = "display ";
	strcpy(exec, cmd);
	strcat(exec, filename);
	strcat(exec, " &");
	system(exec);
}

void receive(int readport, char* filename)
{
 
   //trying to read one character at a time
    char buff;
    int size = 0;
    int n = 0;
    char c;
    char t[6];
    FILE *ofp = fopen(filename, "w");

    while(size < 6)
    {
        while(!read(readport, &c, 1));
        t[size] = c;
        size++;
    }
    n = convert(t);
    printf("%s\n",t);
//    printf("%d", convert(t));

    while(n > 0){
     while(!read(readport, &c, 1));
//     printf("%c", c);
     fwrite(&c,1,1,ofp);
     n--;
    }

    fclose(ofp);

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

int convert(char *str)
{
   int tmp = 0;
   int i;
   for(i = 0; i < 6; i++)
   {
	tmp = tmp*10 + (str[i]-'0');
   }
   return tmp;
}

