#include "httpd.h"
void  echo_err(int sock)
{
      const char* request_head="HTTP/1.1 404 notfound\r\n";
      const char*blank="\r\n";
      char p[]="not found 404";
      send(sock,request_head,strlen(request_head),0);
      send(sock,blank,strlen(blank),0);
      send(sock,p,strlen(p),0);
}
int get_line(int sock,char * buff, int size)
{
    int pos=0;
    buff[0]=0;
    char test[1]={0};
    int len=size;
    while(pos<len-1)
    {
      if(recv(sock,test,1,MSG_PEEK)<0)
       {
        perror("recv");
        return -1;
       }
       if(test[0]!='\r'){
        recv(sock,buff+pos,1,0);
       }
       else //if(recv(sock,test,1,MSG_PEEK)&&test[0]=='\n')
       {
          recv(sock,buff+pos,1,0);
          if(recv(sock,test,1,MSG_PEEK)&&test[0]=='\n')
           {
             recv(sock,buff+pos,1,0); // /r/n  quan du shang lai //  pos--> \r   ------->  pos--> \n
          }
          else{
            recv(sock,buff+pos,1,0);
            buff[pos]='\n';
          }
      }
       if(buff[pos]=='\n') break;
       ++pos;
    }
    buff[pos+1]=0; // \n ---> \0
    printf("buff:%s\n",buff);
    return pos;
}
 void set_noblock(int fd)
{
   int fd_flag = fcntl(fd,F_GETFL);
   if(fcntl(fd,F_SETFL,fd_flag|O_NONBLOCK)<0)
   {
     perror("fcntl");
     exit(11);
   }
}
int startup(const char *ip, int port)
{
   assert(ip!=NULL);
   int sock=0;
   if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
   {
     perror("socket");
     exit(1);
   }
   set_noblock(sock);
   int opt=1;
   setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
   struct sockaddr_in addr;
   addr.sin_family=AF_INET;
   addr.sin_port=htons(port);
   addr.sin_addr.s_addr=inet_addr(ip);
   if(bind(sock,(const struct sockaddr*)&addr,sizeof(addr))<0)
   {
     perror("bind");
     exit(2);
   }
   if(listen(sock,BACKLOG)<0)
   {
    perror("listen");
    exit(3);
   }
   return sock;
}
static void exe_cgi(int sock,char*method,char*path,char*query_para,int Content_length)
{
    pid_t id=0;
    const char* request_head="HTTP/1.1 200 OK\r\n";
    const char*blank="Content-Type:text/html\r\n\r\n";
    send(sock,request_head,strlen(request_head),0);
    send(sock,blank,strlen(blank),0);
    int in[2];
    int out[2];
    if(pipe(in)<0) {
    perror("pipe");
    return ;
    }
    if(pipe(out)<0) {
    perror("pipe");
    return ;
    }
    if((id=fork())<0){
    error("fork");
    return ;
   }
   if(id>0)
     {
      close(in[0]);
      close(out[1]);
      char c;
      if(query_para==NULL)
     {
       printf("length:%d sock:%d in:%d\n",Content_length,sock,in[0]);
       char *c=(char*)malloc(Content_length+1);
      // printf("%p:\n",c);
       if(recv(sock,c,Content_length,0)>0){
           printf("Post query:%s\n",c);
           write(in[1],c,Content_length);
        }else{
         perror("recv");
         return;
       }
     //  printf("%p:\n",c);
       free(c);

      // if(sendfile(in[1],sock,0,Content_length)<0){
      //   perror("sendfile");
        // pthread_exit((void*)10);
      // }
    }
     while(read(out[0],&c,1))
     {  
        send(sock,&c,1,0);
     }
     waitpid(id,NULL,0);
     close(in[1]);
     close(out[0]);
   }
   else{
     printf("i am child\n");
     close(in[1]);
     close(out[0]);
     if(dup2(out[1],1)<0){
     perror("dup2");
     }
     if(dup2(in[0],0)<0){
     perror("dup2");
     }
     char METHOD[SIZE/10];
     char QUERY[SIZE/10];
     char LENGTH[SIZE/10];
     sprintf(METHOD,"METHOD=%s",method);
     if(query_para)sprintf(QUERY,"QUERY_PARA=%s",query_para);
    // printf("METHOD:%s\nLength:%d",METHOD,Content_length);
     if(Content_length>0){
      // printf("bug?\n");
       sprintf(LENGTH,"CONTENT_LENGTH=%d",Content_length);
    }
   // printf("1111\n");
    putenv(METHOD);
   // printf("Length:%s\n",LENGTH);
    if(query_para){
     if(putenv(QUERY)!=0)
      {
        perror("putenv");
        return ;
      }
    }else{
      // printf("Length:%s\n",LENGTH);
        if(putenv(LENGTH)!=0){
         perror("putenv");
         return ;
        }
    }
    execl(path,path,NULL);// mei fan hui zhi  , fan hui bi shi bai
    printf("faild\n");
    perror("execl");
    exit(3);
  }
}     // exe_cgi(sock,method,path,NULL,Content-length);
void  handler_request(int sock)
{
   int cgi=-1;
   char buff[SIZE];
   int sz=get_line((int)sock,buff,SIZE);
   if(sz<0){
   printf("du cuo wu\n");
   goto end;
   }
   buff[sz]=0;
   printf("getline:%d\n",sz);
   printf("buff:%s\n",buff);
   char method[SIZE/10];
   char url[SIZE/10];
   char path[SIZE/10];
   int len=SIZE/10;
  char query_para[SIZE/10];
   int i=0,j=0;
   int Content_length=0;
   // char content_len
   while(j<len&&i<SIZE&&!isspace(buff[i]))
   {
       method[j++]=buff[i++];
   }
   method[j]=0;
   j=0;
   i++;
   while(j<len&&i<SIZE&&!isspace(buff[i]))
   {
      url[j++]=buff[i++];
   }
   url[j]=0;
   i=0,j=0;
   while(i<len&&j<len&&url[i]!='?'&&url[i]!='\0') //ke neng mei you ?
   {
       path[j++]=url[i++];
   }
      path[j]=0;
   j=0;
   if(url[i]=='?')
  {
     i++;// xian  tiao guo ?
    while(i<len&&j<len&&!isspace(buff[i]))
   {
     query_para[j++]=url[i++];
   }
  }
   query_para[j]=0;
/* qing qiu hang jie shu */
/////////////////////
/////////////////////
   i=0;
   int content_len=-1;
   do{
       int k=0;
       int pos=0;
       sz=get_line((int)sock,buff,SIZE);
       buff[sz]=0;
       if(sz<0){
       echo_err(sock);
       goto end;
       }
       //  while(k<SIZE&&buff[0]!='\n'&&(buff[k++])!=':');//paichu kong bai hang 
       while(k<SIZE&&buff[0]!='\n'&&!isspace(buff[k++]));
       buff[k-1]=0;
       pos=k;
       printf("buf:%s\n",buff);
       if(strcasecmp(buff,"Content-Length:")==0){
      // while(buff[k]!='\0') ++k;
      // buff[k]=0;
       printf("busss:%s\n",buff+pos);
       Content_length=atoi(buff+pos);
       printf("in Content:%d\n",Content_length);
     }
    }while(sz>0&&buff[0]!='\n');
/* qing qiu bao tou */
/////////////////
   struct stat st;
      if(strcmp(path,"/")==0)
    {
      strcpy(path,"./wwwroot/index.html");
    }
    else
    {
      char temp[SIZE/10]="./wwwroot";
      strcat(temp,path);
      memcpy(path,temp,strlen(temp)+1);
    }
   printf("path :%s\n",path);
   if(stat(path,&st)<0)
   {
    perror("stat");
    echo_err(sock);
    goto end;
   }
   else{
   if(S_ISDIR(st.st_mode)){
    printf("in S_ISDIR\n");
    printf("path :%s\n",path);
    strcpy(path,"./wwwroot/index.html");
    printf("path :%s\n",path);
    echo_www(sock,path,SIZE/10);
    goto end;
   }else if((st.st_mode&S_IXUSR)==S_IXUSR||(st.st_mode&S_IXOTH)==S_IXOTH||(st.st_mode&S_IXGRP)==S_IXGRP)
   {
   cgi=1;
   printf("in cgi\n");
   }
  }
   /* wen jian shu xing */
   /* chuli shuju*/
   printf("Method:%s\n",method);
   printf("url:%s\n",url);
   printf("path:%s\n",path);
   if(strcasecmp(method,"GET")==0){
      if(cgi<0){//j==0 ---> mei you ? suo yi qing qiu jing tai ziyuan
       echo_www(sock,path,len);
      }else{
           // cgi you can shu de GET
          exe_cgi(sock,method,path,query_para,-1);
      }
    }else{
      printf("do post\n");
      exe_cgi(sock,method,path,NULL,Content_length);
     //post
    }
 end:
    return;//close(sock)--> Read han shu zhong close
}

void echo_www(int sock, char* path,int len)
{
      const char* request_head="HTTP/1.1 200 OK\r\n";
      const char*blank="\r\n";
      int fd=-1;
   /*   if(strcmp(path,"/")==0) 
    {
      strcpy(path,"./wwwroot/index.html");
    }
    else
    {
      char temp[SIZE/10]="./wwwroot";
      strcat(temp,path);
      path=temp;
    }*/
      if((fd=open(path,O_RDONLY))<0)
      {
       perror("open");
       return;
      }
     if(send(sock,request_head,strlen(request_head),0)<0)
     {
       perror("send");
       close(fd);
       return;
     }
     if(send(sock,blank,strlen(blank),0)<0)
     {
         perror("send");
         close(fd);
         return;
     }
     if(sendfile(sock,fd,0,10240)<0)
     {
       perror("sendfile");
       close(fd);
       return;
     }
}
