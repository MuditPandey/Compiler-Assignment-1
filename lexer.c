#include<stdio.h>
#include<string.h>

void parser(char *);
char* lexer(FILE *);
char* ret_token(int);
char buffer[100];
int state=0;
int main()
{
    char file[]="test.txt";
    parser(file);
    return 0;
}
char* ret_token(int state_val)
{
    if(state_val==12)
        return "TK_FLOAT";
    else if(state==10)
        return "TK_INTEGER";
    else
        return "ERROR!";
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
        printf("IN PARSE LOOP\n");
        char* string=lexer(fp);
        //printf("%s",string);
        printf("string %s is identified as %s\n",buffer,string);
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
        printf("%c %d",ch,buf_index);
        buffer[buf_index++]=ch;
        switch(state)
        {
            case 0:
                printf("In state 0\n");
                if(ch>='0' && ch<='9')
                    state=10;
                else if(ch==' ')
                    buf_index--;
                else
                    state=999;
            break;
            case 10:
                 printf("In state 10\n");
                if(ch=='.')
                    state=11;
                else if(!(ch>='1' && ch<='9'))
                {
                   buffer[buf_index-1]=0;
                   fseek(fp,-1,SEEK_CUR);
                   //printf("buffer is %s",buffer);
                   return ret_token(state);
                }
            break;
            case 11:
                 printf("In state 11\n");
                if(ch>='0' && ch<='9')
                    state=12;
                else
                    state=999;

            break;
            case 12:
                 printf("In state 12\n");
               if(!(ch>='0' && ch<='9'))
               {
                   buffer[buf_index-1]=0;
                   fseek(fp,-1,SEEK_CUR);
                   //printf("buffer is %s",buffer);
                   return ret_token(state);
               }

            break;

        }
        if(state==999)
        {
            printf("Error State\n");
            buffer[buf_index-1]=0;
            fseek(fp,-1,SEEK_CUR);
            //printf("buffer is %s",buffer);
            return buffer;
        }

    }
    if(ch==EOF)
        {
            char* string=ret_token(state);
            state=1000;
            return string;
        }

}
