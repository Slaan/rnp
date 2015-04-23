#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>

#include "server_opr.h"

static void test_server_reverse1(void** state) {
  char client_msg[]   = "REVERSE hallo welt\n";
  char expected_msg[] = "OK tlew ollah\n";
  if (!match_protocol(client_msg)) {
    fail();
  }
  assert_string_equal(expected_msg, client_msg);
}

static void test_server_reverse2(void** state) {
  char client_msg[]   = "REVERSE H2O ist Wasser.\n";
  char expected_msg[] = "OK .ressaW tsi O2H\n";
  if (!match_protocol(client_msg)) {
    fail();
  }
  assert_string_equal(expected_msg, client_msg);
}

static void test_server_uppercase(void** state) {
  char client_msg[]   = "UPPERCASE hallo welt\n";
  char expected_msg[] = "OK HALLO WELT\n";
  if (!match_protocol(client_msg)) {
    fail();
  }
  assert_string_equal(expected_msg, client_msg);
}

static void test_server_lowercase(void** state) {
  char client_msg[]   = "LOWERCASE HaLlO Welt\n";
  char expected_msg[] = "OK hallo welt\n";
  if (!match_protocol(client_msg)) {
    fail();
  }
  assert_string_equal(expected_msg, client_msg);
}

static void test_reverse(void** state) {
  char text[] = "hallo";
  char expected[] = "ollah";
  reverse(text);
  assert_string_equal(text, expected);
}

int main() {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_reverse),
    cmocka_unit_test(test_server_reverse1),
    cmocka_unit_test(test_server_reverse2),
    cmocka_unit_test(test_server_uppercase),
    cmocka_unit_test(test_server_lowercase)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
