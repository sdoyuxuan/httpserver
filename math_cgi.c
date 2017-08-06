#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void math(char * Method,char * query_para)
{
    char *buff=malloc(atoi(query_para)+1);
    int sz; 
    int para[2];
    if(strcasecmp(Method,"GET")){
       read(0,buff,atoi(query_para));
       if(sz<0){
       perror("read");  
       exit(1);
       }
       buff[sz]=0;
       query_para=buff;
    }   
    int idx=0;
    while(*query_para!='\0')
   {   
      if(*query_para++=='='){
         para[idx++]=atoi(query_para);
       if(idx==2) break;
      }
   }
    float left=para[0];
    float right=para[1];
    printf("<html>\n");
    printf("\n");
    printf("<p>%f+%f=%f<p>\n",left,right,left+right);
    printf("<p>%f*%f=%f<p>\n",left,right,left*right);
   if(right!=0) printf("<p>%f/%f=%f<p>\n",left/right,left,left/right);
   if(para[1]!=0) printf("<p>%d%%%d=%d<p>\n",para[0],para[1],para[0]%para[1]);
    printf("<p>%f-%f=%f<p>\n",left,right,left-right);
    printf("</body>\n");
    printf("</html>");
    free(buff);
}
int main()
{
   char *Method;
   char *query_para;
   Method=getenv("METHOD");
   if(strcasecmp(Method,"GET")==0)
   {
      if(NULL==(query_para=getenv("QUERY_PARA"))){
       printf("get query failed\n");
      }
   }
   else {
    query_para=getenv("CONTENT_LENGTH");
   }
   //printf("Method:%s\n",Method);
  // printf("query_para:%s\n",query_para);
   math(Method,query_para);
   return 0;
}
