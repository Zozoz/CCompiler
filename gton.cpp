#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <set>
using namespace std;
#define N 1000
#define M 1000000
map<char, int> n_flag, t_flag; // 终结符->计数
map<int, char> flag_n, flag_t; // 计数->终结符
int n_cnt, t_cnt; // 非终结符、终结符映射计数
int st, ed; //始态、终态序号
int blank_num; //‘#’序号
int f[N][N]; //NFA图i->f (by j)
// ‘S'代表始态，'Z'代表终止态，'#'代表空
set<int> myset[1000];
int d[N][N]; //DNF图i->d (by j)

//-----------关键字表-----------
char *key[100];
int num_key;
char filename[] = "keyword.txt";

//-----------边界符表-----------
char boundword[] = ";,\":().[]{}";

//-----------运算符表----------
char operatorword[] = "=+-*/<>%!~^&";

struct NFA{
    char ch;
    int to;
    int next;
    bool isEnd;
}nfa[M];
int head_n[N];
int pn;

struct DFA{
    int to;
    char ch;
    int next;
    bool isEnd;
}dfa[M];
int head_d[N];
int pd;

struct Node{
    char type[20];
    char value[20];
    int line;
    bool flag;
    Node *next;
}*root, *p;

void get_keyword(){
    fstream fin;
    fin.open(filename);
    if (!fin){
        cout<<"error"<<endl;
        exit(1);
    }
    char ch[50];
    num_key = 0;
    while(fin.getline(ch, sizeof(ch))){
        int len = strlen(ch) + 1;
        key[num_key] = new char[len];
        strcpy(key[num_key], ch);
        key[num_key][len] = '\0';
        num_key++;
    }
    for(int i=0; i<num_key; i++){
        cout<<key[i]<<endl;
    }
}

void init(){
    get_keyword();
    n_cnt = t_cnt = 0;
    memset(head_n, -1, sizeof(head_n));
    pn = 0;
    memset(head_d, -1, sizeof(head_d));
    pd = 0;
    root = p = NULL;
}

void add_nfa(int u, int v, char ch){
    nfa[pn].ch = ch;
    nfa[pn].to = v;
    nfa[pn].next = head_n[u];
    head_n[u] = pn++;
}

void add_dfa(int u, int v, char ch){
    dfa[pd].ch = ch;
    dfa[pd].to = v;
    dfa[pd].next = head_d[u];
    head_d[u] = pd++;
}

// 把终结符和非终结符映射到数字计数
void change_to_map(char ch){
    if (ch >= 'A' && ch <= 'Z'){
        if (n_flag[ch] == 0){
            n_flag[ch] = ++n_cnt;
            flag_n[n_cnt] = ch;
        }
    }else if (ch == '#'){
        if (t_flag[ch] == 0){
            t_flag[ch] = ++t_cnt;
            blank_num = t_cnt;
            flag_t[t_cnt] = ch;
        }
        if (n_flag['Z'] == 0){
            n_flag['Z'] = ++n_cnt;
            flag_n[n_cnt] = 'Z';
        }
    }
    else{
        if (t_flag[ch] == 0){
            t_flag[ch] = ++t_cnt;
            flag_t[t_cnt] = ch;
        }
    }
}

//正规文法转换成NFA
bool g_to_n(){
    memset(f, 0, sizeof(f));
    init();
    char ch1, ch2, ch3, ch4, ch5, ch6;
    FILE *fp = NULL;
    fp = fopen("g.txt", "r");
    change_to_map('S');
    change_to_map('Z');
    while(true){
        ch1 = fgetc(fp);
        if (ch1 == EOF){
            break;
        }
        ch2 = fgetc(fp);
        ch3 = fgetc(fp);
        ch4 = fgetc(fp);
        ch5 = fgetc(fp);
        change_to_map(ch1);
        change_to_map(ch4);
        if (ch5 == '\n'){
            add_nfa(n_flag[ch1], n_flag['Z'], ch4);
        }else{
            change_to_map(ch5);
            add_nfa(n_flag[ch1], n_flag[ch5], ch4);
            fgetc(fp);
        }
    }

    cout<<"NFA:"<<endl;
    for (int i=1; i<=n_cnt; i++){
        for (int j=head_n[i]; j!=-1; j=nfa[j].next){
            int to = nfa[j].to;
            char ch = nfa[j].ch;
            cout<<i<<"->"<<to<<" by "<<ch<<endl;;
        }
    }

    return true;
}

void get_closure(set<int> T, char ch, set<int> &tmp, bool (&vst)[N]){
    for(int x : T){
        tmp.insert(x);
        vst[x] = true;
        for (int i=head_n[x]; i!=-1; i=nfa[i].next){
            int to = nfa[i].to;
            if (ch == nfa[i].ch && !vst[to]){
                set<int> tp;
                tp.insert(to);
                tmp.insert(to);
                vst[to] = true;
                get_closure(tp, ch, tmp, vst);
            }
        }
    };
}

void get_move(set<int> T, char ch, set<int> &tmp){
    tmp.clear();
    for (int x : T){
        for (int i=head_n[x]; i!=-1; i=nfa[i].next){
            int to = nfa[i].to;
            if (ch == nfa[i].ch){
                tmp.insert(to);
            }
        }
    }
}

int is_new_set(int len, set<int> tmp, int mylen){
    for (int i=1; i<len; i++){
        /*if (i == mylen){
            continue;
        }*/
        if (tmp == myset[i]){
            return i;
        }
    }
    return len;
}

//NFA转换成DFA
void n_to_d(){
    bool vst[1000] = {false};
    st = n_flag['S'];
    ed = n_flag['Z'];
    set<int> tmp;
    tmp.insert(st);
    get_closure(tmp, '#', myset[1], vst);
    cout<<endl;
    int len_of_c = 1;
    int index = 1;
    while(true){
        for(int i=1; i<=t_cnt; i++){
            char ch = flag_t[i];
            if (ch == '#'){
                continue;
            }
            get_move(myset[index], ch, tmp);
            if (tmp.size() == 0){
                continue;
            }
            myset[++len_of_c].clear();
            memset(vst, false, sizeof(vst));
            get_closure(tmp, '#', myset[len_of_c], vst);
            int id = is_new_set(len_of_c, myset[len_of_c], len_of_c);
            if (myset[len_of_c].size() == 0){
                len_of_c--;
            }else{
                if (id != len_of_c){
                    len_of_c--;
                }
                add_dfa(index, id, ch);
            }
        }
        index++;
        if(index > len_of_c){
            break;
        }
    }

    cout<<"DFA:"<<endl;
    for (int i=1; i<=len_of_c; i++){
        for (int j=head_d[i]; j!=-1; j=dfa[j].next){
            int to = dfa[j].to;
            char ch = dfa[j].ch;
            cout<<i<<"->"<<to<<" by "<<ch<<endl;
        }
    }
}

void add_display(char *type, char *value, int line, bool flag){
    Node *q = new Node;
    if (flag){
        strcpy(q->type, type);
    }else{
        strcpy(q->type, "error");
    }
    strcpy(q->value, value);
    q->line = line;
    q->flag = flag;
    q->next = NULL;
    if(root == NULL){
        root = q;
    }else{
        p->next = q;
    }
    p = q;
}

bool isbound(char ch, int line){
    int len = strlen(boundword);
    for (int i=0; i<len; i++){
        if (ch == boundword[i]){
            return true;
        }
    }
    /*
    len = strlen(operatorword);
    for (int i=0; i<len; i++){
        if (ch == operatorword[i]){
            return true;
        }
    }*/
    return false;
}

void display(){
    p = root;
    Node *q;
	fstream ct;
	ct.open("lex_analyze.txt",ios::out);
    while(p != NULL){
        cout<<"(   "<<p->line<<"  ,  "<<p->type<<"  ,  "<<p->value<<"   )"<<endl;
        ct<<p->line<<" "<<p->value<<" "<<p->type<<endl;
        q = p;
        p = p->next;
        delete q;
    }
}

char *get_type(char * word){
    char *type;
    char tmp1[15] = "keyword";
    char tmp2[15] = "identifier";
    char tmp3[15] = "boundword";
    char tmp4[15] = "number";
    char tmp5[15] = "operator";
    type = (char *)malloc(sizeof(char)*15);
    if (word[0] >= '0' && word[0] <= '9'){
        strcpy(type, tmp4);
    }else if (word[0] == '_' || (word[0] >= 'a' && word[0] <= 'z') || (word[0] >= 'A' && word[0] <= 'Z')){
        int i = 0;
        for (i=0; i<num_key; i++){
            if (strcmp(key[i], word) == 0){
                strcpy(type, tmp1);
                break;
            }
        }
        if (i == num_key){
            strcpy(type, tmp2);
        }
    }else{
        int i = 0;
        int len = strlen(operatorword);
        for (i=0; i<len; i++){
            if (word[0] == operatorword[i]){
                strcpy(type, tmp5);
                break;
            }
        }
        len = strlen(boundword);
        for (i=0; i<len; i++){
            if (word[0] == boundword[i]){
                strcpy(type, tmp3);
                break;
            }
        }
    }
    return type;
}

void process(){
    st = n_flag['S'];
    ed = n_flag['Z'];
    cout<<"st="<<st<<" ed="<<ed<<endl;
    FILE *fp = NULL;
    char ch;
    int stat;
    fp = fopen("source.c", "r");
    ch = fgetc(fp);
    stat = st;
    char *word;
    char *type;
    char tmp[50];
    int line = 1;
    int cnt = 0;
    bool flag = false;
    while(true){
        flag = false;
        for (int i=head_d[stat]; i!=-1; i=dfa[i].next){
            int to = dfa[i].to;
            char toch = dfa[i].ch;
            if (toch == ch){
                flag = true;
                stat = to;
                tmp[cnt++] = ch;
                break;
            }
        }
        if (!flag){
            tmp[cnt] = '\0';
            word = (char *)malloc(sizeof(char)*(cnt+1));
            memcpy(word, tmp, cnt+1);
            bool fl = false;
            if (ch == ' ' || ch == '\n'){
                if(strlen(tmp) != 0){
                    add_display(get_type(word), word, line, true);
                }
                free(word);
            }else if (isbound(ch, line)){
                if(strlen(tmp) != 0){
                    add_display(get_type(word), word, line, true);
                }
                free(word);
                word = (char *)malloc(sizeof(char)*2);
                word[0] = ch;
                word[1] = '\0';
                add_display(get_type(word), word, line, true);
                free(word);
            }
            else{
                add_display(word, word, line, false);
            }
            stat = st;
            cnt = 0;
            memset(tmp, '\0', sizeof(tmp));
        }
        ch = fgetc(fp);
        if (ch == '\n'){
            line++;
        }
        if (ch == EOF){
            break;
        }
    }
    display();
}

int main(){
    bool flag = g_to_n();
    n_to_d();
    process();
    return 0;
}


