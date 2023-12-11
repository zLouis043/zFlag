#include <stdio.h>

#define ZFLAGS_IMPLEMENTATION
#include "src/zflags.h"

void print_argv(int argc, char **argv){
   printf("[INFO] Command line: ");
   for(size_t i = 0; i < argc; i++){
      printf("%s ", argv[i]);
   }
   printf("\n");
}

int main(int argc, char *argv[]){
   
   print_argv(argc, argv);

   bool * flag_b = zflag_bool("bool", "Enable bool flag", false);
   char** flag_str = zflag_str("str", "Define a string flag", "string");
   char * flag_c = zflag_char("char", "Define a char flag", 'c');
   size_t * flag_u = zflag_uint64("uint64_t", "Pick a number just as an example", 1024);
   size_t * flag_s = zflag_size("size", "Pick a size just as an example", 1024);

   if (!zflag_parse(argc, argv)) {
        exit(1);
   }

   zflag_print_flags(stdout);

   if(*flag_b){
      printf("[INFO] FLAG_B ENABLED\n");
   } 

   if(strcmp(*flag_str, "check") == 0){
      printf("[INFO] FLAG_STR MATCH\n");
   } 

   if(*flag_c == 'd'){
      printf("[INFO] FLAG_C MATCH\n");
   } 

   if(*flag_u == 2048){
      printf("[INFO] FLAG_U MATCH\n");
   }

   if(*flag_s == 2048){
      printf("[INFO] FLAG_S MATCH\n");
   } 

   return 0; 
}
