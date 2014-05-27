#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <map>
using namespace std;
#define IDEN "标识符"
#define KEY "关键词"
#define OPERATOR "运算符"
#define BOUND "限界符"
#define COMMENT1 "注释//"
#define COMMENT2 "注释/**/"
#define INT "整型"
#define DECIMAL "实型"
#define SCIENCE "科学计数法"
#define ERROR "错误"

//-----------关键词表-----------
string keyword[32] = {"auto", "short", "int", "long", "float", "double","char",
    "struct", "union", "enum", "typedef", "const", "unsigned", "signed",
    "extern", "register", "static", "volatile", "void", "if", "else",
    "switch", "case", "for", "do", "while", "goto", "continue", "break",
    "default", "sizeof", "return"
};

//-----------运算符表-----------
char operatorword[] = "+-*/<>";

//-----------限界符表-----------
char boundword[] = "=;,\":().[]{}";

map<string, string> mymap;
int leno, lenb;


struct Node{
    char type[20];
    char value[20];
    int line;
    Node *next;
}*root, *p;

void init(){
    leno = strlen(operatorword);
    lenb = strlen(boundword);

    mymap["="] = "EQU";
    mymap[";"] = "SEM";
    mymap[","] = "COM";
    mymap["\""] = "DOUBLE-QUO";
    mymap[":"] = "COL";
    mymap["("] = "LEFT_PAR";
    mymap[")"] = "RIGHT_PAR";

    mymap["+"] = "ADD";
    mymap["-"] = "SUB";
    mymap["*"] = "MUL";
    mymap["/"] = "DIV";
    mymap["<"] = "LESS_THAN";
    mymap[">"] = "GTEATER_THAN";

    root = p = NULL;
}

void pro(char *type, char *value, int line){
    Node *q = new Node;
    strcpy(q->type, type);
    strcpy(q->value, value);
    q->line = line;
    q->next = NULL;
    if(root == NULL){
        root = q;
    }else{
        p->next = q;
    }
    p = q;
}


char *stringTochar(string s){
    char *ch;
    int len = s.length();
    ch = new char[len+1];
    for(int i=0; i<len; i++){
        ch[i] = s[i];
    }
    ch[len] = '\0';
    return ch;
}

void test(){
    char *ch = stringTochar("nihao");
    cout<<ch<<endl;
    cout<<"end"<<endl;
}

void display(){
    p = root;
    Node *q;
    while(p != NULL){
        cout<<"(   "<<p->line<<"  ,  "<<p->type<<"  ,  "<<p->value<<"   )"<<endl;
        q = p;
        p = p->next;
        delete q;
    }
}


int main(){
    char ch;
    char pre;
    int line = 1;
    int count;
    char tmp[50];
    char *word = NULL;
    int isD, isE;
    FILE *fp = NULL;
    if((fp = fopen("1.txt", "r")) == NULL){
        printf("文件不存在");
        return 0;
    }
    init();
    while(ch != EOF){
        ch = fgetc(fp);
        //处理注释
        if(ch == '/'){
            pre = ch;
            ch = fgetc(fp);

            if(ch == '/'){ //处理 ‘//’类型注释
                while(ch != '\n'){
                    pre = ch;
                    ch = fgetc(fp);
                }
                if(ch == '\n'){
                    line ++;
                }
                pro(stringTochar(COMMENT1), stringTochar(""), line);
            }else if(ch == '*'){ //处理 ‘/* */’类型注释
                pre = fgetc(fp);
                ch = fgetc(fp);
                while(!(pre == '*' && ch == '/')){
                    pre = ch;
                    ch = fgetc(fp);
                    if(ch == '\n'){
                        line ++;
                    }
                }
                pro(stringTochar(COMMENT2), stringTochar(""), line);
            }else{ //处理运算符‘/’
                pro(stringTochar(OPERATOR), stringTochar("/"), line);
            }
        }else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'){ //处理关键字和标识符
            count = 0;
            while((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'){
                tmp[count++] = ch;
                ch = fgetc(fp);
            }
            word = (char *)malloc(sizeof(char)*(count+1));
            memcpy(word, tmp, count);
            word[count] = '\0';
            string tt;
            for(int i=0; i<count; i++){
                tt[i] = tmp[i];
            }
            tt[count] = '\0';
            bool flag = false;
            for(int i=0; i<32; i++){
                if(keyword[i] == tt){
                    pro(stringTochar(KEY), word, line);
                    flag = true;
                }
            }
            if(!flag){
                pro(stringTochar(IDEN), word, line);
            }
            free(word);
            fseek(fp, -1L, SEEK_CUR); //回退1个char
        }else if(ch >= '0' && ch <= '9'){ //处理数字常量
            count = 0;
            isD = isE = 0;
            while(ch >= '0' && ch <= '9'){
                tmp[count++] = ch;
                ch = fgetc(fp);
            }
            //处理带小数点实数
            if(ch == '.'){
                isD = 1;
                tmp[count++] = ch;
                ch = fgetc(fp);
                if(ch >= '0' && ch <= '9'){
                    while(ch >= '0' && ch <= '9'){
                        tmp[count++] = ch;
                        ch = fgetc(fp);
                    }
                }else{
                    isD = -1; //出错，-1表示小数点后没有数字
                }
            }
            //处理科学计数法表示的实数
            if(ch == 'E' || ch == 'e'){
                isE = 1;
                tmp[count++] = ch;
                ch = fgetc(fp);
                if(ch == '+' || ch == '-'){
                    tmp[count++] = ch;
                    ch = fgetc(fp);
                }
                if(ch >= '0' && ch <= '9'){
                    while(ch >= '0' && ch <= '9'){
                        tmp[count++] = ch;
                        ch = fgetc(fp);
                    }
                }else{
                    isE = -1; //出错，-1表示E后面的格式不对
                }
            }
            word = (char *)malloc(sizeof(char)*(count+1));
            memcpy(word, tmp, count);
            word[count] = '\0';
            if(isD == 1){
                pro(stringTochar(DECIMAL), word, line); // 带小数点实数
            }else if(isE == 1){
                pro(stringTochar(SCIENCE), word, line); // 科学计数法表示的实数
            }else if(isD != -1 && isE != -1){
                pro(stringTochar(INT), word, line); // 整数
            }else{
                pro(stringTochar(ERROR), word, line); // 出错
            }
            free(word);
            fseek(fp, -1L, SEEK_CUR);
        }else if(ch == '\n' || ch == ' ' || ch == '\t' || ch == '\r'){ // 处理换行
            if(ch == '\n'){
                line ++;
            }
        }else if(ch == '#'){ // 处理头文件和宏常量
            //pro();
        }else{
            word = (char *)malloc(sizeof(char)*20);
            for(int i=0; i<leno; i++){
                if(ch == operatorword[i]){
                    word[0] = ch;
                    word[1] = '\0';
                    pro(stringTochar(OPERATOR), word, line);
                }
            }
            for(int i=0; i<lenb; i++){
                if(ch == boundword[i]){
                    word[0] = ch;
                    word[1] = '\0';
                    pro(stringTochar(BOUND), word, line);
                }
            }
            free(word);
        }
    }
    display();
    return 0;
}
