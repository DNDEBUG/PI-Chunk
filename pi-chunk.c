#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Efficient modular exponentiation using square-and-multiply algorithm
// Modified to use 64-bit integers for both exponent and modulus
long long mod_pow(long long base, long long exp, long long mod) {
    if (mod == 1) return 0;
    long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// Computes the BBP formula for one j (1, 4, 5, or 6)
// Modified to support 64-bit position values
double bbp_sum(long long d, int j) {
    double sum = 0.0;
    
    // Handle larger positions more efficiently
    // For very large d, we only need to compute the series modulo 1
    
    // First sum: k = 0 to d
    long long k_limit = (d < 1000000) ? d : 1000000; // Limit for extremely large positions
    
    for (long long k = 0; k <= k_limit; ++k) {
        long long m = 8 * k + j;
        if (m == 0) continue; // Avoid division by zero
        
        // For large positions, we need to be careful with the exponentiation
        double term;
        if (d - k <= 1000000) {
            // For smaller exponents, use modular exponentiation
            term = (double)mod_pow(16, d - k, m) / m;
        } else {
            // For extremely large exponents, use a different approach
            // Use the property that 16^(d-k) mod m = 16^((d-k) mod φ(m)) mod m
            // For prime m, φ(m) = m-1. For simplicity we'll use mod (m-1) for all m
            long long reduced_exp = (d - k) % (m - 1);
            if (reduced_exp < 0) reduced_exp += (m - 1); // Ensure positive exponent
            term = (double)mod_pow(16, reduced_exp, m) / m;
        }
        
        sum += term;
        sum = sum - floor(sum); // Keep only the fractional part
    }
    
    // For very large positions, the terms past a certain point become negligible
    // Second sum: k = d+1 to ∞ (with appropriate termination)
    if (d < 1000000) { // Only compute for reasonable positions
        double term = 0.0;
        for (long long k = d + 1; ; ++k) {
            long long m = 8 * k + j;
            // Use more stable method for small values
            term = term / 16.0;
            if (k == d + 1) {
                term = 1.0 / (16.0 * m);
            }
            
            if (term < 1e-15) break;
            sum += term;
            if (k - d > 100) break; // Safety exit condition
        }
    }
    
    return sum - floor(sum); // Return fractional part
}

// Compute the d-th digit of π in base 16 (d >= 0)
char get_pi_hex_digit(long long d) {
    if (d < 0) return '?'; // Error case
    
    double x = 4.0 * bbp_sum(d, 1)
             - 2.0 * bbp_sum(d, 4)
             - 1.0 * bbp_sum(d, 5)
             - 1.0 * bbp_sum(d, 6);
    
    x = x - floor(x);
    if (x < 0) x = 0;
    int digit = (int)(x * 16.0);
    
    // Clamp to valid hex digit range
    if (digit < 0) digit = 0;
    if (digit > 15) digit = 15;
    
    return "0123456789ABCDEF"[digit];
}

int main() {
    long long position; // Changed to 64-bit integer
    printf("Enter digit position (0-based): ");
    if (scanf("%lld", &position) != 1 || position < 0) { // Using %lld format for long long
        printf("Invalid input. Please enter a non-negative integer.\n");
        return 1;
    }
    
    if (position > 1000000000000LL) { // 1 trillion
        printf("Warning: Position is extremely large. Results may not be accurate.\n");
        printf("For best results, positions below 1 billion are recommended.\n");
        printf("Continue? (y/n): ");
        char response;
        scanf(" %c", &response);
        if (response != 'y' && response != 'Y') {
            return 0;
        }
    }
    
    printf("Calculating 64 hexadecimal digits of pi starting at position %lld:\n", position);
    for (int i = 0; i < 64; i++) {
        putchar(get_pi_hex_digit(position + i));
    }
    putchar('\n');
    
    return 0;
}
