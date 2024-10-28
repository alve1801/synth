#include<stdio.h>
const int inc=4;
void main(){
	for(;;){
		unsigned char a=getchar();int i=0;
		if(a==255)return;
		for(;i<a;i+=inc)putchar('#');
		for(;i<256;i+=inc)putchar('.');
		//while(a--)putchar('#');
		putchar(10);
	}
}
