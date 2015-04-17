#include <stdio.h>
#include <string.h>
#include <strings.h>

void reverse(char* to_reverse, char* to_store, int length) {
  for (int i = 0; i < length; i++) {
    to_store[i] = to_reverse[length-i];
  }
}

void lowercase(char* to_lowercase, char* to_store, int length) {

}

void uppercase(char* to_uppercase, char* to_store, int length) {

}

void bye() {

}

void shutdown(char* password) {

}

int main() {
  char str[10];
  reverse("abcde.fghij", str, 11);
  printf("%s", str);
}
