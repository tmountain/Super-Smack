#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

using namespace std;

#include <fstream>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netinet/in.h>
#include        <arpa/inet.h>
#include 	<netdb.h>


class TcpClient
 {
  protected:
   int socket_fd;
   struct sockaddr_in  serv_addr;
   int status;
   FILE *log;

   void print_time(FILE* fp)
    {
     time_t now;
     time(&now);
     char* time_str = ctime(&now);
     time_str[strlen(time_str) - 1] = 0;
     fprintf(fp, "[%s] ", time_str);
    }
   void init(struct in_addr* other_serv_addr, int port);
   void log_init(FILE* init_log) 
    {
     status = 0;
     log = init_log ;
     if(log) 
       setbuf(log, NULL);
    }

  public:
   TcpClient() { status = 0; log = NULL; }
   TcpClient(char* host, int port, FILE* init_log = stderr);
   TcpClient(struct in_addr* other_serv_addr, int port, FILE* init_log = stderr);

   virtual ~TcpClient();
   void connect(int sec_timeout = 0);
   int send(char* data, int num_bytes);
   int receive(char* data, int num_bytes, int sec_timeout = 0, int usec_timeout = 0);

   operator void*() { return (void*)status;}
   int operator !() { return !status;}
   void set_log(FILE* new_log) { log = new_log;}

   virtual void error(char* fmt, ...)
    {
     va_list ap;
     va_start( ap, fmt);
     status = 0;
     if(!log) return;
     print_time(log);
     fprintf(log, "TCP Client error:");
     vfprintf(log, fmt, ap);
     fprintf(log, ": errno = '%s'\n", strerror(errno));
    }
   virtual void warn(char* fmt, ...)
    {
     va_list ap;
     va_start( ap, fmt);
     if(!log) return;
     print_time(log);
     fprintf(log, "TCP Client warning:");
     vfprintf(log, fmt, ap);
     fprintf(log, ": errno = '%s'\n", strerror(errno));
    }
 };

#endif
