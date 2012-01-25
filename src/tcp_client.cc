#include "config.h"
#include "tcp_client.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>

sigjmp_buf jmp_env;

static void connect_alarm(int)
 {
  siglongjmp(jmp_env, 1);
 }

void TcpClient::init(struct in_addr* other_serv_addr, int port)
 {
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  
  serv_addr.sin_family            = AF_INET;
  memcpy(&(serv_addr.sin_addr.s_addr), other_serv_addr, sizeof(serv_addr.sin_addr.s_addr));
  serv_addr.sin_port              = htons(port);

  socket_fd = -1;
        
  if ( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    { 
      error("TcpClient: can't open stream socket");
      return;
    }

  status = 1;
 }

TcpClient::TcpClient(struct in_addr* other_serv_addr, int port,
  FILE* init_log)
  {
   log_init(init_log);
   init(other_serv_addr, port);
  }

TcpClient::TcpClient(char* host, int port, FILE* init_log)
 {
  log_init(init_log);
  struct hostent *h = gethostbyname(host);

  if(!h)
   {
    error("Host name lookup failure for %s", host);
    return;
   }
   init((struct in_addr*)h->h_addr, port);
 }

void TcpClient::connect(int sec_timeout)
 {
  if(sec_timeout)
    {
      signal(SIGALRM, connect_alarm);
      alarm(sec_timeout);
      if(sigsetjmp(jmp_env, 1))
       {
         error("TcpClient: connect() timed out");
	 goto done;
       }
    }

  if (::connect(socket_fd, (struct sockaddr *) &serv_addr, 
        sizeof(serv_addr)) < 0)
    error("TcpClient: can't connect to server : %s", strerror(errno ));

done:
 
  if(sec_timeout)
    alarm(0);
 }

int TcpClient::send(char* data, int num_bytes)
 {
  return write(socket_fd, data, num_bytes);
 }

int TcpClient::receive(char* data, int num_bytes, int sec_timeout,
		       int usec_timeout)
 {
   fd_set rfds;
   struct timeval tv;
   int ret_select;

   FD_ZERO(&rfds);
   FD_SET(socket_fd, &rfds);
  
   tv.tv_sec = sec_timeout;
   tv.tv_usec = usec_timeout;


   ret_select = select(socket_fd + 1, &rfds, NULL, NULL, 
     (sec_timeout || usec_timeout) ? &tv : (struct timeval*)NULL);
   
   if(ret_select == -1)
     warn("Select error: %s\n", strerror(errno));

   if(ret_select < 1)
     return 0;

   return read(socket_fd, data, num_bytes);
 }

TcpClient::~TcpClient()
 {
  if(socket_fd >= 0)
   close(socket_fd);
 }




