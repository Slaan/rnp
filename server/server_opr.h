#ifndef RNP_SERVER_OPR_H
#define RNP_SERVER_OPR_H

#include <stdbool.h>

#include "string_opr.h"

#define LOWERCASE_CMD "LOWERCASE "
#define UPPERCASE_CMD "UPPERCASE "
#define REVERSE_CMD   "REVERSE "

/**
 * Ensure given protocol.
 * Return true when str is valid to protocol and
 * false, when it isn't.
 *
 * When false returned,
 * the string str is pointing to will be modified.
 */
bool ensure_protocol(char* str);

bool cmd_lowercase(char* string);

bool cmd_uppercase(char* str);

bool cmd_reverse(char* str);

bool cmd_bye(char* str);

bool cmd_shutdown(char* str);

bool match_protocol(char* str);

#endif // RNP_SERVER_OPR_H
