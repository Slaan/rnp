#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#include "server_opr.h"
#include "string_opr.h"


bool ensure_protocol(char* instr) {
  if (!ends_with_newline(instr)) {
    return false;
  }
  return true;
}
 
bool cmd_lowercase(char* str) {
  int first_word_length = size_to_next_space(str);
  if (strncmp(LOWERCASE_CMD, str, first_word_length) != 0) {
    return false;
  }
  shift_word_left(str);
  lowercase(str);
  return true;
}

bool cmd_uppercase(char* str) {
  int first_word_length = size_to_next_space(str);
  if (strncmp(UPPERCASE_CMD, str, first_word_length) != 0) {
    return false;
  }
  shift_word_left(str);
  uppercase(str);
  return true;
}

bool cmd_reverse(char* str) {
  int first_word_length = size_to_next_space(str);
  if (strncmp(REVERSE_CMD, str, first_word_length) != 0) {
    return false;
  }
  shift_word_left_with_space(str);
  int size = strlen(str);
  str[size-1] = 0x00;
  str[size]   = 0x00;
  reverse(str);
  str[size-1] = 0x0A;
  str[size]   = 0x00;
  return true;
}

bool match_protocol(char* str) {
  if (!cmd_lowercase(str) 
   && !cmd_uppercase(str)
   && !cmd_reverse(str)) {
    return false;
   }
  append_to_ok(str);
  return true;
}
