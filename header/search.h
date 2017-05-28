#ifndef SEARCH_H
#define SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include "dataStructures.h"
#include "globals.h"
#include "mInodeUtil.h"

int search(int *dev, int searchnode);
int searh_block(int iblock);
int search_indirect(int *iblock_array);
int search_double_indirect(int *iblock_array, int *dev);
void tokenize_path(char *input);
void tokenize_cmd(char *input, char *tpath[]);
char *get_user_input(void);
int get_str_length(char *input);
int kgetino(int *dev);
void kpathnamehelper(char const *pathname);

#endif