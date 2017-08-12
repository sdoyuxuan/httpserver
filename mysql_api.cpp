#include "mysql_api.h"
void Init(MYSQL&conn)
{
   if(mysql_init(&conn)== NULL) {
      mysql_error(&conn);
      exit(1);
}
           if(mysql_real_connect(&conn,"127.0.0.1","root","NULL","user",3306,NULL,0)==NULL){
   mysql_error(&conn);
   fflush(stdout);
   exit(2);
 }
}
int Insert(MYSQL&conn,char * name,char*school)
{
  // cout<<name<<endl;
 //  cout<<school<<endl;
   string temp("INSERT INTO user(name,school) values('");
   temp+=name;
   temp+="','";
   temp+=school;
   temp+="')";
  // cout<<temp<<endl;
   int res=mysql_query(&conn,temp.c_str());
   if(res!=0){
     mysql_error(&conn);
     exit(3);
    }
   cout<<"Insert success"<<endl;
   return 0;
}
int select(MYSQL&conn)
{
    MYSQL_RES*result;
    MYSQL_FIELD * fd;
    MYSQL_ROW sql_row;
    int res=mysql_query(&conn,"select * from user_info");
    if(res!=0){
     mysql_error(&conn);
     exit(4);
    }
    result=mysql_store_result(&conn);
    if(result){
     for(;fd=mysql_fetch_field(result);)
     {
        cout<<" | "<<fd->name; // da yin lie ming 
     }
     cout<<" | "<<endl;
     int col_nums=mysql_num_fields(result);
     int idx=0;
     while(sql_row=mysql_fetch_row(result))//fan hui yige zifu shuzu 
    {                                    //meige zifuchuang 
        for(;idx<col_nums;++idx)      // duiying dui ying hang de 
       {                             // lie xinxi   
          cout<<sql_row[idx]<<"\t";
       }
       cout<<endl;
   }
   }else
   {
     mysql_error(&conn);
     exit(5);
   }
    mysql_free_result(result);
}
