#include "lexer.hpp"

int main(int argc,char *argv[])
{
    if(argc!=2)return printf("Usage:\n\t%s test.l\n",argv[0]),1;
    Lexer lexer;
    if(!lexer.load(argv[1]))
    {
        puts("load failed");
        return 1;
    }
    for(TOKEN t;(t=lexer.next()).token!=TOKEN_EOF;t.print());
    return 0;
}
