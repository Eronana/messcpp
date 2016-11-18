#ifndef __MESSCPP_LEXER
#define __MESSCPP_LEXER

#include <cstdio>
#include <string>
#include "queue.hpp"

#define TOKENS(f) \
    f(ERROR), \
    f(EOF), \
    f(POUND), \
    f(INCREMENT), \
    f(ADD_ASSIGN), \
    f(ADD), \
    f(DECREMENT), \
    f(SUB_ASSIGN), \
    f(SUB), \
    f(MUL_ASSIGN), \
    f(MUL), \
    f(DIV_ASSIGN), \
    f(DIV), \
    f(MOD_ASSIGN), \
    f(MOD), \
    f(LEFT_PAREN), \
    f(RIGHT_PAREN), \
    f(LEFT_SQUARE_BARCKET), \
    f(RIGHT_SQUARE_BARCKET), \
    f(LEFT_BARCKET), \
    f(RIGHT_BARCKET), \
    f(LOGIC_OR), \
    f(BITWISE_OR_ASSIGN), \
    f(BITWISE_OR), \
    f(LOGIC_AND), \
    f(BITWISE_AND_ASSIGN), \
    f(BITWISE_AND), \
    f(BITWISE_NOT), \
    f(BITWISE_XOR_ASSIGN), \
    f(BITWISE_XOR), \
    f(INEQUAL), \
    f(NEGATION), \
    f(EQUAL), \
    f(ASSIGN), \
    f(RIGHT_SHIFT_ASSIGN), \
    f(RIGHT_SHIFT), \
    f(GREATER), \
    f(GREATER_EQ), \
    f(LEFT_SHIFT_ASSIGN), \
    f(LEFT_SHIFT), \
    f(LESS), \
    f(LESS_EQ), \
    f(SEMICOLON), \
    f(COMMA), \
    f(SCOPE), \
    f(DOT), \
    f(DOT_STAR), \
    f(ARROR), \
    f(ARROR_STAR), \
    f(COLON), \
    f(QUESTIONMARK), \
    f(CHAR), \
    f(STRING), \
    f(IDENTIFIER), \
    f(INTEGER), \
    f(HEX_INTEGER), \
    f(FLOAT), \
    f(INCLUDE_FILE), \
    f(DEFINE_NAME), \
    f(DEFINE_BODY)

#define GET_TOKEN_LIST(NAME) TOKEN_##NAME
#define GET_TOKEN_NAME(NAME) #NAME
const char CHAR_EOF=(char)EOF;
enum TOKEN_TYPE {TOKENS(GET_TOKEN_LIST)};
const char *TOKEN_NAME[]={TOKENS(GET_TOKEN_NAME)};

struct TOKEN
{
    TOKEN_TYPE token;
    int line,col;
    std::string raw;
    std::string msg;
    void print(FILE *f=stdout)
    {
        fprintf(f,"<TOKEN: %s, RAW: %s, LINE: %d, COL: %d",TOKEN_NAME[token],raw.c_str(),line,col);
        if(msg.length())fprintf(f,", MSG: %s",msg.c_str());
        puts(">");
    }
};
class Lexer
{
private:
    FILE *fp;
    Queue<char> lookhead_buffer;
    int line_number;
    int col_number;
    char lookHead(int index=0)
    {
        while(index>=lookhead_buffer.size())lookhead_buffer.push((char)fgetc(fp));
        return lookhead_buffer[index];
    }

    void clearChar(int len=1)
    {
        col_number+=len;
        lookhead_buffer.pop(len);
    }

    bool isIdentifierHead(char c)
    {
        return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c=='_');
    }
    bool isNumber(char c)
    {
        return c>='0'&&c<='9';
    }
    bool isHexNumber(char c)
    {
        return isNumber(c)||(c>='a'&&c<='f')||(c>='A'&&c<='F');
    }
    bool isIdentifier(char c)
    {
        return isIdentifierHead(c)||isNumber(c);
    }
    bool isWhiteSpace(char c)
    {
        for(const char *w=" \r\f\t\v";*w;w++)if(*w==c)return true;
        return false;
    }

    TOKEN createToken(TOKEN_TYPE token,int len=1)
    {
        TOKEN t;
        t.token=token;
        t.line=line_number;
        t.col=col_number;
        t.raw=std::string(lookhead_buffer.begin(),len);
        clearChar(len);
        return t;
    }
    TOKEN createToken(TOKEN_TYPE token,int len,std::string s)
    {
        TOKEN t=createToken(token,len);
        t.msg=s;
        return t;
    }
    void nextLine()
    {
        line_number++;
        col_number=0;
    }
    void skipWhiteSpace()
    {
        for(char c;~(c=lookHead());clearChar())
        {
            if(c=='\n')nextLine();
            else if(!isWhiteSpace(c))break;
        }
    }
    bool skipComment(TOKEN &t)
    {
        skipWhiteSpace();
        if(lookHead()!='/')return true;
        if(lookHead(1)=='/')
        {
            for(clearChar(2);lookHead()!=CHAR_EOF&&lookHead()!='\n';clearChar());
            if(lookHead()==CHAR_EOF)return true;
            nextLine();
            clearChar();
            return skipComment(t);
        }

        else if(lookHead(1)=='*')
        {
            for(clearChar(2);;clearChar())
            {
                switch(lookHead())
                {
                    case CHAR_EOF:
                        t=createToken(TOKEN_ERROR,0,"Unterminated comment");
                        return false;
                    case '\n':
                        nextLine();
                        break;
                    case '*':
                        if(lookHead(1)=='/')
                        {
                            clearChar(2);
                            return skipComment(t);
                        }
                }
            }
        }
        return true;
    }
    bool isPound;
    bool isInclude;
    int isDefine;

public:
    bool load(char *file)
    {
        fp=fopen(file,"r");
        if(!fp)return false;
        line_number=1;
        col_number=1;
        return true;
    }
    Lexer():isPound(false),isInclude(false),isDefine(0){}
    ~Lexer()
    {
        if(fp)fclose(fp);
    }

    TOKEN next()
    {
        if(!fp)return createToken(TOKEN_ERROR,0,"No source file.");
        TOKEN token_temp;
        if(!skipComment(token_temp))return token_temp;
        skipWhiteSpace();
        int c,i;
        if((c=lookHead())==CHAR_EOF)return createToken(TOKEN_EOF,0);
        if(isInclude)
        {
            isInclude=false;
            char cc=c=='<'?'>':c;
            for(i=1;lookHead(i)!=cc&&lookHead(i)!=CHAR_EOF;i++);
            if(lookHead(i)==CHAR_EOF)return createToken(TOKEN_INCREMENT,i,"EOF in #include");
            return createToken(TOKEN_INCLUDE_FILE,i+1);
        }
        else if(isDefine)
        {
            for(i=1;lookHead(i)!='\n'&&lookHead(i)!=' '&&lookHead(i)!=CHAR_EOF;i++);
            if(isDefine++==1)return createToken(TOKEN_DEFINE_NAME,i);
            else return isDefine=0,createToken(TOKEN_DEFINE_BODY,i);
        }
        switch(c)
        {
            case '+':
                switch(lookHead(1))
                {
                    case '+':
                        return createToken(TOKEN_INCREMENT,2);
                    case '=':
                        return createToken(TOKEN_ADD_ASSIGN,2);
                    default:
                        return createToken(TOKEN_ADD);
                }
            case '-':
                switch(lookHead(1))
                {
                    case '-':
                        return createToken(TOKEN_DECREMENT,2);
                    case '=':
                        return createToken(TOKEN_SUB_ASSIGN,2);
                    case '>':
                        if(lookHead(2)=='*')return createToken(TOKEN_ARROR_STAR,3);
                        else return createToken(TOKEN_ARROR,2);
                    default:
                        return createToken(TOKEN_SUB);
                }
            case '*':
                if(lookHead(1)=='=')return createToken(TOKEN_MUL_ASSIGN,2);
                return createToken(TOKEN_MUL);
            case '/':
                if(lookHead(1)=='=')return createToken(TOKEN_DIV_ASSIGN,2);
                return createToken(TOKEN_DIV);
            case '%':
                if(lookHead(1)=='=')return createToken(TOKEN_MOD_ASSIGN,2);
                return createToken(TOKEN_MOD);
            case '(':
                return createToken(TOKEN_LEFT_PAREN);
            case ')':
                return createToken(TOKEN_RIGHT_PAREN);
            case '[':
                return createToken(TOKEN_LEFT_SQUARE_BARCKET);
            case ']':
                return createToken(TOKEN_RIGHT_SQUARE_BARCKET);
            case '{':
                return createToken(TOKEN_LEFT_BARCKET);
            case '}':
                return createToken(TOKEN_RIGHT_BARCKET);
            case '#':
                isPound=true;
                return createToken(TOKEN_POUND);
            case '|':
                switch(lookHead(1))
                {
                    case '|':
                        return createToken(TOKEN_LOGIC_OR,2);
                    case '=':
                        return createToken(TOKEN_BITWISE_OR_ASSIGN,2);
                    default:
                        return createToken(TOKEN_BITWISE_OR);
                }
            case '&':
                switch(lookHead(1))
                {
                    case '&':
                        return createToken(TOKEN_LOGIC_AND,2);
                    case '=':
                        return createToken(TOKEN_BITWISE_AND_ASSIGN,2);
                    default:
                        return createToken(TOKEN_BITWISE_AND);
                }
            case '~':
                return createToken(TOKEN_BITWISE_NOT);
            case '^':
                if(lookHead(1)=='=')return createToken(TOKEN_BITWISE_XOR_ASSIGN,2);
                return createToken(TOKEN_BITWISE_XOR);
            case '!':
                if(lookHead(1)=='=')return createToken(TOKEN_INEQUAL,2);
                return createToken(TOKEN_NEGATION);
            case '=':
                if(lookHead(1)=='=')return createToken(TOKEN_EQUAL,2);
                return createToken(TOKEN_ASSIGN);
            case '>':
                if(lookHead(1)=='>')
                {
                    if(lookHead(2)=='=')return createToken(TOKEN_RIGHT_SHIFT_ASSIGN,3);
                    return createToken(TOKEN_RIGHT_SHIFT,2);
                }
                else if(lookHead(1)=='=')return createToken(TOKEN_GREATER_EQ,2);
                return createToken(TOKEN_GREATER);
            case '<':
                if(lookHead(1)=='<')
                {
                    if(lookHead(2)=='=')return createToken(TOKEN_LEFT_SHIFT_ASSIGN,3);
                    return createToken(TOKEN_LEFT_SHIFT,2);
                }
                else if(lookHead(1)=='=')return createToken(TOKEN_LESS_EQ,2);
                return createToken(TOKEN_LESS);
            case ';':
                return createToken(TOKEN_SEMICOLON);
            case ',':
                return createToken(TOKEN_COMMA);
            case ':':
                if(lookHead(1)==':')return createToken(TOKEN_SCOPE,2);
                return createToken(TOKEN_COLON);
            case '?':
                return createToken(TOKEN_QUESTIONMARK);
            case '\'':
            case '"':
                for(i=1;(lookHead(i)!=c||lookHead(i-1)=='\\')&&lookHead(i)!='\n'&&lookHead(i)!=CHAR_EOF;i++);
                if(lookHead(i)=='\n')
                {
                    if(c=='\'')return createToken(TOKEN_ERROR,i,"Unterminated char literal in new line");
                    else return createToken(TOKEN_ERROR,i,"Unterminated string literal in new line");
                }
                if(lookHead(i)==CHAR_EOF)
                {
                    if(c=='\'')return createToken(TOKEN_ERROR,i,"Unterminated char literal in EOF");
                    else return createToken(TOKEN_ERROR,i,"Unterminated string literal in EOF");
                }
                if(c=='\'')return createToken(TOKEN_CHAR,i+1);
                else return createToken(TOKEN_STRING,i+1);
            default:
                if(c=='.')
                {
                    if(lookHead(1)=='*')return createToken(TOKEN_DOT_STAR,2);
                    if(!isNumber(lookHead(1)))return createToken(TOKEN_DOT);
                }
                // a_inditifer123
                if(isIdentifierHead(c))
                {
                    for(i=1;isIdentifierHead(lookHead(i));i++);
                    token_temp=createToken(TOKEN_IDENTIFIER,i);
                    if(isPound)
                    {
                        isPound=false;
                        if(token_temp.raw=="include")isInclude=true;
                        else if(token_temp.raw=="define")isDefine=1;
                    }
                    return token_temp;
                }
                //0x12345678
                if(c=='0'&&((lookHead(1)=='x')||lookHead(1)=='X'))
                {
                    for(i=2;isHexNumber(lookHead(i));i++);
                    return createToken(TOKEN_HEX_INTEGER,i);
                }
                //123456
                //.12346
                //1.234
                //123465f
                if(isNumber(c)||c=='.')
                {
                    for(i=0;isNumber(lookHead(i));i++);
                    if(lookHead(i)!='.')
                    {
                        if(lookHead(i)=='F'||lookHead(i)=='f')return createToken(TOKEN_FLOAT,i+1);
                        return createToken(TOKEN_INTEGER,i);
                    }
                    for(i++;isNumber(lookHead(i));i++);
                    if(lookHead(i)=='F'||lookHead(i)=='f')i++;
                    return createToken(TOKEN_FLOAT,i);
                }
                return createToken(TOKEN_ERROR,1,"Unexcepted symbol");
        }
    }
};

#endif