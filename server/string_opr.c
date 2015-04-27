#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "string_opr.h"

bool is_empty_string(char* str) {
  return strlen(str) == 0;
}

char* find_next_space(char* str) {
  for ( ; *str; str++) {
    if (*str == 0x20 || *str == 0x00) {
      return str;
    }
  }
}

int size_to_next_space(char* str) {
  return find_next_space(str) - str;
}

bool ends_with_newline(char* str) {
  int length = strlen(str)-1;
  if (str[length] == '\n') {
    return true;
  }
  return false;
}

bool has_more_then_one_newline(char* str) {
  int i = 0;
  for ( ; *str; ++str) {
    if (*str == 0x20) 
      i++;
  }
  if (i > 1)
    return true;
  return false;
}

bool cmd_matches(char* cmd, char* str, int length) {
  return strncmp(cmd, str, length-1) != 0;
}

void lowercase(char* str) {
  for ( ; *str; ++str) *str = tolower(*str);
}

void uppercase(char* str) {
  for ( ; *str; ++str) *str = toupper(*str);
}

void reverse(char* str) {
  // Subtract two, because of every EOF
  // and every newline!
  int length = strlen(str);
  char cpy[length+1]; 
  strcpy(cpy, str);
  for (int i = 0; i < length; i++) {
    str[i] = cpy[length-i-1];
  }
}

void shift_str_empty(char* str) {
  bzero(str, sizeof(str));
}

void shift_str_left(char* str, int amount) {
  str += amount;
  for ( ; *str; ++str) {
    *(str - amount) = *str;
  }
  // eof copy
  *(str - amount) = *str;
}

void shift_word_left(char* str) {
  char* next_space = find_next_space(str);
  if (next_space == 0) {
    shift_str_empty(str);
    return;
  }
  shift_str_left(str, next_space - str);
}

void shift_word_left_with_space(char* str) {
  char* next_space = find_next_space(str);
  if (next_space == 0 || next_space + 1 == 0) {
    shift_str_empty(str);
    return;
  }
  shift_str_left(str, (next_space - str) + 1);
}

/**
 * Bufferoverflow possible!
 */
void append_to_ok(char* str) {
  int with_space = str[0] == ' ' ? 0 : 1;
  for (int i = strlen(str); with_space + i > 0; i--) {
    str[i + 2 + with_space] = str[i];
  }
  str[0] = 'O'; 
  str[1] = 'K'; 
  str[2] = ' '; 
}
