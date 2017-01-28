/**********************************************************************************************************************************
             Symbol          State
             ------         -------
1.             +               1
2.             -               2
3.             *               3
4.             /               4
5.             %               5
6.             =               6
7.             >               7
8.             <               8
9.             !               9
10.            &               10
11.            |               11
12.            ^               12
13.            ~               13
14.            >>              14
15.            <<              15
16.            >>>             16
17.            +=              17
18.            -=              18
19.            *=              19
20.            /=              20
21.            %=              21
22.            ==              22
23.            >=              23
24.            <=              24
25.            !=              25
26.            >>=             26
27.            <<=             27
28.            &=              28
29.            |=              29
30.            ^=              30
31.            identifier      31
32.            integer         32
33.            .               33    [not a final state]
34.            float           34
35.            "               35    [not a final state]
36.            string literal  36    [not a final state]
37.            "(ending)       37
38.            ;               38
39.            {               39
40.            }               40
41.            (               41
42.            )               42
43.            :               43
44.            ,               44
45.       Comments_typ1(/*)    45     [not a final state]
46.       Comments_typ1(/*..*) 46     [not a final state]
47.       Comments_typ2(//)    47     [not a final state]
48.       CommentEnd           48     [common end state for both comments]
******************************************************************************************************************************/
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>
#define NO_KEYWORDS 7 //39

void parser(char *);
char* lexer(FILE *);
char* ret_token(int);
bool isKeyword();
char buffer[100];
int line=1;
int state=0;
//ADD MORE KEYWORDS
/*char *keyword[]={"abstract","case","catch","class","def","do","else","extends","false","final","finally","for","forSome",
				"if","implicit","import","lazy","match","new","Null","object","override","package","private","protected",
				"return","scaled","super","this","throw","trait","Try","true","type","val","Var","while","with","yield",
				/*":","=>","<-","<:","<%",">:","#","@"*///};
char *keyword[]={"int","else","if","return","void","while","main"};
int main(int argc,char* argv[])
{
    char file[]="test.txt";
    parser(file);
    return 0;
}
bool isKeyword()
{
    int i;
    for(i=0;i<NO_KEYWORDS;i++)
    {
        if(!strcmp(buffer,keyword[i]))
            return true;
    }
    return false;
}
char* ret_token(int state_val)
{
   // printf("%d\n",state_val);
    if(state_val>=1 && state_val<=5)
        return "TK_ARITHMETIC_OPERATOR";
    else if(state_val==6 || (state_val>=17 && state_val<=21) || (state_val>=26 && state_val<=30))
        return "TK_ASSIGNMENT_OPERATOR";
    else if(state_val==7 || state_val==8 || (state_val>=22 && state_val<=25))
        return "TK_RELATIONAL_OPERATOR";
    else if(state_val>=9 && state_val<=16)
        return "TK_BITWISE_OPERATOR";
    else if(state_val==31)
        {
            if(!isKeyword())
                return "TK_IDENTIFIER";
            else
                return "TK_KEYWORD";
        }
    else if(state_val==32)
        return "TK_INTEGER";
    else if(state_val==34)
        return "TK_FLOAT";
    else if(state_val==37)
        return "TK_STRING";
    else if(state_val>=38 && state_val<=44)
        return "TK_DELIMITER";
    else if(state_val==48)
        return "COMMENTS_IGNORE";
    else
        return "BAD CHARACTER or INCORRECT USAGE";
}
void parser(char * file)
{
    FILE *fp=fopen("test.txt","r");
    if(fp==NULL)
    {
        printf("Error opening file!");
        return;
    }
    while(state!=1000)
    {
        //printf("IN PARSE LOOP\n");
        char* string=lexer(fp);
        //printf("%s",string);
        printf("string %s is identified as %s at line %d\n",buffer,string,line);
    }
    close(fp);
}

char* lexer(FILE* fp)
{
    char ch;

    int buf_index=0;
    state=0;
    while((ch=fgetc(fp))!=EOF)
    {
        //printf("%c %d",ch,buf_index);
        buffer[buf_index++]=ch;
        switch(state)
        {
            case 0:
                //printf("In state 0\n");
                if(ch=='+')
					state=1;
				else if(ch=='-')
					state=2;
				else if(ch=='*')
					state=3;
				else if(ch=='/')
					state=4;
				else if(ch=='%')
					state=5;
				else if(ch=='=')
					    state=6;
				else if(ch=='>')
					state=7;
				else if(ch=='<')
					state=8;
				else if(ch=='!')
					state=9;
				else if(ch=='&')
					state=10;
				else if(ch=='|')
					state=11;
				else if(ch=='^')
					state=12;
				else if(ch=='~')
					state=13;
                else if(isdigit(ch))
                    state=32;
                else if(isalpha(ch) || ch=='_')
                    state=31;
                else if(ch=='"')
                    state=35;
                else if(ch==' ')
                    buf_index--;
                else if(ch==';')
                    state=38;
                else if(ch=='{')
                    state=39;
                else if(ch=='}')
                    state=40;
                else if(ch=='(')
                    state=41;
                else if(ch==')')
                    state=42;
                else if(ch==':')
                    state=43;
                else if(ch==',')
                    state=44;
                else if(ch=='\n')
                {
                    line++;
                    buf_index--;
                }
                else
                    state=999;
            break;
            case 1:
                    //printf("In state 1\n");
                    if(ch=='=')
                        state=17;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 2:
                    //printf("In state 2\n");
                    if(ch=='=')
                        state=18;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 3:
                    //printf("In state 3\n");
                    if(ch=='=')
                        state=19;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 4:
                    //printf("In state 4\n");
                    if(ch=='=')
                        state=20;
                    else if(ch=='*')
                        state=45;
                    else if(ch=='/')
                        state=47;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 5:
                    //printf("In state 5\n");
                    if(ch=='=')
                        state=21;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 6:
                    //printf("In state 6\n");
                    if(ch=='=')
                       state=22;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 7:
                    //printf("In state 7\n");
                    if(ch=='>')
                        state=14;
                    else if(ch=='=')
                        state=23;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 8:
                    //printf("In state 8\n");
                    if(ch=='<')
                        state=15;
                    else if(ch=='=')
                        state=24;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 9://printf("In state 9\n");
                    if(ch=='=')
                        state=25;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 10://printf("In state 10\n");
                    if(ch=='=')
                        state=28;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 11://printf("In state 11\n");
                    if(ch=='=')
                        state=29;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 12://printf("In state 12\n");
                    if(ch=='=')
                        state=30;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 13://printf("In state 13\n");
                    buffer[buf_index-1]=0;
                    fseek(fp,-1,SEEK_CUR);
                    return ret_token(state);
            break;
            case 14://printf("In state 14\n");
                    if(ch=='>')
                        state=16;
                    else if(ch=='=')
                        state=26;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 15:
                    //printf("In state 15\n");
                    if(ch=='=')
                        state=27;
                    else
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 16://printf("In state 16-30\n");
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
                    buffer[buf_index-1]=0;
                    //printf("%s\n",buffer);
                    fseek(fp,-1,SEEK_CUR);
                    return ret_token(state);
            break;
            case 31:
                    //printf("In state 32\n");
                    if(!isalpha(ch)&& !isdigit(ch)&&ch!='_')
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        return ret_token(state);
                    }
            break;
            case 32:
                    //printf("In state 32\n");
                    if(ch=='.')
                        state=33;
                    else if(!(ch>='1' && ch<='9'))
                    {
                       buffer[buf_index-1]=0;
                       fseek(fp,-1,SEEK_CUR);
                       //printf("buffer is %s",buffer);
                       return ret_token(state);
                    }
            break;
            case 33:
                    //printf("In state 33\n");
                    if(isdigit(ch))
                        state=34;
                    else
                        state=999;
            break;
            case 34:
                    //printf("In state 34\n");
                    if(!isdigit(ch))
                   {
                       buffer[buf_index-1]=0;
                       fseek(fp,-1,SEEK_CUR);
                       //printf("buffer is %s",buffer);
                       return ret_token(state);
                   }
            break;
            case 35:
                    if(ch=='"')
                        state=37;
                    else if(ch!='\n')
                        state=36;
            break;
            case 36:
                    if(ch=='\n')
                    {
                        buffer[buf_index-1]=0;
                        fseek(fp,-1,SEEK_CUR);
                        //printf("buffer is %s",buffer);
                        return ret_token(state);
                    }
                    else if(ch=='"')
                        state=37;
            break;
            case 37:
                    buffer[buf_index-1]=0;
                    fseek(fp,-1,SEEK_CUR);
                    //printf("buffer is %s",buffer);
                    return ret_token(state);
            break;
            case 38:
            case 39:
            case 40:
            case 41:
            case 42:
            case 43:
            case 44:
                    buffer[buf_index-1]=0;
                    fseek(fp,-1,SEEK_CUR);
                    //printf("buffer is %s",buffer);
                    return ret_token(state);
            break;
            case 45:
                    if(ch=='*')
                        state=46;
                    if(ch=='\n')
                        line++;
            break;
            case 46:
                    if(ch=='/')
                    {
                        buffer[buf_index]=0;
                        state=48;
                        //return comment or reset buffer here.
                    }
                    else if(ch!='*')
                        state=45;
                    if(ch=='\n')
                        line++;
            break;
            case 47:
                    if(ch=='\n')
                    {
                        buffer[buf_index-1]=0;
                        line++;
                        state=48;
                        //return single line comment heree.
                    }
            case 48:
                    buf_index=0;
                    return ret_token(state); 
            break;

        }
        if(state==999)
        {
            buffer[buf_index-1]=0;
            return ret_token(state);
        }

    }
    if(ch==EOF)
        {
            buffer[buf_index]=0;
            char* string=ret_token(state);
            state=1000;
            return string;
        }
    return "SilenceWarning";

}