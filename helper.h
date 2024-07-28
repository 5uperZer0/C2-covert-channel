#ifndef HELPER_H
#define HELPER_H

void rem_to_ascii(int rem[], char ascii_text[]);
long long gcd (long long a, long long b);
long long mod_inv(long long rem, long long modulus);
long long CRT(int rem[], int moduli[]);
void txt_to_ascii_rem(const char* text, int rem[], int moduli[]);

#endif