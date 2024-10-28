#include<stdio.h>
#include<math.h>
const int rate=8000,nlen=rate/8;
char DEBUG=0;

char program[256],pp=0,stack[256],sp=0,melodies[256*256],a,b;int res;

void push(char a){stack[sp++]=a;}
char pop(){return stack[--sp];}

void main(int argc,char**argv){
	if(argc<2)return;
	if(argc>2)DEBUG=1;
	FILE*f=fopen(argv[1],"r");

	/*for(int m=0;;m++){ // XXX this is a mess
		char a=getc(f),mp=0;
		if(a==':')break;
		do melodies[256*m+mp++]=a;
		while((a=getc(f))!=10);
	}while((a=getc(f))!='$')if(a!=10)program[pp++]=a;
	program[pp]='$';*/

	b=0;
	newmelody:
	a=getc(f);
	if(a!=':' && a!=10){ // melody
		melodies[b<<8]=1;
		melodies[(b<<8)+1]=a;
		while((a=getc(f))!=10)melodies[(b<<8) + (melodies[b<<8]++) +1]=a;
		b++;goto newmelody;
	}b=0;
	while((a=getc(f))!='$')if(a!=10)program[b++]=a;
	program[b]='$';

	int t=1;
	while(t++){ // main loop
	pp=sp=0;
	while(program[pp]!='$'){ // XXX make this a while loop?
		if(DEBUG)printf("cmd <%c>; ",program[pp]);
		switch(program[pp]){
			case'0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
				push(program[pp]-'0');break;
			case'a':case'b':case'c':case'd':case'e':case'f':
				push(program[pp]-'a'+10);break;
			case'`':a=pop();push(a|(pop()<<4));break;
			case't':
				a=pop();b=pop(); // b is melody, a is read rate
				a=melodies[((t/nlen/a)%melodies[b<<8]) + (b<<8) +1];
				//a=(a==' '?0:a);
				push(a);break;

			case'T':
				a=pop();b=pop(); // b is melody, a is read rate
				push(melodies[(((t/nlen+a/2)/a)%melodies[b<<8]) + (b<<8) +1]);
				// at read rate 1, this hits the second note at the 1000th sample
				// at read rate 2, -//- 2000th
				break;

			case'~':{ // sine
				a=pop();if(a==32){push(0);break;}
				float freq = 440*powf(2,(float)(a-49)/12)/rate;
				push(sinf((float)t*freq*M_PI*2)*127+127);
				}break;

			case'%':{ // sawtooth
				a=pop();if(a==32){push(0);break;}
				float freq = 440*powf(2,(float)(a-49)/12)/rate;
				push(t*freq*256);
				}break;

			case'_':{ // square
				a=pop();if(a==32){push(0);break;}
				float freq = 440*powf(2,(float)(a-49)/12)/rate;
				//push(((int)(t*freq*256)%256)>127?255:0);
				push(fmodf(t*freq,1)>.5?255:0);
				}break;

			case'^':{ // triangle
				a=pop();if(a==32){push(0);break;}
				float freq = 440*powf(2,(float)(a-49)/12)/rate;
				a=t*freq*256;
				if(a>127)a=255-a;
				push(a<<1);
				}break;

			case'F':{ // frequency synthesis - boi this one fucked
				a=pop();b=pop();if(b==32)break; // a is melody, b is base frequency
				float freq = 440*powf(2,(float)(b-49)/12)*2*M_PI/rate,
				res=sinf(t*freq)*127; // using range 0-127 so we can stick with ascii in melody
				int otones=melodies[a<<8];
				if(DEBUG)printf("fsyn, base %2x melody %2x - %i overtones\n",b,a,otones);
				for(int i=1;i<otones;i++)
					if(melodies[(a<<8)+i]!=32)
						res+=sinf(t*freq*(i+1))*melodies[(a<<8)+i];
				//otones++;
				otones=3;
				push(res/otones+127); // XXX normalisation gonna be a bit more complicated, but yeah
				}break;

			case'w': // noise
				//if(DEBUG)printf("noise %s\n",(char*)main);
				push(((char*)main)[t%1000]);

			case'z': // sawtooth - raw
				a=pop();push(t*256/nlen/a); // XXX formulate in terms of rate
				break;

			case'Z': // sawtooth - raw
				a=pop();push(t*256/nlen/a+127); // XXX formulate in terms of rate
				break;

			case'+': // mixer
				a=pop();res=0;
				for(int i=0;i<a;i++)res+=pop();
				push(res/a);
				break;

			case'-': // might be a good idea to ADD them instead
				a=pop();b=pop();if(b!=' ')push(b-a);else push(' ');break;

			case'g': // gate
				a=pop();b=pop();push(b>a?255:0);break;

			case'*': // fader
				a=pop();b=pop();
				push((a*b)>>8);
				break;

			case'\\': // amplifier
				a=pop();b=pop();
				push(a*b); // this dont work for some reason
				break;

			case'!':push(255-pop());break;
			case':':a=pop();push(a);push(a);break;
			case'/':a=pop();b=pop();push(a);push(b);break;

		}
		if(DEBUG){for(int i=0;i<sp;i++)printf("%2x ",stack[i]);putchar(10);}
		pp++;
	}
	if(DEBUG){a=pop();printf("%4i %2x: ",t,a);for(int i=a>>2;i;i--)putchar('#');putchar(10);}
	else putchar(pop()*.999); // this makes sure we dont emit an EOF by accident
	}

}
