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
48.       CommentEnd           48     [end state for both comments]
49.            .               49
50.            [               50
51.            ]               51
52.            &&              52
53.            ||              53
/******************************************************************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define true 1
#define false 0
#define N 58
#define NO_KEYWORDS 45 //45
#define db printf("db")

void parser(char *);
char* lexer(FILE *);
char* ret_token(int);
char* to_lower(char *);
int isKeyword();
char buffer[100];
int line=1;
int state=0;
//ADD MORE KEYWORDS
char *keyword[]={"abstract","case","catch","class","def","do","else","extends","false","final","finally","for","forSome",
				"if","implicit","import","lazy","match","new","Null","object","override","package","private","protected",
				"return","scaled","super","this","throw","trait","Try","true","type","val","Var","while","with","yield",
				"Int","String","args","Array","main","var"};
//char *keyword[]={"int","else","if","return","void","while","main"}; 7
//Parser Structures
typedef struct first_follow
{
    int first;
    int follow;
    int first_set[20];
    int follow_set[20];
}first_follow;
typedef struct cell{
    int x;
    int shift;
}cell;
typedef struct node{
	struct node* parent;
	struct node* children[30];
	char val[20];
	int n_child;

}node;
typedef struct grammar_rule{
    char LHS[20];
    char RHS[100];
}rule;
//Parser Globals
first_follow fstfllw[100];
cell array[102][100];
char *g[]={"object","tk_identifier","{","def","main","(","args",":","Array","[","String","]",")","}","||","&&","!","true","false",">",">=","<","<=","!=","==","+","-","*","/","=","tk_integer","tk_string","tk_float","var",",","if","else","while","$","PROGRAM","STMTS","B","C","D","REL","E","T","F","ASSIGNMENT","VAL","DECLARATIONS","IDLIST","VALLIST","IFELSE","ELSE","WHILE","","$"};
rule grammar[50];
int N_RULES=0;
node* stack[1000];
int int_stack[10000];
int node_top=-1;
int int_tos=-1;
//Parser Related functions
int f(char *str);
void func(int a,int b,int c,int d);
node* create_node(char *str);
void int_pop();
void int_push(int x);
node* node_pop();
void node_push(node *push);
void print_nodestack();
int pop_count(int index);
void get_LHS(int i,char *ret);
void init();
void load_grammar_rules();
void error_handler(int state_no, char * string, FILE * fp);
void print_parse_tree(node* root);
void set_firstandfollow();
int find_follow(char *, char *);


int main(int argc,char* argv[])
{
    char file[]="parser_test1.txt";
    //printf("asgdadgvasdgvajhsd\n");
    set_firstandfollow();
    parser(file);
    return 0;
}
int isKeyword()
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
    if(state_val>=1 && state_val<=5)
        return "TK_ARITHMETIC_OPERATOR";
    else if(state_val==6 || (state_val>=17 && state_val<=21) || (state_val>=26 && state_val<=30))
        return "TK_ASSIGNMENT_OPERATOR";
    else if(state_val==7 || state_val==8 || (state_val>=22 && state_val<=25))
        return "TK_RELATIONAL_OPERATOR";
    else if(state_val>=9 && state_val<=16)
        return "TK_BITWISE_OPERATOR";
    else if(state==31)
        {
            if(isKeyword()==0)
                return "tk_identifier";
            else
                return "TK_KEYWORD";
        }
    else if(state_val==32)
        return "tk_integer";
    else if(state_val==34)
        return "tk_float";
    else if(state_val==37)
        return "tk_string";
    else if((state_val>=38 && state_val<=44) || (state_val>=49 && state_val<=51))
        return "TK_DELIMITER";
    else if(state_val>=52 && state_val<=53)
        return "TK_LOGICAL_OPERATOR";
    else
        return "BAD_CHARACTER";
}
void parser(char * file)
{
    init();load_grammar_rules();
    node *ROOT;
    FILE *fp=fopen(file,"r");
    if(fp==NULL)
    {
        printf("Error opening file!");
        return;
    }
    char s[100];
    char string[100];
    strcpy(string,lexer(fp));
    //printf("%s",string);
    if(!strcmp(string,"END OF FILE"))
        strcpy(s,"$");
    else if(!strcmp(string,"tk_identifier")||!strcmp(string,"tk_float")||!strcmp(string,"tk_string")||!strcmp(string,"tk_integer"))
        strcpy(s,string);
    else
        strcpy(s,buffer);
    printf("string %s is identified as %s at line %d\n",buffer,string,line);
    //scanf("%s",s);
    int_push(0);
    while(1)
    {
        int idx = int_stack[int_tos];
        if(f(s)==-1||array[idx][f(s)].shift==-1)
        {
            error_handler(idx,s,fp);
        }
        if( array[idx][f(s)].shift==false )
        {
            printf("Reduce ");
            int c = pop_count(array[idx][f(s)].x);
            char push_s[100];
            printf("%d %s\n",idx,s);
            get_LHS(array[idx][f(s)].x,push_s);
            int i;
            node *temp=create_node(push_s);
            for(i=0;i<c;i++)
            {
                node *temp2=node_pop();
                temp->children[c-i-1]=temp2;
                temp->n_child++;
                temp2->parent=temp;
                int_pop();
            }
            int idx_mudit = int_stack[int_tos];
            node_push(temp);
            print_nodestack();
            int_push(array[idx_mudit][f(push_s)].x);
        }
        else if(array[idx][f(s)].shift==true)
        {
            printf("Shift ");
            node *temp3=create_node(s);
            node_push(temp3);
            print_nodestack();
            int_push(array[idx][f(s)].x);
            if(int_stack[int_tos]==-1)
            {
                printf("ACC ");
                ROOT=create_node("PROGRAM");
                int i,p=0;
                for(i=0;i<node_top;i++)
                {
                    ROOT->children[p++]=stack[i];
                    ROOT->n_child++;
                    stack[i]->parent=ROOT;
                }
                break;
            }
            char* string=lexer(fp);
            //printf("%s",string);
            if(!strcmp(string,"END OF FILE"))
                strcpy(s,"$");
            else if(!strcmp(string,"tk_identifier")||!strcmp(string,"tk_float")||!strcmp(string,"tk_string")||!strcmp(string,"tk_integer"))
                strcpy(s,string);
            else
                strcpy(s,buffer);
            printf("string %s is identified as %s at line %d\n",buffer,string,line);
        }
    }
    printf("\nRoot=%s no. of child=%d.\n",ROOT->val,ROOT->n_child);
    print_parse_tree(ROOT);
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
                else if(ch==' ' || ch=='\t')
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
                else if(ch=='.')
                    state=49;
                else if(ch=='[')
                    state=50;
                else if(ch==']')
                    state=51;
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
                    else if(ch=='&')
                        state=52;
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
                    else if(ch=='|')
                        state=53;
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
                    //printf("In state 31\n");
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
                        state=48;
                        //return comment or reset buffer here.
                    else if(ch!='*')
                        state=45;
                    else if(ch=='\n')
                        line++;
            break;
            case 47:
                    //printf("in state 47");
                    if(ch=='\n')
                    {
                        line++;
                        state=48;
                    }
            break;
            case 48:
                    buf_index=0;
                    fseek(fp,-1,SEEK_CUR);
                    state=0;
                    //return ret_token(state);
            break;
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
                    buffer[buf_index-1]=0;
                    fseek(fp,-1,SEEK_CUR);
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
            if(buf_index==0)
            {
                state=1000;
                return "END OF FILE";
            }
            buffer[buf_index]=0;
            //printf("%s",buffer);
            char* string=ret_token(state);
            state=1000;
            return string;
        }

}

int find_follow(char * nt, char * s){
	int i;
	//return 0;
	//printf("%s %s",nt,s);
	for(i=0;i<fstfllw[f(nt)].follow;i++){
		if(fstfllw[f(nt)].follow_set[i]==f(s)){
			return 0;
		}
	}
	return 1;
}

void error_handler(int state_no, char * sinp,FILE *fp)
{
    node * temp= stack[node_top];
	printf("Error found at line number %d on input string \" %s \" ^ \" %s \" \n",line,temp->val,sinp);
	node_pop();
	//db;
	int_pop();
	node * n = stack[node_top];
	//printf("%s",n->val);
	while(f(n->val)<f("PROGRAM")){
		node_pop();
		int_pop();
		n = stack[node_top];
		db;
	}
	//printf("%s",sinp);
	while( find_follow(n->val,sinp)){
	char string[100];
	//db;
	strcpy(string,lexer(fp));
    	//printf("%s",string);
    	if(!strcmp(string,"END OF FILE"))
        	strcpy(sinp,"$");
    	else if(!strcmp(string,"tk_identifier")||!strcmp(string,"tk_float")||!strcmp(string,"tk_string")||!strcmp(string,"tk_integer"))
       		strcpy(sinp,string);
    	else
        	strcpy(sinp,buffer);
        if(!strcmp(sinp,"$"))
        {
            printf("Compiled");
            exit(0);

        }

	}


	/*if(!strcmp("$",string)){
	printf("EOF file reached while parsing\n");
	exit(0);
	}
	if(state_no==25)
	{
		printf(" Parsing complete Redundant %s\n.",string);
		exit(0);
	}
    printf("\nFound %s, needed",string);
    int i;
    for( i=0;i<100;i++) if(array[state_no][i].x!=-1)	printf(" %s",g[i]);
    printf("\n");
    scanf("%s",string);
    while(array[state_no][f(string)].x==-1){
	if(!strcmp("$",string)){
		printf("EOF file reached while parsing")	;
		exit(0);
	}
	scanf("%s",string);
    }*/
}

void print_parse_tree(node* root)
{
    if(root==NULL)
        return;
    printf("%s ",root->val);
    int i;
    for(i=0;i<root->n_child;i++)
    {
        print_parse_tree(root->children[i]);
    }
}

void init()
{
    int i,j;
    for(i=0;i<102;i++) for(j=0;j<100;j++) array[i][j].x=array[i][j].shift=-1;

    func( 0 ,f( "object" ), 1 ,  true );
    func( 1 ,f( "tk_identifier" ), 2 ,  true );
    func( 2 ,f( "{" ),3 ,  true );
    func( 3 ,f( "def" ), 4 ,  true );
    func( 4 ,f( "main" ), 5 ,  true );
    func( 5 ,f( "(" ),6 ,  true );
    func( 6 ,f( "args" ), 7 ,  true );
    func( 7 ,f( ":" ), 8 ,  true );
    func( 8 ,f( "Array" ), 9 ,  true );
    func( 9 ,f( "[" ), 10 ,  true );
    func( 10 ,f( "String" ), 11 ,  true );
    func( 11 ,f( "]" ), 12 ,  true );
    func( 12 ,f( ")" ), 13 ,  true );
    func( 13 ,f( "{" ), 14 ,  true );
    func( 14 ,f( "tk_identifier" ), 5 ,  false );
    func( 14 ,f( "}" ), 5 ,  false );
    func( 14 ,f( "var" ), 5 ,  false );
    func( 14 ,f( "if" ), 5 ,  false );
    func( 14 ,f( "while" ), 5 ,  false );
    func( 14 ,f( "STMTS" ), 15 ,  true );
    func( 15 ,f( "tk_identifier" ), 21 ,  true );
    func( 15 ,f( "}" ), 16 ,  true );
    func( 15 ,f( "var" ), 22 ,  true );
    func( 15 ,f( "if" ), 24 ,  true );
    func( 15 ,f( "while" ), 23 ,  true );
    func( 15 ,f( "ASSIGNMENT" ), 17 ,  true );
    func( 15 ,f( "DECLARATIONS" ), 18 ,  true );
    func( 15 ,f( "IFELSE" ), 20 ,  true );
    func( 15 ,f( "WHILE" ), 19 ,  true );
    func( 16 ,f( "}" ), 25 ,  true );
    func( 17 ,f( "tk_identifier" ), 1 ,  false );
    func( 17 ,f( "}" ), 1 ,  false );
    func( 17 ,f( "var" ), 1 ,  false );
    func( 17 ,f( "if" ), 1 ,  false );
    func( 17 ,f( "while" ), 1 ,  false );
    func( 18 ,f( "tk_identifier" ), 2 ,  false );
    func( 18 ,f( "}" ), 2 ,  false );
    func( 18 ,f( "var" ), 2 ,  false );
    func( 18 ,f( "if" ), 2 ,  false );
    func( 18 ,f( "while" ), 2 ,  false );
    func( 19 ,f( "tk_identifier" ), 3 ,  false );
    func( 19 ,f( "}" ), 3 ,  false );
    func( 19 ,f( "var" ), 3 ,  false );
    func( 19 ,f( "if" ), 3 ,  false );
    func( 19 ,f( "while" ), 3 ,  false );
    func( 20 ,f( "tk_identifier" ), 4 ,  false );
    func( 20 ,f( "}" ), 4 ,  false );
    func( 20 ,f( "var" ), 4 ,  false );
    func( 20 ,f( "if" ), 4 ,  false );
    func( 20 ,f( "while" ), 4 ,  false );
    func( 21 ,f( "=" ), 26 ,  true );
    func( 22 ,f( "(" ), 28 ,  true );
    func( 22 ,f( "tk_identifier" ), 27 ,  true );
    func( 23 ,f( "(" ), 29 ,  true );
    func( 24 ,f( "(" ), 30 ,  true );
    func( 25 ,f( "$" ), -1 ,  true );
    func( 26 ,f( "args" ), 36 ,  true );
    func( 26 ,f( "tk_integer" ), 37 ,  true );
    func( 26 ,f( "tk_string" ), 38 ,  true );
    func( 26 ,f( "tk_float" ), 39 ,  true );
    func( 26 ,f( "E" ), 31 ,  true );
    func( 26 ,f( "T" ), 32 ,  true );
    func( 26 ,f( "F" ), 33 ,  true );
    func( 26 ,f( "VAL" ), 35 ,  true );
    func( 27 ,f( "=" ), 40 ,  true );
    func( 28 ,f( "tk_identifier" ), 42 ,  true );
    func( 28 ,f( "IDLIST" ), 41 ,  true );
    func( 29 ,f( "tk_identifier" ), 34 ,  true );
    func( 29 ,f( "(" ), 47 ,  true );
    func( 29 ,f( "!" ), 46 ,  true );
    func( 29 ,f( "true" ), 49 ,  true );
    func( 29 ,f( "false" ), 50 ,  true );
    func( 29 ,f( "tk_integer" ), 37 ,  true );
    func( 29 ,f( "tk_string" ), 38 ,  true );
    func( 29 ,f( "tk_float" ), 39 ,  true );
    func( 29 ,f( "B" ), 43 ,  true );
    func( 29 ,f( "C" ), 44 ,  true );
    func( 29 ,f( "D" ), 45 ,  true );
    func( 29 ,f( "E" ), 48 ,  true );
    func( 29 ,f( "T" ), 32 ,  true );
    func( 29 ,f( "F" ), 33 ,  true );
    func( 29 ,f( "VAL" ), 35 ,  true );
    func( 30 ,f( "tk_identifier" ), 34 ,  true );
    func( 30 ,f( "(" ), 47 ,  true );
    func( 30 ,f( "!" ), 46 ,  true );
    func( 30 ,f( "true" ), 49 ,  true );
    func( 30 ,f( "false" ), 50 ,  true );
    func( 30 ,f( "tk_integer" ), 37 ,  true );
    func( 30 ,f( "tk_string" ), 38 ,  true );
    func( 30 ,f( "tk_float" ), 39 ,  true );
    func( 30 ,f( "B" ), 51 ,  true );
    func( 30 ,f( "C" ), 44 ,  true );
    func( 30 ,f( "D" ), 45 ,  true );
    func( 30 ,f( "E" ), 48 ,  true );
    func( 30 ,f( "T" ), 32 ,  true );
    func( 30 ,f( "F" ), 33 ,  true );
    func( 30 ,f( "VAL" ), 35 ,  true );
    func( 31 ,f( "tk_identifier" ), 30 ,  false );
    func( 31 ,f( "}" ), 30 ,  false );
    func( 31 ,f( "+" ), 52 ,  true );
    func( 31 ,f( "-" ), 53 ,  true );
    func( 31 ,f( "var" ), 30 ,  false );
    func( 31 ,f( "if" ), 30 ,  false );
    func( 31 ,f( "while" ), 30 ,  false );
    func( 32 ,f( "tk_identifier" ), 23 ,  false );
    func( 32 ,f( ")" ), 23 ,  false );
    func( 32 ,f( "}" ), 23 ,  false );
    func( 32 ,f( "||" ), 23 ,  false );
    func( 32 ,f( "&&" ), 23 ,  false );
    func( 32 ,f( "!" ), 23 ,  false );
    func( 32 ,f( ">" ), 23 ,  false );
    func( 32 ,f( ">=" ), 23 ,  false );
    func( 32 ,f( "<" ), 23 ,  false );
    func( 32 ,f( "<=" ), 23 ,  false );
    func( 32 ,f( "!=" ), 23 ,  false );
    func( 32 ,f( "==" ), 23 ,  false );
    func( 32 ,f( "+" ), 23 ,  false );
    func( 32 ,f( "-" ), 23 ,  false );
    func( 32 ,f( "*" ), 54 ,  true );
    func( 32 ,f( "/" ), 55 ,  true );
    func( 32 ,f( "var" ), 23 ,  false );
    func( 32 ,f( "," ), 23, false );
    func( 32 ,f( "if" ), 23 ,  false );
    func( 32 ,f( "while" ), 23 ,  false );
    func( 33 ,f( "tk_identifier" ), 26 ,  false );
    func( 33 ,f( ")" ), 26 ,  false );
    func( 33 ,f( "}" ), 26 ,  false );
    func( 33 ,f( "||" ), 26 ,  false );
    func( 33 ,f( "&&" ), 26 ,  false );
    func( 33 ,f( "!" ), 26 ,  false );
    func( 33 ,f( ">" ), 26 ,  false );
    func( 33 ,f( ">=" ), 26 ,  false );
    func( 33 ,f( "<" ), 26 ,  false );
    func( 33 ,f( "<=" ), 26 ,  false );
    func( 33 ,f( "!=" ), 26 ,  false );
    func( 33 ,f( "==" ), 26 ,  false );
    func( 33 ,f( "+" ), 26 ,  false );
    func( 33 ,f( "-" ), 26 ,  false );
    func( 33 ,f( "*" ), 26 ,  false );
    func( 33 ,f( "/" ), 26 ,  false );
    func( 33 ,f( "var" ), 26 ,  false );
    func( 33 ,f( "," ), 26, false );
    func( 33 ,f( "if" ), 26 ,  false );
    func( 33 ,f( "while" ), 26 ,  false );
    func( 34 ,f( "tk_identifier" ), 27 ,  false );
    func( 34 ,f( ")" ), 27 ,  false );
    func( 34 ,f( "}" ), 27 ,  false );
    func( 34 ,f( "||" ), 27 ,  false );
    func( 34 ,f( "&&" ), 27 ,  false );
    func( 34 ,f( "!" ), 27 ,  false );
    func( 34 ,f( ">" ), 27 ,  false );
    func( 34 ,f( ">=" ), 27 ,  false );
    func( 34 ,f( "<" ), 27 ,  false );
    func( 34 ,f( "<=" ), 27 ,  false );
    func( 34 ,f( "!=" ), 27 ,  false );
    func( 34 ,f( "==" ), 27 ,  false );
    func( 34 ,f( "+" ), 27 ,  false );
    func( 34 ,f( "-" ), 27 ,  false );
    func( 34 ,f( "*" ), 27 ,  false );
    func( 34 ,f( "/" ), 27 ,  false );
    func( 34 ,f( "var" ), 27 ,  false );
    func( 34 ,f( "," ), 27, false );
    func( 34 ,f( "if" ), 27 ,  false );
    func( 34 ,f( "while" ), 27 ,  false );
    func( 34 ,f( "tk_identifier" ), 28 ,  false );
    func( 35 ,f( ")" ), 28 ,  false );
    func( 35 ,f( "}" ), 28 ,  false );
    func( 35 ,f( "||" ), 28 ,  false );
    func( 35 ,f( "&&" ), 28 ,  false );
    func( 35 ,f( "!" ), 28 ,  false );
    func( 35 ,f( ">" ), 28 ,  false );
    func( 35 ,f( ">=" ), 28 ,  false );
    func( 35 ,f( "<" ), 28 ,  false );
    func( 35 ,f( "<=" ), 28 ,  false );
    func( 35 ,f( "!=" ), 28 ,  false );
    func( 35 ,f( "==" ), 28 ,  false );
    func( 35 ,f( "+" ), 28 ,  false );
    func( 35 ,f( "-" ), 28 ,  false );
    func( 35 ,f( "*" ), 28 ,  false );
    func( 35 ,f( "/" ), 28 ,  false );
    func( 35 ,f( "var" ), 28 ,  false );
    func( 35 ,f( "," ), 28, false );
    func( 35 ,f( "if" ), 28 ,  false );
    func( 35 ,f( "while" ), 28 ,  false );
    func( 36 ,f( "tk_identifier" ), 34 , true );
    func( 36 ,f( "tk_integer" ), 37 , true );
    func( 36 ,f( "tk_float" ), 38 , true );
    func( 36 ,f( "tk_string" ), 39 , true );
    func( 36 ,f( "E" ), 56 , false );
    func( 36 ,f( "T" ), 32 , false );
    func( 36 ,f( "F" ), 33 , false );
    func( 36 ,f( "VAL" ), 35 , false );
    func( 37 ,f( "tk_identifier" ), 31 , false );
    func( 37 ,f( ")" ), 31 , false );
    func( 37 ,f( "}" ), 31 , false );
    func( 37 ,f( "||" ), 31 , false );
    func( 37 ,f( "&&" ), 31 , false );
    func( 37 ,f( ">" ), 31 , false );
    func( 37 ,f( ">=" ), 31 , false );
    func( 37 ,f( "<" ), 31 , false );
    func( 37 ,f( "<=" ), 31 , false );
    func( 37 ,f( "!=" ), 31 , false );
    func( 37 ,f( "==" ), 31 , false );
    func( 37 ,f( "+" ), 31 , false );
    func( 37 ,f( "-" ), 31 , false );
    func( 37 ,f( "*" ), 31 , false );
    func( 37 ,f( "/" ), 31 , false );
    func( 37 ,f( "var" ), 31 , false );
    func( 37 ,f( "," ), 31,false );
    func( 37 ,f( "if" ), 31 , false );
    func( 37 ,f( "while" ), 31 , false );
    func( 37 ,f( "&&" ), 31 , false );
    func( 38 ,f( "tk_identifier" ), 32 , false );
    func( 38 ,f( ")" ), 32 , false );
    func( 38 ,f( "}" ), 32 , false );
    func( 38 ,f( "||" ), 32 , false );
    func( 38 ,f( "&&" ), 32 , false );
    func( 38 ,f( ">" ), 32 , false );
    func( 38 ,f( ">=" ), 32 , false );
    func( 38 ,f( "<" ), 32 , false );
    func( 38 ,f( "<=" ), 32 , false );
    func( 38 ,f( "!=" ), 32 , false );
    func( 38 ,f( "==" ), 32 , false );
    func( 38 ,f( "+" ), 32 , false );
    func( 38 ,f( "-" ), 32 , false );
    func( 38 ,f( "*" ), 32 , false );
    func( 38 ,f( "/" ), 32 , false );
    func( 38 ,f( "var" ), 32 , false );
    func( 38 ,f( "," ), 32,false );
    func( 38 ,f( "if" ), 32 , false );
    func( 38 ,f( "while" ), 32 , false );
    func( 38 ,f( "&&" ), 32 , false );
    func( 39 ,f( "tk_identifier" ), 33 , false );
    func( 39 ,f( ")" ), 33 , false );
    func( 39 ,f( "}" ), 33 , false );
    func( 39 ,f( "||" ), 33 , false );
    func( 39 ,f( "&&" ), 33 , false );
    func( 39 ,f( ">" ), 33 , false );
    func( 39 ,f( ">=" ), 33 , false );
    func( 39 ,f( "<" ), 33 , false );
    func( 39 ,f( "<=" ), 33 , false );
    func( 39 ,f( "!=" ), 33 , false );
    func( 39 ,f( "==" ), 33 , false );
    func( 39 ,f( "+" ), 33 , false );
    func( 39 ,f( "-" ), 33 , false );
    func( 39 ,f( "*" ), 33 , false );
    func( 39 ,f( "/" ), 33 , false );
    func( 39 ,f( "var" ), 33 , false );
    func( 39 ,f( "," ), 33,false );
    func( 39 ,f( "if" ), 33 , false );
    func( 39 ,f( "while" ), 33 , false );
    func( 39 ,f( "&&" ), 33 , false );
    func( 40 ,f( "tk_identifier" ), 34 , true );
    func( 40 ,f( "(" ), 36 , true );
    func( 40 ,f( "tk_integer" ), 37 , true );
    func( 40 ,f( "tk_string" ), 38 , true );
    func( 40 ,f( "tk_float" ), 39 , true );
    func( 40 ,f( "E" ), 57 , false );
    func( 40 ,f( "T" ), 32 , false );
    func( 40 ,f( "F" ), 33 , false );
    func( 40 ,f( "VAL" ), 35 , false );
    func( 41 ,f( ")" ), 58 , true );
    func( 42 ,f( ")" ), 36 , false );
    func( 42 ,f( "," ), 59,true );
    func( 43 ,f( ")" ), 60 , true );
    func( 43 ,f( "||" ), 61 , true );
    func( 44 ,f( ")" ), 7 , false );
    func( 44 ,f( "||" ), 7 , false );
    func( 44 ,f( "&&" ), 62 , true );
    func( 45 ,f( ")" ), 9 , false );
    func( 45 ,f( "||" ), 9 , false );
    func( 45 ,f( "&&" ), 9 , false );
    func( 46 ,f( "tk_identifier" ), 34 , true );
    func( 46 ,f( "(" ), 47 , true );
    func( 46 ,f( "!" ), 46 , true );
    func( 46 ,f( "true" ), 49 , true );
    func( 46 ,f( "false" ), 50 , true );
    func( 46 ,f( "tk_integer" ), 37 , true );
    func( 46 ,f( "tk_string" ), 38 , true );
    func( 46 ,f( "tk_float" ), 39 , true );
    func( 46 ,f( "D" ), 63 , false );
    func( 46 ,f( "E" ), 48 , false );
    func( 46 ,f( "T" ), 32 , false );
    func( 46 ,f( "F" ), 33 , false );
    func( 46 ,f( "VAL" ), 35 , false );
    func( 47 ,f( "tk_identifier" ), 34 , true );
    func( 47 ,f( "(" ), 47 , true );
    func( 47 ,f( "!" ), 46 , true );
    func( 47 ,f( "true" ), 49 , true );
    func( 47 ,f( "false" ), 50 , true );
    func( 47 ,f( "tk_integer" ), 37 , true );
    func( 47 ,f( "tk_string" ), 38 , true );
    func( 47 ,f( "tk_float" ), 39 , true );
    func( 47 ,f( "B" ), 64 , false );
    func( 47 ,f( "C" ), 44 , false );
    func( 47 ,f( "D" ), 45 , false );
    func( 47 ,f( "E" ), 65 , false );
    func( 47 ,f( "T" ), 32 , false );
    func( 47 ,f( "F" ), 33 , false );
    func( 47 ,f( "VAL" ), 35 , false );
    func( 48 ,f( ">" ), 67 , true );
    func( 48 ,f( ">=" ), 68 , true );
    func( 48 ,f( "<" ), 69 , true );
    func( 48 ,f( "<=" ), 70 , true );
    func( 48 ,f( "!=" ), 71 , true );
    func( 48 ,f( "==" ), 72 , true );
    func( 48 ,f( "+" ), 52 , true );
    func( 48 ,f( "-" ), 53 , true );
    func( 48 ,f( "REL" ), 66 , false );
    func( 49 ,f( ")" ), 13 , false );
    func( 49 ,f( "||" ), 13 , false );
    func( 49 ,f( "&&" ), 13 , false );
    func( 50 ,f( ")" ), 14 , false );
    func( 50 ,f( "||" ), 14 , false );
    func( 50 ,f( "&&" ), 14 , false );
    func( 51 ,f( ")" ), 73 , true );
    func( 51 ,f( "||" ), 61 , true );
    func( 52 ,f( "tk_integer" ), 34 , true );
    func( 52 ,f( "(" ), 36 , true );
    func( 52 ,f( "tk_integer" ), 37 , true );
    func( 52 ,f( "tk_string" ), 38 , true );
    func( 52 ,f( "tk_float" ), 39 , true );
    func( 52 ,f( "T" ), 74 , false );
    func( 52 ,f( "F" ), 33 , false );
    func( 52 ,f( "VAL" ), 35 , false );
    func( 53 ,f( "tk_integer" ), 34 , true );
    func( 53 ,f( "(" ), 36 , true );
    func( 53 ,f( "tk_integer" ), 37 , true );
    func( 53 ,f( "tk_string" ), 38 , true );
    func( 53 ,f( "tk_float" ), 39 , true );
    func( 53 ,f( "T" ), 74 , false );
    func( 53 ,f( "F" ), 33 , false );
    func( 53 ,f( "VAL" ), 35 , false );
    func( 54 ,f( "tk_integer" ), 34 , true );
    func( 54 ,f( "(" ), 36 , true );
    func( 54 ,f( "tk_integer" ), 37 , true );
    func( 54 ,f( "tk_string" ), 38 , true );
    func( 54 ,f( "tk_float" ), 39 , true );
    func( 54 ,f( "F" ), 76 , false );
    func( 54 ,f( "VAL" ), 35 , false );
    func( 55 ,f( "tk_integer" ), 34 , true );
    func( 55 ,f( "(" ), 36 , true );
    func( 55 ,f( "tk_integer" ), 37 , true );
    func( 55 ,f( "tk_string" ), 38 , true );
    func( 55 ,f( "tk_float" ), 39 , true );
    func( 55 ,f( "F" ), 77 , false );
    func( 55 ,f( "VAL" ), 35 , false );
    func( 56 ,f( ")" ), 78 , true );
    func( 56 ,f( "+" ), 52 , true );
    func( 56 ,f( "-" ), 53 , true );
    func( 57 ,f( "tk_identifier" ), 34 , false );
    func( 57 ,f( "}" ), 34 , false );
    func( 57 ,f( "+" ), 52 , true );
    func( 57 ,f( "-" ), 53 , true );
    func( 57 ,f( "var" ), 34 , false );
    func( 57 ,f( "if" ), 34 , false );
    func( 57 ,f( "while" ), 34 , false );
    func( 58 ,f( "=" ), 79 , true );
    func( 59 ,f( "tk_identifier" ), 42 , true );
    func( 59 ,f( "tk_identifier" ), 80 , false );
    func( 60 ,f( "{" ), 81 , true );
    func( 61 ,f( "tk_identifier" ), 34 , true );
    func( 61 ,f( "(" ), 47 , true );
    func( 61 ,f( "!" ), 46 , true );
    func( 61 ,f( "true" ), 49 , true );
    func( 61 ,f( "false" ), 50 , true );
    func( 61 ,f( "tk_integer" ), 37 , true );
    func( 61 ,f( "tk_string" ), 38 , true );
    func( 61 ,f( "tk_float" ), 39 , true );
    func( 61 ,f( "C" ), 82 , false );
    func( 61 ,f( "D" ), 45 , false );
    func( 61 ,f( "E" ), 48 , false );
    func( 61 ,f( "T" ), 32 , false );
    func( 61 ,f( "F" ), 33 , false );
    func( 61 ,f( "VAL" ), 35 , false );
    func( 62 ,f( "tk_identifier" ), 34 , true );
    func( 62 ,f( "(" ), 47 , true );
    func( 62 ,f( "!" ), 46 , true );
    func( 62 ,f( "true" ), 49 , true );
    func( 62 ,f( "false" ), 50 , true );
    func( 62 ,f( "tk_integer" ), 37 , true );
    func( 62 ,f( "tk_string" ), 38 , true );
    func( 62 ,f( "tk_float" ), 39 , true );
    func( 62 ,f( "D" ), 83 , false );
    func( 62 ,f( "E" ), 48 , false );
    func( 62 ,f( "T" ), 32 , false );
    func( 62 ,f( "F" ), 33 , false );
    func( 62 ,f( "VAL" ), 35 , false );
    func( 63 ,f( ")" ), 10 , false );
    func( 63 ,f( "||" ), 10 , false );
    func( 63 ,f( "&&" ), 10 , false );
    func( 64 ,f( ")" ), 84 , true );
    func( 64 ,f( "||" ), 61 , true );
    func( 65 ,f( ")" ), 78 , true );
    func( 65 ,f( ">" ), 67 , true );
    func( 65 ,f( ">=" ), 68 , true );
    func( 65 ,f( "<" ), 69 , true );
    func( 65 ,f( "<=" ), 70 , true );
    func( 65 ,f( "!=" ), 71 , true );
    func( 65 ,f( "==" ), 72 , true );
    func( 65 ,f( "+" ), 52 , true );
    func( 65 ,f( "-" ), 53 , true );
    func( 65 ,f( "REL" ), 66 , false );
    func( 66 ,f( "tk_identifier" ), 34 , true );
    func( 66 ,f( "(" ), 36 , true );
    func( 66 ,f( "tk_integer" ), 37 , true );
    func( 66 ,f( "tk_string" ), 38 , true );
    func( 66 ,f( "tk_float" ), 39 , true );
    func( 66 ,f( "E" ), 85 , false );
    func( 66 ,f( "T" ), 32 , false );
    func( 66 ,f( "F" ), 33 , false );
    func( 66 ,f( "VAL" ), 35 , false );
    func( 67 ,f( "tk_identifier" ), 15 , false );
    func( 67 ,f( "(" ), 15 , false );
    func( 67 ,f( "tk_integer" ), 15 , false );
    func( 67 ,f( "tk_string" ), 15 , false );
    func( 67 ,f( "tk_float" ), 15 , false );
    func( 68 ,f( "tk_identifier" ), 16 , false );
    func( 68 ,f( "(" ), 16 , false );
    func( 68 ,f( "tk_integer" ), 16 , false );
    func( 68 ,f( "tk_string" ), 16 , false );
    func( 68 ,f( "tk_float" ), 16 , false );
    func( 69 ,f( "tk_identifier" ), 17 , false );
    func( 69 ,f( "(" ), 17 , false );
    func( 69 ,f( "tk_integer" ), 17 , false );
    func( 69 ,f( "tk_string" ), 17 , false );
    func( 69 ,f( "tk_float" ), 17 , false );
    func( 71 ,f( "tk_identifier" ), 19 , false );
    func( 71 ,f( "(" ), 19 , false );
    func( 71 ,f( "tk_integer" ), 19 , false );
    func( 71 ,f( "tk_string" ), 19 , false );
    func( 71 ,f( "tk_float" ), 19 , false );
    func( 72 ,f( "tk_identifier" ), 19 , false );
    func( 72 ,f( "(" ), 19 , false );
    func( 72 ,f( "tk_integer" ), 19 , false );
    func( 72 ,f( "tk_string" ), 19 , false );
    func( 72 ,f( "tk_float" ), 19 , false );
    func( 73 ,f( "{" ), 86 , true );
    func( 74 ,f( "tk_identifier" ), 21 , false );
    func( 74 ,f( ")" ), 21 , false );
    func( 74 ,f( "}" ), 21 , false );
    func( 74 ,f( "||" ), 21 , false );
    func( 74 ,f( "&&" ), 21 , false );
    func( 74 ,f( ">" ), 21 , false );
    func( 74 ,f( ">=" ), 21 , false );
    func( 74 ,f( "<" ), 21 , false );
    func( 74 ,f( "<=" ), 21 , false );
    func( 74 ,f( "!=" ), 21 , false );
    func( 74 ,f( "==" ), 21 , false );
    func( 74 ,f( "+" ), 21 , false );
    func( 74 ,f( "-" ), 21 , false );
    func( 74 ,f( "*" ), 21 , false );
    func( 74 ,f( "/" ), 54 , true );
    func( 74 ,f( "=" ), 55 , true );
    func( 74 ,f( "var" ), 21 , false );
    func( 74 ,f( "," ), 21,false );
    func( 74 ,f( "if" ), 21 , false );
    func( 74 ,f( "while" ), 21 , false );
    func( 75 ,f( "tk_identifier" ), 22 , false );
    func( 75 ,f( ")" ), 22 , false );
    func( 75 ,f( "}" ), 22 , false );
    func( 75 ,f( "||" ), 22 , false );
    func( 75 ,f( "&&" ), 22 , false );
    func( 75 ,f( ">" ), 22 , false );
    func( 75 ,f( ">=" ), 22 , false );
    func( 75 ,f( "<" ), 22 , false );
    func( 75 ,f( "<=" ), 22 , false );
    func( 75 ,f( "!=" ), 22 , false );
    func( 75 ,f( "==" ), 22 , false );
    func( 75 ,f( "+" ), 22 , false );
    func( 75 ,f( "-" ), 22 , false );
    func( 75 ,f( "*" ), 22 , false );
    func( 75 ,f( "/" ), 54 , true );
    func( 75 ,f( "=" ), 55 , true );
    func( 75 ,f( "var" ), 22 , false );
    func( 75 ,f( "," ), 22,false );
    func( 75 ,f( "if" ), 22 , false );
    func( 75 ,f( "while" ), 22 , false );
    func( 76 ,f( "tk_identifier" ), 24 , false );
    func( 76 ,f( ")" ), 24 , false );
    func( 76 ,f( "}" ), 24 , false );
    func( 76 ,f( "||" ), 24 , false );
    func( 76 ,f( "&&" ), 24 , false );
    func( 76 ,f( ">" ), 24 , false );
    func( 76 ,f( ">=" ), 24 , false );
    func( 76 ,f( "<" ), 24 , false );
    func( 76 ,f( "<=" ), 24 , false );
    func( 76 ,f( "!=" ), 24 , false );
    func( 76 ,f( "==" ), 24 , false );
    func( 76 ,f( "+" ), 24 , false );
    func( 76 ,f( "-" ), 24 , false );
    func( 76 ,f( "*" ), 24 , false );
    func( 76 ,f( "/" ), 24 , false );
    func( 76 ,f( "=" ), 24 , false );
    func( 76 ,f( "var" ), 24 , false );
    func( 76 ,f( "," ), 24,false );
    func( 76 ,f( "if" ), 24 , false );
    func( 76 ,f( "while" ), 24 , false );
    func( 77 ,f( "tk_identifier" ), 25 , false );
    func( 77 ,f( ")" ), 25 , false );
    func( 77 ,f( "}" ), 25 , false );
    func( 77 ,f( "||" ), 25 , false );
    func( 77 ,f( "&&" ), 25 , false );
    func( 77 ,f( ">" ), 25 , false );
    func( 77 ,f( ">=" ), 25 , false );
    func( 77 ,f( "<" ), 25 , false );
    func( 77 ,f( "<=" ), 25 , false );
    func( 77 ,f( "!=" ), 25 , false );
    func( 77 ,f( "==" ), 25 , false );
    func( 77 ,f( "+" ), 25 , false );
    func( 77 ,f( "-" ), 25 , false );
    func( 77 ,f( "*" ), 25 , false );
    func( 77 ,f( "/" ), 25 , false );
    func( 77 ,f( "=" ), 25 , false );
    func( 77 ,f( "var" ), 25 , false );
    func( 77 ,f( "," ), 25,false );
    func( 77 ,f( "if" ), 25 , false );
    func( 77 ,f( "while" ), 25 , false );
    func( 78 ,f( "tk_identifier" ), 29 , false );
    func( 78 ,f( ")" ), 29 , false );
    func( 78 ,f( "}" ), 29 , false );
    func( 78 ,f( "||" ), 29 , false );
    func( 78 ,f( "&&" ), 29 , false );
    func( 78 ,f( ">" ), 29 , false );
    func( 78 ,f( ">=" ), 29 , false );
    func( 78 ,f( "<" ), 29 , false );
    func( 78 ,f( "<=" ), 29 , false );
    func( 78 ,f( "!=" ), 29 , false );
    func( 78 ,f( "==" ), 29 , false );
    func( 78 ,f( "+" ), 29 , false );
    func( 78 ,f( "-" ), 29 , false );
    func( 78 ,f( "*" ), 29 , false );
    func( 78 ,f( "/" ), 29 , false );
    func( 78 ,f( "=" ), 29 , false );
    func( 78 ,f( "var" ), 29 , false );
    func( 78 ,f( "," ), 29,false );
    func( 78 ,f( "if" ), 29 , false );
    func( 78 ,f( "while" ), 29 , false );
    func( 79 ,f( "(" ), 87 , true );
    func( 80 ,f( ")" ), 37 , false );
    func( 81 ,f( "tk_identifier" ), 81 , false );
    func( 81 ,f( "}" ), 5 , false );
    func( 81 ,f( "var" ), 5 , false );
    func( 81 ,f( "if" ), 5 , false );
    func( 81 ,f( "while" ), 5 , false );
    func( 81 ,f( "STMTS" ), 88 , false );
    func( 82 ,f( ")" ), 6 , false );
    func( 82 ,f( "||" ), 6 , false );
    func( 82 ,f( "&&" ), 62 , true );
    func( 83 ,f( ")" ), 8 , false );
    func( 83 ,f( "||" ), 8 , false );
    func( 83 ,f( "&&" ), 8 , false );
    func( 84 ,f( ")" ), 11 , false );
    func( 84 ,f( "||" ), 11 , false );
    func( 84 ,f( "&&" ), 11 , false );
    func( 85 ,f( ")" ), 12 , false );
    func( 85 ,f( "||" ), 12 , false );
    func( 85 ,f( "&&" ), 12 , false );
    func( 85 ,f( "+" ), 52 , true );
    func( 85 ,f( "-" ), 53 , true );
    func( 86 ,f( "tk_identifier" ), 5 , false );
    func( 86 ,f( "}" ), 5 , false );
    func( 86 ,f( "var" ), 5 , false );
    func( 86 ,f( "if" ), 5 , false );
    func( 86 ,f( "while" ), 5 , false );
    func( 86 ,f( "STMTS" ), 89 , true );
    func( 87 ,f( "tk_identifier" ), 34 , true );
    func( 87 ,f( "(" ), 36 , true );
    func( 87 ,f( "tk_integer" ), 37 , true );
    func( 87 ,f( "tk_string" ), 38 , true );
    func( 87 ,f( "tk_float" ), 39 , true );
    func( 87 ,f( "E" ), 91 , false );
    func( 87 ,f( "T" ), 32 , false );
    func( 87 ,f( "F" ), 33 , false );
    func( 87 ,f( "VAL" ), 35 , false );
    func( 87 ,f( "VALLIST" ), 90 , false );
    func( 95 ,f( "tk_identifier" ), 34 , true );
    func( 95 ,f( "(" ), 36 , true );
    func( 95 ,f( "tk_integer" ), 37 , true );
    func( 95 ,f( "tk_string" ), 38 , true );
    func( 95 ,f( "tk_float" ), 39 , true );
    func( 95 ,f( "E" ), 91 , false );
    func( 95 ,f( "T" ), 32 , false );
    func( 95 ,f( "F" ), 33 , false );
    func( 95 ,f( "VAL" ), 35 , false );
    func( 95 ,f( "VALLIST" ), 90 , false );
    func( 92 ,f( "tk_identifier" ), 43 , false );
    func( 92 ,f( "}" ), 43 , false );
    func( 92 ,f( "var" ), 43 , false );
    func( 92 ,f( "if" ), 43 , false );
    func( 92 ,f( "while" ), 43 , false );
    func( 93 ,f( "tk_identifier" ), 41 , false );
    func( 93 ,f( "}" ), 41 , false );
    func( 93 ,f( "var" ), 41 , false );
    func( 93 ,f( "if" ), 41 , false );
    func( 93 ,f( "while" ), 41 , false );
    func( 94 ,f( "tk_identifier" ), 35 , false );
    func( 94 ,f( "}" ), 35 , false );
    func( 94 ,f( "var" ), 35 , false );
    func( 94 ,f( "if" ), 35 , false );
    func( 94 ,f( "while" ), 35 , false );
    func( 94 ,f( "else" ), 97 , true );
    func( 96 ,f( "tk_identifier" ), 40 , false );
    func( 96 ,f( "}" ), 40 , false );
    func( 96 ,f( "var" ), 40 , false );
    func( 96 ,f( "if" ), 40 , false );
    func( 96 ,f( "while" ), 40 , false );
    func( 99 ,f( "tk_identifier" ), 5 , false );
    func( 99 ,f( "}" ), 5 , false );
    func( 99 ,f( "var" ), 5 , false );
    func( 99 ,f( "if" ), 5 , false );
    func( 99 ,f( "while" ), 5 , false );
    func( 99 ,f( "STMTS" ), 100 , true );
    func( 101 ,f( "tk_identifier" ), 42 , false );
    func( 101 ,f( "}" ), 42 , false );
    func( 101 ,f( "var" ), 42 , false );
    func( 101 ,f( "if" ), 42 , false );
    func( 101 ,f( "while" ), 42 , false );
    func( 100 ,f( "tk_identifier" ), 21 , true );
    func( 100 ,f( "}" ), 101 , true );
    func( 100 ,f( "var" ), 22 , true );
    func( 100 ,f( "if" ), 24 , true );
    func( 100 ,f( "while" ), 23 , true );
    func( 100 ,f( "ASSIGNMENT" ), 17 , true );
    func( 100 ,f( "DECLARATIONS" ), 18 , true );
    func( 100 ,f( "IFELSE" ), 20 , true );
    func( 100 ,f( "WHILE" ), 19 , true );
    func( 88 ,f( "tk_identifier" ), 21 , true );
    func( 88 ,f( "}" ), 92 , true );
    func( 88 ,f( "var" ), 22 , true );
    func( 88 ,f( "if" ), 24 , true );
    func( 88 ,f( "ASSIGNMENT" ), 17 , true );
    func( 88 ,f( "DECLARATIONS" ), 18 , true );
    func( 88 ,f( "IFELSE" ), 20 , true );
    func( 88 ,f( "WHILE" ), 19 , true );
    func( 89 ,f( "tk_identifier" ), 21 , true );
    func( 89 ,f( "}" ), 92 , true );
    func( 89 ,f( "var" ), 22 , true );
    func( 89 ,f( "if" ), 24 , true );
    func( 89 ,f( "ASSIGNMENT" ), 17 , true );
    func( 89 ,f( "DECLARATIONS" ), 18 , true );
    func( 89 ,f( "IFELSE" ), 20 , true );
    func( 89 ,f( "WHILE" ), 19 , true );
    func( 90 ,f( ")" ),  94 , true );
    func( 97 ,f( "{" ),99 , true );
    func( 98 ,f( ")" ), 39 , false );
    func( 91 ,f( ")" ), 38 , false );
    func( 91 ,f( "+" ), 52 , true );
    func( 91 ,f( "-" ), 53 , true );
    func( 91 ,f( "," ), 95,true );
}

void set_rule(int i,char *a,char *b)
{
    strcpy(grammar[i].LHS, a);
    strcpy(grammar[i].RHS, b);
}

void load_grammar_rules()
{
    int i=0;
    set_rule(i++,"PROGRAM","object tk_identifier { def main ( args : Array [ String ] ) { STMTS } }");
    set_rule(i++,"STMTS","STMTS ASSIGNMENT");
    set_rule(i++,"STMTS","STMTS DECLARATIONS");
    set_rule(i++,"STMTS","STMTS WHILE");
    set_rule(i++,"STMTS","STMTS IFELSE");
    set_rule(i++,"STMTS","");
    set_rule(i++,"B","B || C");
    set_rule(i++,"B","C");
    set_rule(i++,"C","C && D");
    set_rule(i++,"C","D");
    set_rule(i++,"D","! D");
    set_rule(i++,"D","( B )");
    set_rule(i++,"D","E REL E");
    set_rule(i++,"D","true");
    set_rule(i++,"D","false");
    set_rule(i++,"REL",">");
    set_rule(i++,"REL",">=");
    set_rule(i++,"REL","<");
    set_rule(i++,"REL","<=");
    set_rule(i++,"REL","!=");
    set_rule(i++,"REL","==");
    set_rule(i++,"E","E + T");
    set_rule(i++,"E","E - T");
    set_rule(i++,"E","T");
    set_rule(i++,"T","T * F");
    set_rule(i++,"T","T / F");
    set_rule(i++,"T","F");
    set_rule(i++,"F","tk_identifier");
    set_rule(i++,"F","VAL");
    set_rule(i++,"F","( E )");
    set_rule(i++,"ASSIGNMENT","tk_identifier = E");
    set_rule(i++,"VAL","tk_integer");
    set_rule(i++,"VAL","tk_string");
    set_rule(i++,"VAL","tk_float");
    set_rule(i++,"DECLARATIONS","var tk_identifier = E");
    set_rule(i++,"DECLARATIONS","var ( IDLIST ) = ( VALLIST )");
    set_rule(i++,"IDLIST","tk_identifier");
    set_rule(i++,"IDLIST","tk_identifier , IDLIST");
    set_rule(i++,"VALLIST","E");
    set_rule(i++,"VALLIST","E , VALLIST");
    set_rule(i++,"IFELSE","if ( B ) { STMTS } ELSE");
    set_rule(i++,"ELSE","");
    set_rule(i++,"ELSE","else { STMTS }");
    set_rule(i++,"WHILE","while ( B ) { STMTS }");
    N_RULES=i;
    printf("Total number of rules=%d\n",N_RULES);
}

void get_LHS(int i,char *ret)
{
    if(i>=0 && i<N_RULES)
    {
        //printf("%s",grammar[i].LHS);
        strcpy(ret,grammar[i].LHS);
        //printf("%s",ret);
        return;
    }

}
int pop_count(int index)
{
    int j=0;
    int ret=0;
    if(grammar[index].RHS[0]==0)
        return 0;
    while(grammar[index].RHS[j]!=0)
    {
        //printf("%c",grammar[index].RHS[j]);
        if(grammar[index].RHS[j]==' ')
            ret++;
        j++;
    }
    return ret+1;
}
void print_nodestack()
{
    if(node_top==-1)
    {
        printf("Empty!\n");
        return;
    }
    int i=0;
    for(;i<=node_top;i++)
    {
        printf("%s ",stack[i]->val);
    }
    printf("\n");
}
void node_push(node *push)
{
 stack[++node_top]=push;
}
node* node_pop()
{
 node *temp=stack[node_top];
 printf("Popped:%s\n",temp->val);
 node_top--;
 return temp;
}
void int_push(int x){
    int_tos++;
    int_stack[int_tos]=x;
}
void int_pop(){
    int_tos--;
}
node* create_node(char *str)
{
	node* temp=(node*)malloc(sizeof(node));
	temp->n_child=0;
	temp->parent=NULL;
	strcpy(temp->val,str);
	int i;
	for(i=0;i<30;i++)
	{
		temp->children[i]=NULL;
	}
	return temp;
}
int f(char *str)
{
    int i;
    for(i=0;i<N;i++)
    {
        if(strcmp(g[i],str)==0)
            return i;
    }
    return -1;
}
void func(int a,int b,int c,int d){
    array[a][b].x=c;
    array[a][b].shift=d;
}

void set_firstandfollow()
{
    fstfllw[f("PROGRAM")].first=1;
    fstfllw[f("PROGRAM")].first_set[0]=f("PROGRAM");
    fstfllw[f("PROGRAM")].follow=1;

    fstfllw[f("STMTS")].first=5;
    fstfllw[f("STMTS")].first_set[0]=f("");
    fstfllw[f("STMTS")].first_set[1]=f("tk_identifier");
    fstfllw[f("STMTS")].first_set[2]=f("tk_integer");
    fstfllw[f("STMTS")].first_set[3]=f("tk_string");
    fstfllw[f("STMTS")].first_set[4]=f("tk_float");
    fstfllw[f("STMTS")].follow=5;
    fstfllw[f("STMTS")].follow_set[0]=f("}");
    fstfllw[f("STMTS")].follow_set[1]=f("tk_identifier");
    fstfllw[f("STMTS")].follow_set[2]=f("var");
    fstfllw[f("STMTS")].follow_set[3]=f("while");
    fstfllw[f("STMTS")].follow_set[4]=f("if");

    fstfllw[f("B")].first=8;
    fstfllw[f("B")].first_set[0]=f("!");
    fstfllw[f("B")].first_set[1]=f("(");
    fstfllw[f("B")].first_set[2]=f("true");
    fstfllw[f("B")].first_set[3]=f("false");
    fstfllw[f("B")].first_set[4]=f("tk_identifier");
    fstfllw[f("B")].first_set[5]=f("tk_integer");
    fstfllw[f("B")].first_set[6]=f("tk_string");
    fstfllw[f("B")].first_set[7]=f("tk_float");
    fstfllw[f("B")].follow=2;
    fstfllw[f("B")].follow_set[0]=f("||");
    fstfllw[f("B")].follow_set[1]=f(")");

    fstfllw[f("C")].first=8;
    fstfllw[f("C")].first_set[0]=f("!");
    fstfllw[f("C")].first_set[1]=f("(");
    fstfllw[f("C")].first_set[2]=f("true");
    fstfllw[f("C")].first_set[3]=f("false");
    fstfllw[f("C")].first_set[4]=f("tk_identifier");
    fstfllw[f("C")].first_set[5]=f("tk_integer");
    fstfllw[f("C")].first_set[6]=f("tk_string");
    fstfllw[f("C")].first_set[7]=f("tk_float");
    fstfllw[f("C")].follow=3;
    fstfllw[f("C")].follow_set[0]=f("||");
    fstfllw[f("C")].follow_set[1]=f("&&");
    fstfllw[f("C")].follow_set[1]=f(")");

    fstfllw[f("D")].first=8;
    fstfllw[f("D")].first_set[0]=f("!");
    fstfllw[f("D")].first_set[1]=f("(");
    fstfllw[f("D")].first_set[2]=f("true");
    fstfllw[f("D")].first_set[3]=f("false");
    fstfllw[f("D")].first_set[4]=f("tk_identifier");
    fstfllw[f("D")].first_set[5]=f("tk_integer");
    fstfllw[f("D")].first_set[6]=f("tk_string");
    fstfllw[f("D")].first_set[7]=f("tk_float");
    fstfllw[f("D")].follow=3;
    fstfllw[f("D")].follow_set[0]=f("||");
    fstfllw[f("D")].follow_set[1]=f("&&");
    fstfllw[f("D")].follow_set[1]=f(")");

    fstfllw[f("REL")].first=6;
    fstfllw[f("REL")].first_set[0]=f(">");
    fstfllw[f("REL")].first_set[1]=f(">=");
    fstfllw[f("REL")].first_set[2]=f("<");
    fstfllw[f("REL")].first_set[3]=f("<=");
    fstfllw[f("REL")].first_set[4]=f("!=");
    fstfllw[f("REL")].first_set[5]=f("==");
    fstfllw[f("REL")].follow=5;
    fstfllw[f("REL")].follow_set[0]=f("tk_identifier");
    fstfllw[f("REL")].follow_set[1]=f("(");
    fstfllw[f("REL")].follow_set[2]=f("tk_integer");
    fstfllw[f("REL")].follow_set[3]=f("tk-string");
    fstfllw[f("REL")].follow_set[4]=f("tk_float");

    fstfllw[f("E")].first=5;
    fstfllw[f("E")].first_set[0]=f("tk_identifier");
    fstfllw[f("E")].first_set[1]=f("(");
    fstfllw[f("E")].first_set[2]=f("tk_integer");
    fstfllw[f("E")].first_set[3]=f("tk_string");
    fstfllw[f("E")].first_set[4]=f("tk_float");
    fstfllw[f("E")].follow=17;
    fstfllw[f("E")].follow_set[0]=f(">");
    fstfllw[f("E")].follow_set[1]=f(">=");
    fstfllw[f("E")].follow_set[2]=f("<");
    fstfllw[f("E")].follow_set[3]=f("<=");
    fstfllw[f("E")].follow_set[4]=f("!=");
    fstfllw[f("E")].follow_set[5]=f("==");
    fstfllw[f("E")].follow_set[6]=f("||");
    fstfllw[f("E")].follow_set[7]=f("&&");
    fstfllw[f("E")].follow_set[8]=f("+");
    fstfllw[f("E")].follow_set[9]=f("-");
    fstfllw[f("E")].follow_set[10]=f(")");
    fstfllw[f("E")].follow_set[11]=f("}");
    fstfllw[f("E")].follow_set[12]=f("tk_identifier");
    fstfllw[f("E")].follow_set[13]=f("var");
    fstfllw[f("E")].follow_set[14]=f(",");
    fstfllw[f("E")].follow_set[15]=f("wile");
    fstfllw[f("E")].follow_set[16]=f("if");

    fstfllw[f("T")].first=5;
    fstfllw[f("T")].first_set[0]=f("tk_identifier");
    fstfllw[f("T")].first_set[1]=f("(");
    fstfllw[f("T")].first_set[2]=f("tk_integer");
    fstfllw[f("T")].first_set[3]=f("tk_string");
    fstfllw[f("T")].first_set[4]=f("tk_float");
    fstfllw[f("T")].follow=19;
    fstfllw[f("T")].follow_set[0]=f(">");
    fstfllw[f("T")].follow_set[1]=f(">=");
    fstfllw[f("T")].follow_set[2]=f("<");
    fstfllw[f("T")].follow_set[3]=f("<=");
    fstfllw[f("T")].follow_set[4]=f("!=");
    fstfllw[f("T")].follow_set[5]=f("==");
    fstfllw[f("T")].follow_set[6]=f("||");
    fstfllw[f("T")].follow_set[7]=f("&&");
    fstfllw[f("T")].follow_set[8]=f("+");
    fstfllw[f("T")].follow_set[9]=f("-");
    fstfllw[f("T")].follow_set[10]=f("*");
    fstfllw[f("T")].follow_set[11]=f("/");
    fstfllw[f("T")].follow_set[12]=f(")");
    fstfllw[f("T")].follow_set[13]=f("}");
    fstfllw[f("T")].follow_set[14]=f("tk_identifier");
    fstfllw[f("T")].follow_set[15]=f("var");
    fstfllw[f("T")].follow_set[16]=f(",");
    fstfllw[f("T")].follow_set[17]=f("wile");
    fstfllw[f("T")].follow_set[18]=f("if");

    fstfllw[f("F")].first=5;
    fstfllw[f("F")].first_set[0]=f("tk_identifier");
    fstfllw[f("F")].first_set[1]=f("(");
    fstfllw[f("F")].first_set[2]=f("tk_integer");
    fstfllw[f("F")].first_set[3]=f("tk_string");
    fstfllw[f("F")].first_set[4]=f("tk_float");
    fstfllw[f("F")].follow=19;
    fstfllw[f("F")].follow_set[0]=f(">");
    fstfllw[f("F")].follow_set[1]=f(">=");
    fstfllw[f("F")].follow_set[2]=f("<");
    fstfllw[f("F")].follow_set[3]=f("<=");
    fstfllw[f("F")].follow_set[4]=f("!=");
    fstfllw[f("F")].follow_set[5]=f("==");
    fstfllw[f("F")].follow_set[6]=f("||");
    fstfllw[f("F")].follow_set[7]=f("&&");
    fstfllw[f("F")].follow_set[8]=f("+");
    fstfllw[f("F")].follow_set[9]=f("-");
    fstfllw[f("F")].follow_set[10]=f("*");
    fstfllw[f("F")].follow_set[11]=f("/");
    fstfllw[f("F")].follow_set[12]=f(")");
    fstfllw[f("F")].follow_set[13]=f("}");
    fstfllw[f("F")].follow_set[14]=f("tk_identifier");
    fstfllw[f("F")].follow_set[15]=f("var");
    fstfllw[f("F")].follow_set[16]=f(",");
    fstfllw[f("F")].follow_set[17]=f("wile");
    fstfllw[f("F")].follow_set[18]=f("if");

    fstfllw[f("ASSIGNMENT")].first=1;
    fstfllw[f("ASSIGNMENT")].first_set[0]=f("tk_identifier");
    fstfllw[f("ASSIGNMENT")].follow=5;
    fstfllw[f("ASSIGNMENT")].follow_set[0]=f("}");
    fstfllw[f("ASSIGNMENT")].follow_set[1]=f("tk_identifier");
    fstfllw[f("ASSIGNMENT")].follow_set[2]=f("var");
    fstfllw[f("ASSIGNMENT")].follow_set[3]=f("while");
    fstfllw[f("ASSIGNMENT")].follow_set[4]=f("if");

    fstfllw[f("VAL")].first=3;
    fstfllw[f("VAL")].first_set[0]=f("tk_integer");
    fstfllw[f("VAL")].first_set[1]=f("tk_string");
    fstfllw[f("VAL")].first_set[2]=f("tk_float");
    fstfllw[f("VAL")].follow=19;
    fstfllw[f("VAL")].follow_set[0]=f(">");
    fstfllw[f("VAL")].follow_set[1]=f(">=");
    fstfllw[f("VAL")].follow_set[2]=f("<");
    fstfllw[f("VAL")].follow_set[3]=f("<=");
    fstfllw[f("VAL")].follow_set[4]=f("!=");
    fstfllw[f("VAL")].follow_set[5]=f("==");
    fstfllw[f("VAL")].follow_set[6]=f("||");
    fstfllw[f("VAL")].follow_set[7]=f("&&");
    fstfllw[f("VAL")].follow_set[8]=f("+");
    fstfllw[f("VAL")].follow_set[9]=f("-");
    fstfllw[f("VAL")].follow_set[10]=f("*");
    fstfllw[f("VAL")].follow_set[11]=f("/");
    fstfllw[f("VAL")].follow_set[12]=f(")");
    fstfllw[f("VAL")].follow_set[13]=f("}");
    fstfllw[f("VAL")].follow_set[14]=f("tk_identifier");
    fstfllw[f("VAL")].follow_set[15]=f("var");
    fstfllw[f("VAL")].follow_set[16]=f(",");
    fstfllw[f("VAL")].follow_set[17]=f("wile");
    fstfllw[f("VAL")].follow_set[18]=f("if");

    fstfllw[f("DECLARATIONS")].first=1;
    fstfllw[f("DECLARATIONS")].first_set[0]=f("var");
    fstfllw[f("DECLARATIONS")].follow=5;
    fstfllw[f("DECLARATIONS")].follow_set[0]=f("}");
    fstfllw[f("DECLARATIONS")].follow_set[1]=f("tk_identifier");
    fstfllw[f("DECLARATIONS")].follow_set[2]=f("var");
    fstfllw[f("DECLARATIONS")].follow_set[3]=f("while");
    fstfllw[f("DECLARATIONS")].follow_set[4]=f("if");

    fstfllw[f("IDLIST")].first=1;
    fstfllw[f("IDLIST")].first_set[0]=f("tk_identifier");
    fstfllw[f("IDLIST")].follow=1;
    fstfllw[f("IDLIST")].follow_set[0]=f(")");

    fstfllw[f("VALLIST")].first=5;
    fstfllw[f("VALLIST")].first_set[0]=f("tk_identifier");
    fstfllw[f("VALLIST")].first_set[1]=f("(");
    fstfllw[f("VALLIST")].first_set[2]=f("tk_integer");
    fstfllw[f("VALLIST")].first_set[3]=f("tk_string");
    fstfllw[f("VALLIST")].first_set[4]=f("tk_float");
    fstfllw[f("VALLIST")].follow=1;
    fstfllw[f("VALLIST")].follow_set[0]=f(")");

    fstfllw[f("IFELSE")].first=1;
    fstfllw[f("IFELSE")].first_set[0]=f("if");
    fstfllw[f("IFELSE")].follow=5;
    fstfllw[f("IFELSE")].follow_set[0]=f("}");
    fstfllw[f("IFELSE")].follow_set[1]=f("tk_identifier");
    fstfllw[f("IFELSE")].follow_set[2]=f("var");
    fstfllw[f("IFELSE")].follow_set[3]=f("while");
    fstfllw[f("IFELSE")].follow_set[4]=f("if");

    fstfllw[f("ELSE")].first=2;
    fstfllw[f("ELSE")].first_set[0]=f("");
    fstfllw[f("ELSE")].first_set[1]=f("else");
    fstfllw[f("ELSE")].follow=5;
    fstfllw[f("ELSE")].follow_set[0]=f("}");
    fstfllw[f("ELSE")].follow_set[1]=f("tk_identifier");
    fstfllw[f("ELSE")].follow_set[2]=f("var");
    fstfllw[f("ELSE")].follow_set[3]=f("while");
    fstfllw[f("ELSE")].follow_set[4]=f("if");

    fstfllw[f("WHILE")].first=1;
    fstfllw[f("WHILE")].first_set[0]=f("while");
    fstfllw[f("WHILE")].follow=5;
    fstfllw[f("WHILE")].follow_set[0]=f("}");
    fstfllw[f("WHILE")].follow_set[1]=f("tk_identifier");
    fstfllw[f("WHILE")].follow_set[2]=f("var");
    fstfllw[f("WHILE")].follow_set[3]=f("while");
    fstfllw[f("WHILE")].follow_set[4]=f("if");

}
