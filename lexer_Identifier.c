#include<stdio.h>

FILE *fp;
FILE *temp;
char buffer[100];
char in;
char delimiter[]={'[','{','(',')',']','}',',',':',';'};
int size_del = sizeof(delimiter)/sizeof(delimiter[0]);

int checkdel(int x){
    int i;
    for(i=0;i<size_del;i++){
        if(x==delimiter[i]) return 1;
    }
    return -1;
}

int alpha(int x){
    return (x>='a'&&x<='z')||(x>='A'&&x<='Z');
}

int digit(int x){
    return x>='0'&&x<='9';
}

char * lexer(FILE* fp){
    int c=0;
    in = fgetc(fp);
    if(alpha(in)||in=='_'){
        buffer[c++]=in;
        while(1){
            in = fgetc(fp);
            if(digit(in)||alpha(in)||in=='_'){
                buffer[c++]=in;
            }
            else {
                fseek(fp,-1,SEEK_CUR);
                break;
            }
        }
    }
    else if(checkdel(in)){
        buffer[c++]=in;
    }
    buffer[c]=0;
    return buffer;
}

int main(){
    fp = fopen("input.txt","r");
    if(fp==NULL){
        printf("FILE OPEN ERROR");
        return -1;
    }
    return 0;
}
