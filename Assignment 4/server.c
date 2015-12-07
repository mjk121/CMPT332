#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SENDERPORT "31000"
#define RECEIVERPORT "32000"
#define BACKLOG 10

void sigchld_handler()
{
   int saved_errno;
   saved_errno = errno;
   while(waitpid(-1, NULL, WNOHANG) > 0);
   errno = saved_errno;   
}

void *get_in_addr(struct sockaddr *sa)
{
   if(sa->sa_family == AF_INET)
   {
      return &(((struct sockaddr_in*)sa)->sin_addr);
   }
   return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(/*int argc, char *argv[]*/void)
{
   int rv;  // return value
   // int sockfd, new_fd;
   struct addrinfo hints, *servinfo, *p;
   // struct sockaddr_storage their_addr;
   // socklen_t sin_size;
   
   int yes = 1;  // for setsockopt()
   int sender, receiver;  // sender and receiver socket descriptors
   // char s[INET6_ADDRSTRLEN];
     
   char hostname[128];  // name of the host
   char ipstr[INET6_ADDRSTRLEN];  // IP string
   void *addr;
   char *ipver;  // IPv4 or IPv6   

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
   hints.ai_socktype = SOCK_STREAM;  // TCP
   hints.ai_flags = AI_PASSIVE;

   if(gethostname(hostname, sizeof hostname) != 0)
   {
      perror("gethostname");
      exit(1);
   }
   
   if((rv = getaddrinfo(hostname, NULL, &hints, &servinfo)) != 0)
   {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
   }
   
   if(servinfo->ai_family == AF_INET)
   {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
   }
   else
   {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)servinfo->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
   }
   
   // convert IP to string and print
   inet_ntop(servinfo->ai_family, addr, ipstr, sizeof ipstr);
   printf("Server: %s   IP address:  %s - %s   Sender Port: %s   Receiver Port: %s\n", hostname, ipver, ipstr, SENDERPORT, RECEIVERPORT);
   
   freeaddrinfo(servinfo);
      

   // done with getting the server info. initialize sender now

   
   // get a socket and bind it
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   if ((rv = getaddrinfo(NULL, SENDERPORT, &hints, &servinfo)) != 0) 
   {
      fprintf(stderr, "SERVER: Error on getaddrinfo(): %s\n", gai_strerror(rv));
      exit(EXIT_FAILURE);
   }
   
   for(p = servinfo; p != NULL; p = p->ai_next)
   {
      sender = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if(sender < 0)
      {
         continue;
      }
	
      // lose the pesky "address already in use" error message
      setsockopt(sender, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      if (bind(sender, p->ai_addr, p->ai_addrlen) < 0) 
      {
         close(sender);
         continue;
      }
      
      break;
   }

   // if we got here, it means we didn't get bound
   if(p == NULL)
   {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
   }

   freeaddrinfo(servinfo);
   yes = 1;
   
    
   // done with sender. initialize receiver now


   // get a socket and bind it
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   if ((rv = getaddrinfo(NULL, RECEIVERPORT, &hints, &servinfo)) != 0)
   {
      fprintf(stderr, "SERVER: Error on getaddrinfo(): %s\n", gai_strerror(rv));
      exit(EXIT_FAILURE);
   }

   for(p = servinfo; p != NULL; p = p->ai_next)
   {
      receiver = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if(receiver < 0)
      {
         continue;
      }

      // lose the pesky "address already in use" error message
      setsockopt(receiver, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      if (bind(receiver, p->ai_addr, p->ai_addrlen) < 0)
      {
         close(receiver);
         continue;
      }

      break;
   }

   // if we got here, it means we didn't get bound
   if(p == NULL)
   {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
   }

   freeaddrinfo(servinfo);
   yes = 1;


   // done with recevier.  start main loop


   while(1)
   {
      /*
      sin_size = sizeof their_addr;
      new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
      if(new_fd == -1)
      {
         perror("accept");
         continue;
      }

      inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
      printf("server: got connection from %s\n", s);

      if(!fork())
      {
         close(sockfd);
         if(send(new_fd, "Hello, world!", 13, 0) == -1)
         {
            perror("send");
         }
         close(new_fd);
         exit(0);
      }		
      close(new_fd);
      */
   }
   
   return 0;
}
