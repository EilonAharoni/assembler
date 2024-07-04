#include "asm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LINE_LENGTH 6  // 5 hex characters + 1 newline
#define DEFAULT_LINE "00000\n"
void parse_line(const char *line, char *optcode, char *rd, char *rs, char *rt, int *imm, int *imm_flag, char *memin_line,Label* labelList) {
    char line_copy[MAX_LINE_LENGTH]; // Assuming MAX_LINE_LENGTH is defined in asm.h
    strcpy(line_copy, line); // Copy line to line_copy
    char *field;
    int field_count = 0;

    // Check for and skip label if present
    char *colon_pos = strchr(line_copy, ':');
    if (colon_pos) {
        // Move past the colon and any leading whitespace
        colon_pos++; // Move past the colon
        while (isspace((unsigned char)*colon_pos)) {
            colon_pos++; // Skip whitespace
        }
        strcpy(line_copy, colon_pos); // Copy the rest of the line after the label
    }

    // Trim leading and trailing spaces from line_copy
    char trimmed_line[MAX_LINE_LENGTH];
    int i = 0, j = 0;
    while (isspace((unsigned char)line_copy[i])) i++;
    while (line_copy[i]) {
        trimmed_line[j++] = line_copy[i++];
    }
    trimmed_line[j] = '\0';
    strcpy(line_copy, trimmed_line);

    // If line_copy is empty after trimming, return without parsing
    if (line_copy[0] == '\0') {
        return;
    }

    // Replace tabs with spaces
    for (i = 0; line_copy[i]; i++) {
        if (line_copy[i] == '\t') {
            line_copy[i] = ' ';
        }
    }

    // Split the line by space and comma
    field = strtok(line_copy, " ,");
    while (field && field_count < 5) {
        // Trim leading and trailing spaces
        while (isspace((unsigned char)*field)) field++;
        char *end = field + strlen(field) - 1;
        while (end > field && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';
        switch (field_count) {
            case 0:
                strcpy(optcode, field);
                break;
            case 1:
                strcpy(rd, field);
                break;
            case 2:
                strcpy(rs, field);
                break;
            case 3:
                strcpy(rt, field);
                break;
            case 4:

                *imm = find_label(labelList,field);
                    if (*imm !=-1)
                        break;

//                if(strncmp("0x", field, 2) == 0)
//                    *imm = (int)strtol(field,NULL,16);
//                else
                    *imm = (int)strtol(field,NULL,0);

                *imm_flag = 1;

                break;
        }

        field_count++;
        field = strtok(NULL, " ,");
    }

    // Translate optcode to hex
    int opcode_hex = transOptcode(optcode);
    if (opcode_hex != -1) {
        snprintf(memin_line, 3, "%02X", opcode_hex); // Store first two characters in memin_line
    } else {
        strcpy(memin_line, "XX");  // Invalid opcode
        return;
    }

    // Translate registers to hex and store in memin_line
    char *registers[3] = {rd, rs, rt};
    for (int i = 0; i < 3; ++i) {
        translate_register(registers[i], memin_line + 2 + i); // Offset by 2 for opcode, then by i for each register
    }
}

Label* first_run(FILE *file) {
    char line[MAX_LINE_LENGTH];
    char* labelName = NULL;
    Label * Label_List = create_label("List",0);
    Label * currLabel = Label_List;
    int PC = 0;
    int isLabel;// flag foe label
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        isLabel =0;

        // Check if line starts with a letter and ends with ":"
        char *colon_pos = strchr(line, ':');
        if (colon_pos) {
            isLabel =1;
            // Extract label name
            *colon_pos = '\0'; // Replace ':' with null terminator to isolate label name

            // Allocate memory for label name
//            char *label_name = (char *)malloc(strlen(line) + 1);
//            if (label_name == NULL) {
//                perror("Memory allocation failed");
//                exit(1);
//            }
// Create new label
            labelName =  strdup(line); //allocate temp name for the label
            if(!labelName)
            {
                printf("Allocation Error\n");
                return NULL;
            }
            currLabel = add_first(currLabel, labelName, PC);
            if(!currLabel)
            {
                free_list(Label_List);
                printf("Allocation error\n");
                return NULL;
            }
            free(labelName); // free the temp name




            // Print label information (for demonstration)
            printf("Label: %s at line: %d\n", currLabel->label_name, currLabel->line_num);

            // Clean up allocated memory if needed (not shown in this snippet)
            // Remember to free(new_label->label_name) and free(new_label) later
        }
        if(isLabel)
            continue; //don't acres the pc
        // Check for "imm" in the line
        if (strstr(line, "imm") != NULL) {
            PC += 2;
        } else {
            PC++;
        }
    }
    return Label_List; //returns the head of the list. his next is the first label
}
Label* create_label(char* label_name, int line_num)
{
    Label* new_label = (Label*)malloc(sizeof(Label));
    if (new_label != NULL)
    {
        strcpy(new_label->label_name, label_name);
        new_label->line_num = line_num;
        new_label->next = NULL;
    }
    return new_label;
}
Label* add_first(Label* curr, char* lable_name, int line_num) // add a lable to the list
{
    Label* new_label = create_label(lable_name, line_num);
    if (new_label != NULL)
        curr->next = new_label;
    return new_label;
}
int find_label(Label* head, char* label) {
    while (head != NULL && strcmp(head->label_name, label) != 0)
        head = head->next;
    if (head == NULL) // lable is not found
        return -1;
    return head->line_num;
}
void printLabelList(Label* head)
{
    if(!head)//if the list is empty
        return;

    Label * currLabel = head->next;
    while (currLabel)
    {
        printf("Label name: %s Label line number: %d\n ",currLabel->label_name,currLabel->line_num);
        currLabel = currLabel->next;
    }


}
void free_list(Label* head)//free all the memory in th list
{
    Label * curr = head;
    Label * temp = NULL;
    while (curr)
    {
        temp = curr->next;
        free(curr);
        curr = temp;
    }

//    if (head == NULL)
//        return;
//    free_list(head->next);
//    free(head);
}

int transOptcode(char *optcode) {
    if (strcmp(optcode, "add") == 0) return 0x00;
    if (strcmp(optcode, "sub") == 0) return 0x01;
    if (strcmp(optcode, "mul") == 0) return 0x02;
    if (strcmp(optcode, "and") == 0) return 0x03;
    if (strcmp(optcode, "or") == 0) return 0x04;
    if (strcmp(optcode, "xor") == 0) return 0x05;
    if (strcmp(optcode, "sll") == 0) return 0x06;
    if (strcmp(optcode, "sra") == 0) return 0x07;
    if (strcmp(optcode, "srl") == 0) return 0x08;
    if (strcmp(optcode, "beq") == 0) return 0x09;
    if (strcmp(optcode, "bne") == 0) return 0x0A;
    if (strcmp(optcode, "blt") == 0) return 0x0B;
    if (strcmp(optcode, "bgt") == 0) return 0x0C;
    if (strcmp(optcode, "ble") == 0) return 0x0D;
    if (strcmp(optcode, "bge") == 0) return 0x0E;
    if (strcmp(optcode, "jal") == 0) return 0x0F;
    if (strcmp(optcode, "lw") == 0) return 0x10;
    if (strcmp(optcode, "sw") == 0) return 0x11;
    if (strcmp(optcode, "reti") == 0) return 0x12;
    if (strcmp(optcode, "in") == 0) return 0x13;
    if (strcmp(optcode, "out") == 0) return 0x14;
    if (strcmp(optcode, "halt") == 0) return 0x15;
    return -1; // Invalid opcode
}

void translate_register(char *reg_name, char *memin_line) {
    if (strcmp(reg_name, "$zero") == 0) *memin_line = '0';
    else if (strcmp(reg_name, "$imm") == 0) *memin_line = '1';
    else if (strcmp(reg_name, "$v0") == 0) *memin_line = '2';
    else if (strcmp(reg_name, "$a0") == 0) *memin_line = '3';
    else if (strcmp(reg_name, "$a2") == 0) *memin_line = '5';
    else if (strcmp(reg_name, "$a3") == 0) *memin_line = '6';
    else if (strcmp(reg_name, "$t0") == 0) *memin_line = '7';
    else if (strcmp(reg_name, "$t1") == 0) *memin_line = '8';
    else if (strcmp(reg_name, "$t2") == 0) *memin_line = '9';
    else if (strcmp(reg_name, "$s0") == 0) *memin_line = 'A';
    else if (strcmp(reg_name, "$s1") == 0) *memin_line = 'B';
    else if (strcmp(reg_name, "$s2") == 0) *memin_line = 'C';
    else if (strcmp(reg_name, "$gp") == 0) *memin_line = 'D';
    else if (strcmp(reg_name, "$sp") == 0) *memin_line = 'E';
    else if (strcmp(reg_name, "$ra") == 0) *memin_line = 'F';
    else *memin_line = '0'; // Default to zero for unknown registers
}




int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fRead = fopen(argv[1], "r");
    if (fRead == NULL) {
        perror("Error opening file");
        return 1;
    }

    FILE * fWrite= fopen(argv[2], "w");
    if (fWrite == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char optcode[MAX_FIELD_LENGTH];
    char rd[MAX_FIELD_LENGTH];
    char rs[MAX_FIELD_LENGTH];
    char rt[MAX_FIELD_LENGTH];
    int imm;
    int imm_flag = 0;
    char memin_line[6]; // Array to hold "2 hex digits" for opcode, rd, rs, rt, and an extra char for '\0'
    unsigned int masked_imm;
//    Label *labels[MAX_LABELS]; // Array of pointers to labels
    int label_count = 0;

    // First pass to collect labels and update PC
    Label* labelList = first_run(fRead);

    // Reset file pointer to beginning of file
    fseek(fRead, 0, SEEK_SET);

    // Second pass to parse and process instructions
    while (fgets(line, sizeof(line), fRead)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Check for and skip label if present
        char *colon_pos = strchr(line, ':');
        if (colon_pos) {
            // Move past the colon and any leading whitespace
            colon_pos++; // Move past the colon
            while (isspace((unsigned char)*colon_pos)) {
                colon_pos++; // Skip whitespace
            }
            strcpy(line, colon_pos); // Copy the rest of the line after the label
        }
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        // Process each line
        parse_line(line, optcode, rd, rs, rt, &imm, &imm_flag, memin_line,labelList);
//        if(strcmp(memin_line,"XX") == 0 ){//if it is a word. command
//            process_word_command(line,fWrite);
//        }


        // Print the parsed data (for demonstration)
        printf("Original line: %s\n", line);
        printf("Parsed: optcode=%s, rd=%s, rs=%s, rt=%s, imm=%d, imm_flag=%d\n", optcode, rd, rs, rt, imm, imm_flag);
        printf("memin_line: %s\n", memin_line);
        if(!fprintf(fWrite,"%s\n",memin_line))
            perror("Error in write to file\n");

        if (strstr(line, "imm") != NULL)
        {
            masked_imm = imm & 0xFFFFF; // Mask to get the lower 5 hexadecimal digits
            printf("Address: %05x\n", masked_imm);
            fprintf(fWrite,"%05x\n",masked_imm);   //write to memin.txt file

        }







    }

    // Close file
    fclose(fRead);
    fclose(fWrite);



    free_list(labelList);

    return 0;
}