#include <set>
#include <string>
#include <cstdlib>
using namespace std;

void write(const set<string> &ss)
{
    freopen("mess_name.h","w",stdout);
    puts("const char *mess_name[]={");
    for(auto &s:ss)printf("    \"%s\",\n",s.c_str());
    puts("};");
}
void generate(set<string> &s,int n)
{
    char buffer[4]="___";
    for(char i='a';i<='z';i++)
    {
        buffer[0]=i;
        for(char j='a';j<='z';j++)
        {
            buffer[2]=j;
            s.insert(buffer);
            if(!--n)return;
        }
    }
}
int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("Usage:\n%s number\n",argv[0]);
        return 1;
    }
    int n=atoi(argv[1]);
    set<string> s;
    generate(s,n);
    write(s);
    return 0;
}