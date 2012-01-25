#include "tcp_client.h"

int main(int c, char** argv)
 {
  char* host = "www.mysql.com";
  int port = 80;
  int sec_timeout = 2;
  char* log_file = NULL;
  FILE* log;


  if(c > 5)  c = 5;

  switch(c)
   {
    case 5: sec_timeout = 2;
    case 4: log_file = argv[3];
    case 3: port = atoi(argv[2]);
    case 2: host = argv[1];
   }

   if(log_file)
    {
     log =  fopen(log_file, "a");
     if(!log)
      {
       fprintf(stderr, "Could not open log file %s: %s\n", log_file,
        strerror(errno));
       exit(1);
      }
    }
    else
     log = stderr;

    char buffer[2048];
    TcpClient client(host,port, log);
    TcpClient client1(host,port, log);
    if(!client)
     exit(1);
    if(!client1)
     exit(1);

    client.connect(sec_timeout);
    client1.connect(sec_timeout);
   
    if(!client)
     {
      exit(1);
     }
   
   
    if(!client1)
     {
      exit(1);
     }

    client.send("GET / HTTP/1.0\n\n", strlen("GET / HTTP/1.0\n\n"));
    int bytes_read;
    bytes_read =  client.receive(buffer, 2048);

    do
     {
       cout.write(buffer, bytes_read);  
       bytes_read =  client.receive(buffer, 2048);
     } while(bytes_read > 0);

   
 }


