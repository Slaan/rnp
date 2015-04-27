#ifndef RNP_STRING_OPR_H
#define RNP_STRING_OPR_H

#include <stdbool.h>

/**
 * Returns true when str size is null.
 */
bool is_empty_string(char* str);

/**
 * Returns pointer on first found space.
 * When no space found 0 will returned.
 */
char* find_next_space(char* str);

/**
 * Returns wordlength between given pointer till next
 * space.
 */
int size_to_next_space(char* str);

/**
 * Return true when last character before 0x00 is
 * newline.
 */
bool ends_with_newline(char* str);

/**
 * Return true when more then one '\n' is located in string
 * and false, when 1 >= amount of '\n.
 */
bool has_more_then_one_newline(char* str);

bool cmd_matches(char* cmd, char* str, int length);

void lowercase(char* str);

void uppercase(char* str);

void reverse(char* str);

/**
 * Shifts all characters once left.
 */
void shift_str_left(char* str, int amount);

/**
 * Shift whole word at end. 
 */
void shift_word_left(char* str);

/**
 * Shifts word with space at end. When there is no
 * space after word, it won't be skipped.
 */
void shift_word_left_with_space(char* str);

/**
 * Appends to the beginning of the string the
 * string "OK ".
 * An bufferoverflow is possible.
 */
void append_to_ok(char* str);

#endif // RNP_STRING_OPR_H
