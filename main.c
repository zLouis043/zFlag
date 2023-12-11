#include <stdio.h>

#define ZFLAGS_IMPLEMENTATION
#include "src/zflags.h"

int main(int argc, char *argv[]){
   
   size_t * flag_s = zflag_size("size", "decide size", 1024);
   bool * flag_b = zflag_bool("bool", "enable bool", false);

   if (!zflag_parse(argc, argv)) {
        exit(1);
   }

   zflag_print_flags(stdout);

   if(*flag_b){
      printf("Hello\n");
   } 

   if(*flag_s == 2048){
      printf("ciao\n");
   } 

   return 0; 
}
