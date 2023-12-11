#ifndef ZFLAGS_H_
#define ZFLAGS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>  
#include <string.h>  
#include <stdint.h>

typedef union{
    bool as_bool;
    char * as_str;
    char as_char;
    uint64_t as_uint64;
    size_t as_size;
}ZF_Val;

typedef enum {
    F_BOOL,
    F_STRING,
    F_CHAR,
    F_UINT64,
    F_SIZE,
    F_COUNT
}ZF_Type;

typedef struct {

    const char * name;
    const char * description;

    ZF_Val in_val;
    ZF_Val def_val;

    ZF_Type type;

}zFlag;

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif

typedef struct {

    zFlag flags[FLAGS_CAP];
    size_t flags_count;

    int argc;
    char **argv;

}ZF_Context;

static ZF_Context g_zflag_context;

static char * zflag_shift_args(int * argc, char *** argv);
zFlag * zflag_new(ZF_Type type, const char* name, const char * description);
bool * zflag_bool(const char* name, const char * description, bool default_value);
char ** zflag_str(const char* name, const char * description, char * default_value);
char * zflag_char(const char* name, const char * description, char  default_value);
uint64_t * zflag_uint64(const char* name, const char * description, uint64_t  default_value);
size_t * zflag_size(const char* name, const char * description, uint64_t  default_value);
bool zflag_parse(int argc, char **argv);
void zflag_print_flags(FILE * _Stream);

#endif // ZFLAG_H_

#ifdef ZFLAGS_IMPLEMENTATION 

static char * zflag_shift_args(int * argc, char *** argv){

    assert(*argc > 0);
    char * result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;

}

zFlag * zflag_new(ZF_Type type, const char* name, const char * description){
    
    ZF_Context *c = &g_zflag_context;

    assert(c->flags_count < FLAGS_CAP);
    zFlag * flag = &c->flags[c->flags_count++];  
    memset(flag, 0, sizeof(zFlag*)); 
    flag->type = type;
    flag->name = name;
    flag->description = description;
    return flag; 

}

bool * zflag_bool(const char* name, const char * description, bool default_value){

    zFlag * flag = zflag_new(F_BOOL, name, description);
    flag->def_val.as_bool = default_value;
    flag->in_val.as_bool = default_value;
    return &flag->in_val.as_bool;

}

char ** zflag_str(const char* name, const char * description, char * default_value){
    zFlag * flag = zflag_new(F_STRING, name, description);
    flag->def_val.as_str = default_value;
    flag->in_val.as_str = default_value;
    return &flag->in_val.as_str;
}

char * zflag_char(const char* name, const char * description, char  default_value){
    zFlag * flag = zflag_new(F_CHAR, name, description);
    flag->def_val.as_char = default_value;
    flag->in_val.as_char = default_value;
    return &flag->in_val.as_char;
}

uint64_t * zflag_uint64(const char* name, const char * description, uint64_t default_value){
    zFlag * flag = zflag_new(F_UINT64, name, description);
    flag->def_val.as_uint64 = default_value;
    flag->in_val.as_uint64 = default_value;
    return &flag->in_val.as_uint64;
}

size_t * zflag_size(const char* name, const char * description, uint64_t  default_value){
    zFlag * flag = zflag_new(F_SIZE, name, description);
    flag->def_val.as_size = default_value;
    flag->in_val.as_size = default_value;
    return &flag->in_val.as_size;
}

bool zflag_parse(int argc, char **argv){

    ZF_Context *c = &g_zflag_context;

    zflag_shift_args(&argc, &argv);

    while(argc > 0){
        char * flag = zflag_shift_args(&argc, &argv);

        if(*flag != '-'){
            c->argc = argc + 1;
            c->argv = argv - 1;
            return true;
        }

        if (strcmp(flag, "--") == 0) {
            c->argc = argc;
            c->argv = argv;
            return true;
        }

        flag += 1;

        bool found = false;

        for(size_t i = 0; i < c->flags_count && !found; i++){

            if(strncmp(c->flags[i].name, flag, strlen(c->flags[i].name)) == 0){
                switch (c->flags[i].type) {
                    case F_BOOL: {;
                        c->flags[i].in_val.as_bool = true;
                    }break;
                    case F_STRING: {
                        
                        if(argc == 0){
                            fprintf(stderr, "[ERROR] no value specified for flag '%s'\n", flag);
                            return false;
                        }

                        char* arg = zflag_shift_args(&argc, &argv);

                        c->flags[i].in_val.as_str = arg;
                    }break;
                    case F_CHAR: {
                        if(argc == 0){
                            fprintf(stderr, "[ERROR] no value specified for flag '%s'\n", flag);
                            return false;
                        }

                        char * arg = zflag_shift_args(&argc, &argv);

                        c->flags[i].in_val.as_char = *arg;
                    }break;
                    case F_UINT64: {

                        if(argc == 0){
                            fprintf(stderr, "[ERROR] no value specified for flag '%s'\n", flag);
                            return false;
                        }

                        char* arg = zflag_shift_args(&argc, &argv);
                        
                        char *endptr;
                        unsigned long long int result = strtoull(arg, &endptr, 10);

                        c->flags[i].in_val.as_uint64 = result;
                    }break;
                    case F_SIZE: {

                        if(argc == 0){
                            fprintf(stderr, "[ERROR] no value specified for flag '%s'\n", flag);
                            return false;
                        }

                        char* arg = zflag_shift_args(&argc, &argv);
                        
                        char *endptr;
                        unsigned long long int result = strtoull(arg, &endptr, 10);

                        if (strcmp(endptr, "K") == 0) {
                            result *= 1024;
                        } else if (strcmp(endptr, "M") == 0) {
                            result *= 1024*1024;
                        } else if (strcmp(endptr, "G") == 0) {
                            result *= 1024*1024*1024;
                        } else if (strcmp(endptr, "") != 0) {
                            fprintf(stderr, "[ERROR] wrong prefix used for size '%s'\n", endptr);
                            return false;
                        }

                        c->flags[i].in_val.as_size = result;
                    }break;
                }
                found = true;
            }
        }

        if (!found) {
            return false;
        }

    }

    c->argc = argc;
    c->argv = argv;
    return true;
}

void zflag_print_flags(FILE* _Stream){

    ZF_Context *c = &g_zflag_context;
    for (size_t i = 0; i < c->flags_count; ++i) {
        zFlag *flag = &c->flags[i];

        fprintf(_Stream, "    -%s\n", flag->name);
        fprintf(_Stream, "        %s\n", flag->description);
        switch (c->flags[i].type) {
        case F_BOOL:
            fprintf(_Stream, "        Default: %s\n", flag->def_val.as_bool ? "true" : "false");
            break;
        case F_UINT64:
            fprintf(_Stream, "        Default: %d\n", flag->def_val.as_uint64);
            break;
        case F_SIZE:
            fprintf(_Stream, "        Default: %zu\n", flag->def_val.as_size);
            break;
        case F_STRING:
            if (flag->def_val.as_str) {
                fprintf(_Stream, "        Default: %s\n", flag->def_val.as_str);
            }
            break;
        default:
            assert(0 && "unreachable");
            exit(69);
        }
    }

}


#endif // ZFLAGS_IMPLEMENTATION