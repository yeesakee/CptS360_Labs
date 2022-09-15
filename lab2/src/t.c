#include <stdio.h>              // for I/O
#include <stdlib.h>             // for exit()
#include <libgen.h>             // for dirname()/basename()
#include <string.h>             // for string functions

typedef struct node{
        char  name[64];         // node's name string
        char  type;             // type = 'D' or 'F'  
   struct node *child, *sibling, *parent;
}NODE;


NODE *root, *cwd, *start;       // root->/, cwd->CWD, start->start_node
char line[128];                 // user input line
char command[16], pathname[64]; // command pathname strings

// you need these for dividing pathname into token strings
char gpath[128];                // token string holder
char *name[32];                 // token string pointers
int  n;                         // number of token strings

//               0         1      2     3     4        5       6     7       8
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "menu", "quit", 0};

int findCmd(char *command)
{
   int i = 0;
   while(cmd[i]){
     if (strcmp(command, cmd[i])==0)
         return i;
     i++;
   }
   return -1;
}

NODE *search_child(NODE *parent, char *name, char type)
{
  NODE *p;
  printf("search for %s in parent DIR %s\n", name, parent->name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p){
    printf("p: %s, type: %c\n", p->name, p->type);
    if (strcmp(p->name, name)==0 && (p->type == type))
      return p;
    p = p->sibling;
  }
  return 0;
}

int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into parent's child list\n", q->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else{
    while(p->sibling)
      p = p->sibling;
    
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

int remove_child(NODE* parent, NODE* q) {
  NODE *p;
  printf("removing NODE %s from parent's child list\n", q->name);
  p = parent->child;
  printf("p: %s\nq: %s\n", p->name, q->name);

  if (!strcmp(p->name, q->name) && (p->type == q->type)) {
    parent->child = p->sibling;
    printf("successfully removed NODE %s from parent's child list\n", q->name);
    free(q);
    return 0;
  }
  NODE* prev = NULL;
  while (p) {
    if (!strcmp(p->name, q->name) && (p->type == q->type)) {
      if (p->sibling && prev) {
        prev->sibling = p->sibling;
      }
      else if (p->sibling && prev == NULL){
        p = p->sibling;
      }
      else {
        prev->sibling = NULL;
      }
      printf("successfully removed NODE %s from parent's child list\n", q->name);
      free(q);
      return 0;
    }
    prev = p;
    p = p->sibling;
  }
  printf("unsuccessful in removing NODE %s from parent's child list\n", q->name);
  return -1;
}

int tokenize(char *pathname) {
  int i;
  char* s;
  strcpy(gpath, pathname);
  printf("gpath=%s\n", gpath);

  n = 0;
  s = strtok(gpath, "/");
  while(s) {
    name[n++] = s;
    s = strtok(0, "/");
  }

  for (i = 0; i < n; i++) {
    printf("name[%d]=%s\n", i, name[i]);
  }
  printf("gpath=%s\n", gpath);
  return n;
}

NODE* path2node(char* pathname, char type) {
  NODE* p = NULL;
  char temp[256];
  strcpy(temp, pathname);
  if (pathname[0] == '/') {
    start = root;
  }
  else {
    start = cwd;
  }
  p = start;
  if ((pathname[0] == '.' && strlen(pathname) == 1) || (pathname[0] == '/' && strlen(pathname) == 1)) {
    return p;
  }
  int count = tokenize(temp);
  for (int i = 0; i < count; i++) {
    printf("%s\n", name[i]);
    if (strcmp(name[i], "..") == 0 && p != NULL) {
      printf("going to parent\n");
      p = p->parent;
    }
    else {
      if (i != (count - 1)) {
        p = search_child(p, name[i], 'D');
      }
      else {
        p = search_child(p, name[i], type);
      }
    }
    if (p == 0) {
      printf("child not found\n");
      return 0;
    }
  }
  return p;
}


/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

int mkdir(char *pathname)
{
  NODE *p, *q;
  char temp[256];
  strcpy(temp, pathname);

  printf("mkdir: name=%s\n", pathname);

  if (!strcmp(pathname, "/") || !strcmp(pathname, ".") || !strcmp(pathname, "..") || !strcmp(pathname, "")){
    printf("can't mkdir with %s\n", pathname);
    return -1;
  }
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  printf("check whether %s already exists\n", pathname);
  // check if given directory already exists
  p = path2node(temp, 'D');
  strcpy(temp, pathname);
  // if p then node of given directory exists, DO NOT mkdir
  if (p){
    printf("path %s already exists, mkdir FAILED\n", pathname);
    return -1;
  }
  // store dirname of given directory
  char* dir = dirname(temp);
  // check if given directory's parent path exists
  p = path2node(dir, 'D');
  // if directory's parent path does not exist, DO NOT mkdir
  if (!p){
    printf("path %s does not exist, mkdir FAILED\n", dir);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", pathname);
  // get the name of the new directory and store it in q->name
  strcpy(temp, pathname);
  char* basedir = basename(temp);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, basedir);
  // insert the new directory to p (the parent directory)
  insert_child(p, q);
  printf("mkdir %s OK\n", pathname);
  printf("--------------------------------------\n");
    
  return 0;
}

int rmdir(char* pathname) {
  NODE *p, *q;
  char temp[256];
  strcpy(temp, pathname);

  printf("rmdir: name=%s\n", pathname);

  if (!strcmp(pathname, "/") || !strcmp(pathname, ".") || !strcmp(pathname, "..") || !strcmp(pathname, "")){
    printf("can't rmdir with %s\n", pathname);
    return -1;
  }
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  printf("check whether %s exists\n", pathname);
  // check if given directory already exists
  p = path2node(temp, 'D');
  // if p then node of given directory does not exist, DO NOT rmdir
  if (!p){
    printf("path %s does not exist, rmdir FAILED\n", pathname);
    return -1;
  }
  if (p->child) {
    printf("directory %s is not empty, rmdir FAILED\n", pathname);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to rmdir %s\n", pathname);
  q = p;
  p = p->parent;
  strcpy(temp, pathname);
  remove_child(p, q);
  printf("rmdir %s OK\n", pathname);
  printf("--------------------------------------\n");
    
  return 0;
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char* pathname)
{
  NODE *p;
  char temp[256];
  strcpy(temp, pathname);
  // if pathname starts with '/' then find path through absolute path
  if (temp[0]=='/')
    start = root;
  else
    start = cwd;
  // if pathname is not empty then search directory using path2node
  if (strcmp(pathname, "") != 0) {
    p = path2node(temp, 'D');
    if (!p) {
      printf("path %s does not exist, ls FAILED\n", pathname);
    }
  }
  // if pathname is empty then p = start
  else {
    printf("pathname is empty\n");
    p = start;
  }
  printf("cwd contents = ");
  if (p) {
    p = p->child;
  }
  while(p){
    printf("[%c %s] ", p->type, p->name);
    p = p->sibling;
  }
  printf("\n");
}

int cd (char* pathname) {
  NODE* p;
  char temp[256];
  if (strcmp(pathname, "..") == 0) {
    if (strcmp(cwd->name, "/") != 0) {
      cwd = cwd->parent;
      printf("successfully changed directory to previous directory\n");
      return 0;
    }
    else {
      printf("uncessful in changing directory, already in root directory\n");
      return -1;
    }
  }
  if (strcmp(pathname, "") == 0) {
    cwd = root;
    printf("successfully changed directory to root\n");
    return 0;
  }
  strcpy(temp, pathname);
  printf("searching path: %s\n", temp);
  p = path2node(temp, 'D');
  if (p) {
    cwd = p;
    printf("successfully changed directory to %s\n", pathname);
    return 0;
  }
  printf("uncessful in changing directory to %s\n", pathname);
  return -1;
}

int pwd_helper (NODE* p) {
  if (p && strcmp(p->name, "/")) {
    pwd_helper(p->parent);
    printf("/%s", p->name);
  }
  else if (p) {
    printf("pwd: ");
  }
  return 0;
}

int pwd () {
  if (cwd == root) {
    printf("pwd: /");
  }
  else {
    pwd_helper(cwd);
  }
  printf("\n");
  return 0;
}

int creat(char* pathname) {
  NODE *p, *q;
  char temp[256];
  char base[256];
  strcpy(temp, pathname);
  strcpy(base, basename(temp));
  printf("creat: name=%s\n", base);
  strcpy(temp, pathname);

  if (!strcmp(pathname, "/") || !strcmp(pathname, ".") || !strcmp(pathname, "..") || !strcmp(pathname, "")){
    printf("can't creat %s\n", pathname);
    return -1;
  }
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  printf("check whether %s already exists\n", pathname);
  // check if given file already exists
  p = path2node(temp, 'F');
  strcpy(temp, pathname);
  // if p then node of given directory exists, DO NOT mkdir
  if (p){
    printf("file %s already exists, creat FAILED\n", pathname);
    return -1;
  }
  // store dirname of given directory
  char* dir = dirname(temp);
  // check if given directory's parent path exists
  p = path2node(dir, 'D');
  // if directory's parent path does not exist, DO NOT mkdir
  if (!p){
    printf("path %s does not exist, creat FAILED\n", dir);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to creat %s\n", pathname);
  // get the name of the new directory and store it in q->name
  strcpy(temp, pathname);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'F';
  strcpy(q->name, base);
  // insert the new directory to p (the parent directory)
  insert_child(p, q);
  printf("creat %s OK\n", pathname);
  printf("--------------------------------------\n");
    
  return 0;
}

int rm(char* pathname) {
  NODE *p, *q;
  char temp[256];
  char base[256];
  strcpy(temp, pathname);
  strcpy(base, basename(temp));
  strcpy(temp, pathname);
  printf("rm: name=%s\n", pathname);

  if (!strcmp(pathname, "/") || !strcmp(pathname, ".") || !strcmp(pathname, "..") || !strcmp(pathname, "")){
    printf("can't rm with %s\n", pathname);
    return -1;
  }
  if (pathname[0]=='/')
    start = root;
  else
    start = cwd;

  printf("check whether %s exists\n", pathname);
  // check if given file exists
  p = path2node(temp, 'F');
  // if p then node of given directory does not exist, DO NOT rmdir
  if (!p){
    printf("file %s does not exist, rm FAILED\n", base);
    return -1;
  }

  printf("--------------------------------------\n");
  printf("ready to rm %s\n", pathname);
  p = p->parent;
  strcpy(temp, pathname);
  q = path2node(temp, 'F');
  remove_child(p, q);
  printf("rm %s OK\n", pathname);
  printf("--------------------------------------\n");
    
  return 0;
}

int menu() {
  printf("\nmkdir pathname :make a new directory for a given pathname\n");
  printf("rmdir pathname :remove the directory, if it is empty.\n");
  printf("cd [pathname] :change CWD to pathname, or to / if no pathname.\n");
  printf("ls [pathname] :list the directory contents of pathname or CWD\n");
  printf("pwd :print the (absolute) pathname of CWD\n");
  printf("creat pathname :create a FILE node.\n");
  printf("rm pathname :remove the FILE node.\n");
  //printf("save filename :save the current file system tree as a file\n");
  //printf("reload filename :construct a file system tree from a file\n");
  printf("menu : show a menu of valid commands\n");
  printf("quit : save the file system tree, then terminate the program.\n\n");
  return 0;
}

int quit()
{
  printf("Program exit\n");
  exit(0);
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}

int initialize()
{
    root = (NODE *)malloc(sizeof(NODE));
    strcpy(root->name, "/");
    root->parent = root;
    root->sibling = 0;
    root->child = 0;
    root->type = 'D';
    cwd = root;
    printf("Root initialized OK\n");
}


int main()
{
  int index;
  initialize();

  while(1){
     printf("Enter command line [mkdir|rmdir|ls|cd|pwd|creat|rm|menu|quit] : ");
     fgets(line, 128, stdin);     // get input line
     line[strlen(line)-1] = 0;    // kill \n at end
     
     sscanf(line, "%s %s", command, pathname); // extract command pathname
     printf("command=%s pathname=%s\n", command, pathname);
      
     if (command[0]==0) 
         continue;

     index = findCmd(command);

     switch (index){
        case 0: mkdir(pathname); break;
        case 1: rmdir(pathname); break;
        case 2: ls(pathname);    break;
        case 3: cd(pathname);    break;
        case 4: pwd();           break;
        case 5: creat(pathname); break;
        case 6: rm(pathname);    break;
        case 7: menu();          break;
        case 8: quit();          break;
     }
     memset(pathname, 0, 64);
  }
}
