#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <fstream>
#include <iomanip>
using namespace std;
struct Production{
    string left;
    vector< string > right;
    Production(){}
    Production(string l, vector<string> r):left(l), right(r){}
};

class LL1{
#define N 100
    private:
        vector< Production > syntax;
        set< string > Vn, Vt;
        map< string, set<string> > first, follow;
        map< string, vector<string> > forecast[N];
        stack< string > status, input, desc;
        vector< string > save[N];
    public:
        bool is_letter(string str){
            if (str[0] >= 'A' && str[0] <= 'Z'){
                return true;
            }else{
                return false;
            }
        }

	    vector<string> split(string str,string pattern)
	    {
	    	string::size_type pos;
	    	vector<string> result;
	    	str+=pattern;//扩展字符串以方便操作
	    	int size=str.size();

	    	for(int i=0; i<size; i++)
	    	{
	    		pos=str.find(pattern,i);
	    		if(pos<size)
	    		{
	    			string s=str.substr(i,pos-i);
	    			result.push_back(s);
	    			i=pos+pattern.size()-1;
	    		}
	    	}
	    	return result;
	    }

        void get_syntax(){
            string tmp;
            fstream fin;
            fin.open("syntax_grammar.txt", ios::in);
            if (fin.good()){
                cout<<"Open success!"<<endl;
            }else{
                cout<<"Open failed!"<<endl;
                exit(1);
            }
            while(getline(fin,tmp)){
                int pos = 0;
                for (int i=0; i<tmp.length(); i++){
                    if (tmp[i] == '-'){
                        pos = i;
                        break;
                    }
                }
                vector< string > result = split(tmp, " ");
                Production p;
                p.left = result[0];
                if (is_letter(p.left)){
                    Vn.insert(p.left);
                }else{
                    Vt.insert(p.left);
                }
                for (int i=2; i<result.size(); i++){
                    string t = result[i];
                    p.right.push_back(t);
                    if (is_letter(t)){
                        Vn.insert(t);
                    }else{
                        Vt.insert(t);
                    }
                }
                syntax.push_back(p);
            }
        }

        void display(){
            set< string >::iterator it;
            for (it=Vn.begin(); it!=Vn.end(); it++){
                cout<<*it<<" ";
            }
            cout<<endl;
            for (it=Vt.begin(); it!=Vt.end(); it++){
                cout<<*it<<" ";
            }
            cout<<endl;
            for (int i=0; i<syntax.size(); i++){
                cout<<syntax[i].left<<"->";
                for (int j=0; j<syntax[i].right.size(); j++){
                    cout<<syntax[i].right[j];
                }
                cout<<endl;
            }
        }

        vector< string > get_first(vector< string > cur){
           vector< string > first;
           for (int i=0; i<cur.size(); i++){
               if (Vt.find(cur[i]) != Vt.end()){ // 如果当前为终结符，直接返回
                   first.push_back(cur[i]);
                   return first;
               }
               bool flag = true;
               for (int j=0; j<syntax.size(); j++){
                   if (syntax[j].left == cur[i]){
                       vector< string > tmp;
                       tmp = get_first(syntax[j].right);
                       for (int k=0; k<tmp.size(); k++){
                           first.push_back(tmp[k]);
                       }
                       if (find(tmp.begin(), tmp.end(), "$") == tmp.end()){
                           flag = false;
                       }else{
                           flag = true;
                       }
                   }
               }
               if (!flag) break;
           }
           return first;
        }

        bool can_to_none(vector< string > vec){
            for (int i=0; i<vec.size(); i++){
                string cur = vec[i];
                if (cur == "$") continue;
                if (Vt.find(cur) != Vt.end()){
                    return false;
                }
                bool flag = false;
                for (int j=0; j<syntax.size(); j++){
                    if (syntax[j].left == cur){
                        flag = can_to_none(syntax[j].right);
                        if (flag) break;
                    }
                }
                if (!flag){
                    return false;
                }
            }
            return true;
        }

        set< string > get_follow(string cur){
            set< string > result;
            if (cur == "S"){
                result.insert("#");
            }
            for (int i=0; i<syntax.size(); i++){
                for (int j=0; j<syntax[i].right.size(); j++){
                    if (cur == syntax[i].right[j]){
                        vector< string > fl;
                        for (int k=j+1; k<syntax[i].right.size(); k++){
                            fl.push_back(syntax[i].right[k]);
                        }
                        vector< string > ft = get_first(fl);
                        for (int k=0; k<ft.size(); k++){
                            if(ft[k] != "$") result.insert(ft[k]);
                        }
                        if (can_to_none(fl)){
                            set< string > tmp = get_follow(syntax[i].left);
                            for (set<string>::iterator it=tmp.begin(); it!=tmp.end(); it++){
                                result.insert(*it);
                            }
                        }
                    }
                }
            }
            return result;
        }

        int get_id(string cur){
            int cnt = 0;
            for (set<string>::iterator it=Vn.begin(); it!=Vn.end(); it++){
                if (*it == cur){
                    return cnt;
                }
                cnt++;
            }
        }

        void make_forecast(){
            for (int i=0; i<syntax.size(); i++){
                vector< string > fst = get_first(syntax[i].right);
                set< string > fl = follow[syntax[i].left];
                set< string > tmp;
                for (int j=0; j<fst.size(); j++){
                    tmp.insert(fst[j]);
                }
                if (can_to_none(syntax[i].right)){
                    for (set<string>::iterator it=fl.begin(); it!=fl.end(); it++){
                        tmp.insert(*it);
                    }
                    tmp.erase("$");
                }
                int id = get_id(syntax[i].left);
                for(set<string>::iterator it=tmp.begin(); it!=tmp.end(); it++){
                    forecast[id][*it] = syntax[i].right;
                    if (syntax[i].right[0] == "e"){
                        cout<<"here:"<<syntax[i].left<<" to "<<"e by "<<*it<<endl;
                    }
                }
            }
            cout<<setw(10)<<" ";
            for (set<string>::iterator it=Vt.begin(); it!=Vt.end(); it++){
                cout<<setw(10)<<*it;
            }
            cout<<endl;
            for (set<string>::iterator it=Vn.begin(); it!=Vn.end(); it++){
                int id = get_id(*it);
                cout<<setw(10)<<*it;
                for (set<string>::iterator t=Vt.begin(); t!=Vt.end(); t++){
                    cout<<setw(10);
                    for (int j=0; j<forecast[id][*t].size(); j++){
                        cout<<forecast[id][*t][j];
                    }
                    if (forecast[id][*t].size() == 0){
                        cout<<" ";
                    }
                }
                cout<<endl;
            }
        }

        void analyse(vector< string > str){
            string s1, s2, s3;
            status.push("#");
            status.push("S");
            s1 += "#";
            s1 += "S";
            for (int i=str.size()-1; i>=0; i--){
               input.push(str[i]);
               s2 += str[i];
            }
            int cnt = 1;
            while (true){
                save[cnt].push_back(s1);
                save[cnt].push_back(s2);
                cout<<setw(20)<<cnt<<setw(20)<<s1<<setw(20)<<s2;
                if (status.top() == input.top() && input.top() == "#"){
                    s3 = "acc";
                    save[cnt].push_back(s3);
                    cout<<setw(20)<<s3<<endl;
                    break;
                }
                string from, to, by;
                from = status.top();
                by = input.top();
                if (from == by){
                    status.pop();
                    input.pop();
                    s1 = s1.substr(0, s1.length()-1);
                    s2 = s2.substr(0, s2.length()-1);
                    s3 = by + "匹配成功";
                    save[cnt].push_back(s3);
                    cout<<setw(20)<<s3<<endl;
                    cnt++;
                    continue;
                }
                int id = get_id(from);
                if (!forecast[id][by].empty()){
                    status.pop();
                    s1 = s1.substr(0, s1.length()-1);
                    string tmp = from + "->";
                    for (int i=forecast[id][by].size()-1; i>=0; i--){
                        s1 += forecast[id][by][i];
                        tmp += forecast[id][by][i];
                        if (forecast[id][by][i] != "$"){
                            status.push(forecast[id][by][i]);
                        }
                    }
                    s3 = tmp;
                    save[cnt].push_back(s3);
                    cout<<setw(20)<<s3<<endl;
                }
                cnt++;
            }
            cout<<"Analyse successfully!"<<endl;
        }

        void process(){
            for (set<string>::iterator it=Vn.begin(); it!=Vn.end(); it++){
                set< string > result1;
                for (int i=0; i<syntax.size(); i++){
                    if (*it == syntax[i].left){
                        vector< string > tt = get_first(syntax[i].right);
                        for (int k=0; k<tt.size(); k++){
                            result1.insert(tt[k]);
                        }
                    }
                }
                first[*it] = result1;
                cout<<"First("<<*it<<") = ";
                for (set<string>::iterator t=result1.begin(); t!=result1.end(); t++){
                    cout<<*t<<" ";
                }cout<<endl;
            }
            set< string > result;
            result = get_follow("S");
            follow["S"] = result;
            while (true){
                bool flag = true;
                for (set<string>::iterator it=Vn.begin(); it!=Vn.end(); it++){
                    if (*it == "S") continue;
                    result = get_follow(*it);
                    if (follow[*it] != result){
                        follow[*it] = result;
                        flag = false;
                    }
                }
                if (flag) break;
            }
            for (set<string>::iterator it=Vn.begin(); it!=Vn.end(); it++){
                cout<<"Follow("<<*it<<") = ";
                for (set<string>::iterator t=follow[*it].begin(); t!=follow[*it].end(); t++){
                    cout<<*t<<" ";
                }
                cout<<endl;
            }
            make_forecast();
            vector< string > input;
            string str = "aaabd#";
            for (int i=0; i<str.length(); i++){
                string tmp = str.substr(i, 1);
                input.push_back(tmp);
            }
            analyse(input);
        }

};

int main(){
    LL1 ll;
    ll.get_syntax();
    ll.display();
    ll.process();
    return 0;
}
