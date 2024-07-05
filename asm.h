//
// Created by אילון אהרוני on 28/06/2024.
//
#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 500
#define MAX_LABELS 250
#define MAX_LABEL 50
#define MAX_FIELD_LENGTH 10


typedef struct label{
    int line_num;
    char label_name [MAX_LABEL];
    struct label* next;
}Label;
Label* create_label(char* label_name, int line_num);
Label* add_first(Label* curr, char* lable_name, int line_num);
void printLabelList(Label* head);
int find_label(Label* head, char* label);
void free_list(Label* head);
void parse_line(const char *line, char *optcode, char *rd, char *rs, char *rt, int *imm, int *imm_flag, char *memin_line,Label* labelList);
int transOptcode(char *optcode);
void write_default_lines(FILE *file, long start, long end);
void process_word_command(const char *command, FILE *file);
void translate_register(char *reg_name, char *memin_line);
Label* first_run(FILE *file);





//        void new_label(char label_name [MAX_LABEL], int line_count);//updates the data of the label if found a new one
int find_label_address (char* line, char label_name [MAX_LABEL]);//saves the address to temp address
int print_to_text_file(FILE* memin , int opt_code , int rd , int rs , int rt , int imm ,int imm_flag);

#endif /* ASM_H */

