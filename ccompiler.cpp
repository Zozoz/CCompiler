#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <map>
using namespace std;


//-----------关键词表-----------
string keyword[] = {"auto", "short", "int", "long", "float", "double","char",
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
}

void pro(){}


int main(){
    char ch;
    char pre;
    int line = 1;
    int count;
    char tmp[50];
    char *word;
    int isD, isE;
    FILE *fp;
    if((fp = fopen("1.txt", "r")) == NULL){
        printf("文件不存在");
        return 0;
    }
    init();
    while(ch != EOF){
        ch = fgetc(fp);
        //printf("%c\n",ch);
        //处理注释
        if(ch == '/'){
            pre = ch;
            ch = fgetc(fp);

            if(ch == '/'){ //处理 ‘//’类型注释
                while(ch != '\n'){
                    pre = ch;
                    ch = fgetc(fp);
                }
                pro();
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
                pro();
            }else{ //处理运算符‘/’
                pro();
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

            cout<<word<<endl;

            pro();
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

                cout<<tmp<<endl;
            }

            word = (char *)malloc(sizeof(char)*(count+1));
            memcpy(word, tmp, count);
            word[count] = '\0';
            if(isD == 1){
                pro(); // 带小数点实数
            }else if(isE == 1){
                pro(); // 科学计数法表示的实数
            }else if(isD != -1 && isE != -1){
                pro(); // 整数
            }else{
                pro(); // 出错
            }
            fseek(fp, -1L, SEEK_CUR);
        }else if(ch == '\n' || ch == ' ' || ch == '\t' || ch == '\r'){ // 处理换行
            if(ch == '\n'){
                line ++;
            }
        }else if(ch == '#'){ // 处理头文件和宏常量
            pro();
        }else{
            for(int i=0; i<leno; i++){
                if(ch == operatorword[i]){
                    pro();
                }
            }
            for(int i=0; i<lenb; i++){
                if(ch == boundword[i]){
                    pro();
                }
            }
        }
    }
    return 0;
}
