#ifndef MYSQL_API_H 
#define MYSQL_API_H 
#include<iostream>
#include"mysql/mysql.h"
#include"mysql/my_global.h"
#include<stdlib.h>
#include<string.h>
extern const  size_t SIZE;
using namespace std;
void  Init(MYSQL&conn);
int Insert(MYSQL&conn,char * name,char * gender);
int select(MYSQL&conn);
#endif
