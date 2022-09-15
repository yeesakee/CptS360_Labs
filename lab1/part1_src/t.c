#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
// please put your main and code in this file. All contained.
// Use the provided mk script to compile your code. You are welcome to change the mk script if you'd like
// but know that is how we will be batch compiling code using mk on the back end so your code will need to be able
// to run when that bash script is ran.
// Most code can be grabbed from the .html file in the root.

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10; 

int prints(char* s) {
  while(*s != '\0') {
    putchar(*s++);
  }
}

int printx_helper(u32 x) {
  char c;
  if (x) {
    c = ctable[x % 16];
    printx_helper(x / 16);
    putchar(c);
  }
}

int printx(u32 x) {
  if (x == 0) {
    putchar('0');
  }
  else {
    printx_helper(x);
  }
}

int printd_helper(int x) {
  if (x) {
    char c;
    if (x < 0) {
      putchar('-');
      x = -1 * x;
    }
    c = ctable[x % BASE];
    printd_helper(x / BASE);
    putchar(c);
  }
}

int printd(int x) {
  if (x == 0) {
    putchar('0');
  }
  else {
    printd_helper(x);
  }
}

int rpu(u32 x)
{  
    char c;
    if (x){
       c = ctable[x % BASE];   
       rpu(x / BASE);
       putchar(c);           
    }
}

int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

int myprintf(char* fmt, ...) {
  va_list list;
  va_start(list, fmt);
  unsigned int temp;
  u32 x;
  char* str;
  while (*fmt != '\0') {
    if (*fmt != '%') {
      putchar(*fmt);
    }
    else {
      fmt++;
      switch(*fmt) {
        case 'c': temp = va_arg(list, unsigned int);
                  putchar(temp);
                  break;
        case 's': str = va_arg(list, char*);
                  prints(str);
                  break;
        case 'u': x = va_arg(list, unsigned int);
                  printu(x);
                  break;
        case 'x': x = va_arg(list, u32);
                  printx(x);
                  break;
        case 'd': temp = va_arg(list, unsigned int);
                  printd(temp);
                  break;
      }
    }
    fmt++;
  }
}

int main(int argc, char *argv[ ], char *env[ ])
{
  printf("enter main\n");
  prints("this is a test string");
  printf("\ntest printx: ");
  printx(1);
  printf("\ntest printd: ");
  printd(9999);
  printf("\ntest printf: ");
  myprintf("char=%c string=%s       dec=%u  hex=%x neg=%d\n", 
	        'A', "this is a test",  100,   100,   -100);
  myprintf("argc: %d\n", argc);
  for (int i = 0; i < argc; i++) {
    myprintf("argv[%d]=%s \n", i, argv[i]);
  }
  for (int i = 0; i < argc; i++) {
    myprintf("env[%d]=%s \n", i, env[i]);
  }
  printf("exit main\n");
}