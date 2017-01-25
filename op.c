#include <stdio.h>
#include <string.h>
#define OP 100
#define KEY 50
#define true 1
#define false 0
int Token_val;
const int Keywords=53;
const int tilde=1;
const int plusequal=2;
const int plus=3;
const int minus=4;
const int minusequal=5;
const int mulequal=6;
const int mul=7;
const int divequal=8;
const int divide=9;
const int modequal=10;
const int mod=11;
const int equality=12;
const int assignment=13;
const int rightunsignshft=14;
const int rightshftassign=15;
const int rightshft=16;
const int xor=17;
const int xorasign=18;
const int or=19;
const int orasign=20;
const int logicalor=21;
const int and=22;
const int andasign=23;
const int logicaland=24;
const int not=25;
const int notassign=26;
const int lesser=27;
const int lessequal=28;
const int leftshft=29;
const int leftshftasgnmnt=30;
const int greater=31;
const int greaterequal=32;
FILE *fp;
char str[200]; // BUFFER FOR KEYWORD
char current;
int state=1;
int lexemebegin=0;
int forward=0;
char *key[]={"abstract","case","catch","class","def","do","else","extends","false","final","finally","for","forSome",
				"if","implicit","import","lazy","match","new","Null","object","override","package","private","protected",
				"return","scaled","super","this","throw","trait","Try","true","type","val","Var","while","with","yield",
				/*":","=>","<-","<:","<%",">:","#","@"*/};
int retkeyword(int retract)
{
	for(int i=0;i<Keywords;i++)
	{
		if(strcmp(str,key[i])==0)
		{
			Token_val=i+100;
			if(retract)
			{
				forward--;
				fseek(fp,forward,SEEK_SET);
			}
			lexemebegin=forward;
			return KEY;
		}
	}
	return -1;
}
int retop(int x, int retract)
{
	Token_val=x;
	if(retract)
	{
		forward--;
		fseek(fp,forward,SEEK_SET);
	}
	lexemebegin=forward;
	//printf("%d\n",x);
	state=1;
	return OP;
}
int main()
{
	
	fp=fopen("f.txt","r");
	
	while(1)
	{
		switch(state)
		{
			case 1:
			{
				current=fgetc(fp);forward++;
				if(current==EOF)
					return 0;
				if(current=='+')
					state=2;
				if(current=='-')
					state=3;
				if(current=='*')
					state=4;
				if(current=='/')
					state=5;
				if(current=='%')
					state=6;
				if(current=='=')
					state=7;
				if(current=='>')
					state=8;
				if(current=='<')
					state=9;
				if(current=='!')
					state=10;
				if(current=='&')
					state=11;
				if(current=='|')
					state=12;
				if(current=='^')
					state=13;
				if(current=='~')
					state=14;
				if(state==1)
					printf("ERROR");
				break;
			}
			case 2:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=15;
				else
					state=16;
				break;
			}
			case 3:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=17;
				else
					state=18;
				break;
			}
			case 4:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=19;
				else
					state=20;
				break;
			}
			case 5:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=21;
				else
					state=22;
				break;
			}
			case 6:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=23;
				else
					state=24;
				break;
			}
			case 7:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=25;
				else
					state=26;
				break;
			}
			case 8:
			{
				current=fgetc(fp);forward++;
				if(current=='>')
					state=27;
				else if(current=='=')
					state=31;
				else
					state=32;
				break;
			}
			case 9:
			{
				current=fgetc(fp);forward++;
				if(current=='<')
					state=33;
				else if(current=='=')
					state=36;
				else
					state=37;
				break;
			}
			case 10:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=38;
				else
					state=39;
				break;
			}
			case 11:
			{
				current=fgetc(fp);forward++;
				if(current=='&')
					state=40;
				else if(current=='=')
					state=41;
				else
					state=42;
				break;
			}
			case 12:
			{
				current=fgetc(fp);forward++;
				if(current=='|')
					state=43;
				else if(current=='=')
					state=44;
				else
					state=45;
				break;
			}
			case 13:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=46;
				else
					state=47;
				break;
			}
			case 14:
			{
				retop(tilde,false);
				break;
			}
			case 15:
			{
				retop(plusequal,false);
				break;
			}
			case 16:
			{
				retop(plus,true);
				break;
			}
			case 17:
			{
				retop(minusequal,false);
				break;
			}
			case 18:
			{
				retop(minus,true);
				break;
			}
			case 19:
			{
				retop(mulequal,false);
				break;
			}
			case 20:
			{
				retop(mul,true);
				break;
			}
			case 21:
			{
				retop(divequal,false);
				break;
			}
			case 22:
			{
				retop(divide,true);
				break;
			}
			case 23:
			{
				retop(modequal,false);
				break;
			}
			case 24:
			{
				retop(mod,true);
				break;
			}
			case 25:
			{
				retop(equality,false);
				break;
			}
			case 26:
			{
				retop(assignment,true);
				break;
			}
			case 27:
			{
				current=fgetc(fp);forward++;
				if(current=='>')
					state=28;
				else if(current=='=')
					state=29;
				else
					state=30;
				break;
			}
			case 28:
			{
				retop(rightunsignshft,false);
				break;
			}
			case 29:
			{
				retop(rightshftassign,false);
				break;
			}
			case 30:
			{
				retop(rightshft,true);
				break;
			}
			case 31:
			{
				retop(greaterequal,false);
				break;
			}
			case 32:
			{
				retop(greater,true);
				break;
			}
			case 33:
			{
				current=fgetc(fp);forward++;
				if(current=='=')
					state=34;
				else
					state=35;
				break;
			}
			case 34:
			{
				retop(leftshftasgnmnt,false);
				break;
			}
			case 35:
			{
				retop(leftshft,true);
				break;
			}
			case 36:
			{
				retop(lessequal,false);
				break;
			}
			case 37:
			{
				retop(lesser,true);
				break;
			}
			case 38:
			{
				retop(notassign,false);
				break;
			}
			case 39:
			{
				retop(not,true);
				break;
			}
			case 40:
			{
				retop(logicaland,false);
				break;
			}
			case 41:
			{
				retop(andasign,false);
				break;
			}
			case 42:
			{
				retop(and,true);
				break;
			}
			case 43:
			{
				retop(logicalor,false);
				break;
			}
			case 44:
			{
				retop(orasign,false);
				break;
			}
			case 45:
			{
				retop(or,true);
				break;
			}
			case 46:
			{
				retop(xorasign,false);
				break;
			}
			case 47:
			{
				retop(xor,true);
				break;
			}
		}
	}
}