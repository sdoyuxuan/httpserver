#include"httpd.h"
static void usege(const char*p)
{
   printf("%s ip port\n");
}
int main(int argv,const char*argc[])
{
   if(argv!=3)
   {
      usege(argc[0]);
      exit(5);
   }
   int listen=startup(argc[1],atoi(argc[2]));
   struct sockaddr_in client;
   socklen_t len=sizeof(client);
   pthread_t tid;
   while(1)
  {
    int re_sock=accept(listen,(struct sockaddr*)&client,&len);
   printf("get client ip:%s id:%d\n",inet_ntoa(client.sin_addr),client.sin_port);
   pthread_create(&tid,NULL,handler_request,(void*)re_sock);
   pthread_detach(tid);
  }
    return 0;
}

