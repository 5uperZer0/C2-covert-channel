#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LENGTH_SET 6

void rem_to_ascii(int rem[], char ascii_text[]) {
    char grouped[13] = {0};
    // Concatenate all remainders together for conversion
    for (int i = 0; i < LENGTH_SET; i++) {
        char temp[3];
        // Store numbers as characters 
        sprintf(temp, "%02d", rem[i]);
        strcat(grouped, temp);
    }

    // Split remainders into new values by using number of digits
    for (int i = 0; i < 4; i++) {
        // Group characters
        char temp[4] = {grouped[3 * i], grouped[3 * i + 1], grouped[3 * i + 2], '\0'};
        // Convert chars to a new number
        int ascii_val = atoi(temp);
        // Convert new number to ascii again
        ascii_text[i] = (char) ascii_val;
    }
    ascii_text[4] = '\0';
    printf("Message: %s\n", ascii_text);
}

// Returns the gcd of a and b
// Uses the fact that gcd(a,b) = gcd(b,q)
// where a = q mod b
long long gcd (long long a, long long b) {
    long long temp;
    while (b!=0){
        temp = b;
        b = a%b;
        a = temp;
    }
    return a;
}

// For rem (mod modulus) calculates the inverse
// With much help from https://www.geeksforgeeks.org/multiplicative-inverse-under-modulo-m/
long long mod_inv(long long rem, long long modulus){
    long long m = modulus;
    long long t, q;
    long long x = 0;
    long long y = 1;

    while (rem>1) {
        q = rem/modulus;
        t = modulus;
        modulus = rem%modulus;
        rem = t;
        t = x;
        x = y-q*x;
        y = t;
    }

    y = y<0 ? y+m : y;
    return y;
}

long long CRT(int rem[], int moduli[]){
    long long N = 1;
    for (int i = 0; i < LENGTH_SET; i++) N*=moduli[i];

    long long x = 0;
    long long N_i, inv_i;
    for (int j = 0;j < LENGTH_SET; j++){
        N_i = N/moduli[j];
        inv_i = mod_inv(N_i, (long long)moduli[j]);
        x+= ((long long)rem[j] * N_i%N * inv_i%N) % N;
        x%=N;
    }
    return (x<0) ? x+N : x;
}

void txt_to_ascii_rem(const char* text, int rem[], int moduli[]){
    char sha1[13] = {0}; // maybe do as sha1[13];
    for (int i = 0; i<4; i++){
        char temp[4];
        sprintf(temp, "%03d", (int) text[i]);
        strcat(sha1, temp);
    }

    for (int j = 0; j < LENGTH_SET; j++){
        char temp[3] = {sha1[2*j], sha1[2*j+1], '\0'};
        rem[j] = atoi(temp);
    }
}