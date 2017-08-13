#include"httpd.h"
static void usege(const char*p)
{
   printf("%s ip port\n");
}
void Read(int sock,int epfd)
{
     printf("sock:%d\n",sock);
     handler_request(sock);
     if(epoll_ctl(epfd,EPOLL_CTL_DEL,sock,NULL)<0)
     {
       perror("epoll_ctl");
       exit(12);
     }
     close(sock);
}

int main(int argv,const char*argc[])
{
   if(argv!=3)
   {
      usege(argc[0]);
      exit(5);
  }
   int listen_sock=startup(argc[1],atoi(argc[2]));
   int epfd=epoll_create(256);
   printf("epfd :%d\n",epfd);
  struct epoll_event ev;
  struct epoll_event env[128];
  ev.events=EPOLLIN|EPOLLET;
  ev.data.fd=listen_sock;
  if(epoll_ctl(epfd, EPOLL_CTL_ADD,listen_sock,&ev)<0)
   {
           perror("epoll_ctl");
           exit(5);
   }
   while(1)
   {
     int timeout = -1;
     int reve_n = epoll_wait(epfd,env,sizeof(env)/sizeof(env[0]),timeout);
      printf("reve_n :%d\n",reve_n);
      switch (reve_n)
     {
             case 0:
                     printf("time out \n");
                     break;
            case -1:
                     perror("epoll_wait");
                     exit(6);
             default:
         {   int idx=0;
             for(idx;idx<reve_n;idx++)
                {
                 int fd=env[idx].data.fd;
                 printf("fd:%d,listen:%d",fd,listen_sock);
                 if(fd == listen_sock&&env[idx].events&EPOLLIN)
                 {
                   printf("okk..\n");
                   struct sockaddr_in cilent ;
                   socklen_t len=sizeof(cilent);
                   int sock=0;
                 while((sock=accept(fd,(struct sockaddr*)&cilent,&len))>0) // while ET
                     {
                           set_noblock(sock);
                           ev.events=EPOLLIN|EPOLLET;
                           ev.data.fd=sock;
                           if(epoll_ctl(epfd, EPOLL_CTL_ADD,sock,&ev)<0)
                           {
                                 perror("epoll_ctl");
                                   exit(7);
                           }
                           const char * cilent_ip=inet_ntoa(cilent.sin_addr);
                           int   cilent_port=ntohs(cilent.sin_port);
                           printf("cilent connect ip:%s port:%u \n",cilent_ip,cilent_port);
                     }
                     if(sock<0&&errno!=EWOULDBLOCK) {
                             perror("accept");
                     }
                     continue;
                }
                else if(env[idx].events&EPOLLIN)
                {
                        printf("2222\n");
                        Read(fd,epfd);
                }else{
                  printf("bug\n");
                  exit(1);
                }
             }
        }
        break;
     }// end switch
   }
   return 0;
}

