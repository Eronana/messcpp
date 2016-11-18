#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "lexer.hpp"
#include <cstdlib>
#include <algorithm>
using namespace std;
#define MESS_LIST_FILE "mess_list.txt"

map<string,string> mess_table {
    {"0","0x0"},
    {"1","x0x"},
    {"(","Xxx"},
    {")","xxX"},
    {"+","xxx"},
    {"<<","XXX"},
};
vector<string> mess_name;
bool read_messlist()
{
    FILE *fp=fopen(MESS_LIST_FILE,"r");
    if(!fp)return false;
    char buf[100];
    while(~fscanf(fp,"%s",buf))mess_name.push_back(buf);
    fclose(fp);
    sort(mess_name.begin(),mess_name.end());
    mess_name.erase(unique(mess_name.begin(),mess_name.end()),mess_name.end());
    random_shuffle(mess_name.begin(),mess_name.end());
    return true;
}
int next_name;
string mess(string &s)
{
    if(!mess_table.count(s))
    {
        if(next_name==mess_name.size())throw runtime_error("no enough mess name");
        mess_table[s]=mess_name[next_name];
        next_name++;
    }
    return mess_table[s];
}

string mess_int(int a)
{
    if(!a)return mess_table["0"];
    else if(a==1)return mess_table["1"];
    string s=mess_table["("]+' '+mess_int(a>>1)+' '+mess_table["<<"]+' '+mess_table["1"]+' '+mess_table[")"];
    if(a&1)s=mess_table["("]+' '+s+' '+mess_table["+"]+' '+mess_table["1"]+' '+mess_table[")"];
    return s;
}

string mess_int(const string &s)
{
    return mess_int(atoi(s.c_str()));
}

string mess_hexint(const string &s)
{
    int v=0;
    for(int i=s.length()-1,base=1;i>1;i--,base*=16)
    {
        int a;
        char x=s[i];
        if(x>='0'&&x<='9')a=x-'0';
        if(x>='a'&&x<='f')a=x-'a'+10;
        if(x>='A'&&x<='F')a=x-'A'+10;
        v+=a*base;
    }
    return mess_int(v);
}

void print(const char *s)
{
    while(*s)putchar(*s++);
}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("Usage:\n%s in.cpp out.cpp\n",argv[0]);
        return 1;
    }
    if(!read_messlist())
    {
        fprintf(stderr,"Cannot open %s\n",MESS_LIST_FILE);
        return 1;
    }
    char *inFile=argv[1],*outFile=argv[2];
    Lexer lexer;
    if(!lexer.load(inFile))
    {
        fprintf(stderr,"Cannot open in file %s\n",inFile);
        return 1;
    }
    FILE *fp=fopen(outFile,"w");
    if(!fp)
    {
        fprintf(stderr,"Cannot open out file %s\n",outFile);
        return 1;
    }
    
    string preprocessing;
    string source_body;
    set<string> noReplace;
    bool is_macro=false;
    int macro_count=0;
    for(TOKEN t;(t=lexer.next()).token!=TOKEN_EOF;)
    {
        string mname;
        switch(t.token)
        {
        case TOKEN_ERROR:
            fprintf(stderr,"Error in line: %d, colum: %d\nRaw: %s\nError message: %s\n",t.line,t.col,t.raw.c_str(),t.msg.c_str());
            return 1;
        case TOKEN_POUND:
            t=lexer.next();
            if(t.raw=="define")
            {
                string macro_name=lexer.next().raw;
                int x=macro_name.find('(');
                if(x!=string::npos)noReplace.insert(macro_name.substr(0,x));
                preprocessing+="#define "+macro_name+" "+lexer.next().raw+"\n";
            }
            else if(t.raw=="include")preprocessing+="#include "+lexer.next().raw+"\n";
            else
            {
                fprintf(stderr,"Unknow preprocessing instruction: %s\n",t.raw.c_str());
                return 1;
            }
            break;
        case TOKEN_INTEGER:
            source_body+=mess_int(t.raw)+' ';
            break;
        case TOKEN_HEX_INTEGER:
            source_body+=mess_hexint(t.raw)+' ';
            break;
        case TOKEN_COMMA:
            if(is_macro)
            {
                source_body+=',';
                break;
            }
        case TOKEN_LEFT_PAREN:
            if(is_macro)
            {
                source_body+='(';
                break;
            }
        case TOKEN_RIGHT_PAREN:
            if(is_macro)
            {
                source_body+=')';
                is_macro=false;
                break;
            }
        default:
            try
            {
                mname=mess(t.raw); 
            }
            catch(runtime_error err)
            {
                fprintf(stderr,"Error: %s\n",err.what());
                return 1;
            }
            if(noReplace.count(t.raw))
            {
                is_macro=true;
                macro_count++;
            }
            source_body+=mname+' ';
        }
    }
    fputs(preprocessing.c_str(),fp);
    for(auto &v:mess_table)if(v.second[0]<'0'||v.second[0]>'9')fprintf(fp,"#define %s %s\n",v.second.c_str(),v.first.c_str());
    fprintf(fp,"%s\n",source_body.c_str());
    fclose(fp);
    printf("Convert finish in %s\nIdentifiers: %lu\nMacro functions: %d\n",outFile,mess_table.size(),macro_count);
    return 0;
}