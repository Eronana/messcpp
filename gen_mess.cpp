#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

void write(const vector<string> &ss)
{
    FILE *fp=fopen("mess_list.txt","w");
    for(auto &s:ss)fprintf(fp,"%s\n",s.c_str());
    fclose(fp);
}

void generate(vector<string> &s,int n)
{
    char buffer[4]="___";
    for(char i='a';i<='z';i++)
    {
        buffer[0]=i;
        for(char j='a';j<='z';j++)
        {
            buffer[2]=j;
            s.push_back(buffer);
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
    char *file=argv[1];
    vector<string> s;
    generate(s,atoi(file));
    sort(s.begin(),s.end());
    s.erase(unique(s.begin(),s.end()),s.end());
    write(s);
    printf("%ld mess names have been generated in mess_list.txt\n",s.size());
    return 0;
}