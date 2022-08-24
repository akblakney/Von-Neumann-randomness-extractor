/* This file contains methods that implement the Von Neumann randomness extractor.
 * The Von Neumann extractor works on pairs of bits from an i.i.d. entropy source.
 * If two bits are identical, they are discarded. If they are different, then we take
 * the leading bit and output it to our output stream. 
 *
 * The Von Neumann extractor is useful because even if our entropy source is biased
 * (i.e. the probability of a 1 is not equal to the probability of a 0), the output of
 * the Von Neumann extractor will be unbiased (i.e., the output will be 1's and 0's with
 * equal probability, like flipping a fair coin). However it is important to note that
 * the Von Neumann extractor does NOT handle or account for correlation, so it is critical
 * that the input to the Von Neumann extractor is NOT correlated. In other words, it
 * de-biases but does not de-correlate.
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * The set_bit, clear_bit, and get_bit functions are not my own. They are from
 * here: https://hackernoon.com/bit-manipulation-in-c-and-c-1cs2bux
 */

uint8_t set_bit(uint8_t num, uint8_t position)
{
  uint8_t mask = 1 << position;
  return num | mask;
}

uint8_t clear_bit(uint8_t num, uint8_t position)
{
  uint8_t mask = 1 << position;
  return num & ~mask;
}

// not currently using right now but good to have
bool get_bit(uint8_t num, uint8_t position)
{
  bool bit = num & (1 << position);
  return bit;
}

/*
 * b1 and b2 are 8-bit integers. Each should be either 0 or 1 representing the two bits
 * we're considering.
 * 
 * curr_byte is the current value of the byte we're working on
 * 
 * bitcount is the bit number we are on (which bit we will set if applicable)
 *
 * As per the Von Neumann extractor, if the two bits are identical, then we return -1
 * to indicate we should throw the bits out. Otherwise, we take the value of the second
 * bit and add it to our output by returning an updated value of curr_byte.
 */
int single_von_neumann(uint8_t b1, uint8_t b2, uint8_t curr_byte, uint8_t bitcount) {

  // if they are the same, don't return anything; return -1 for our setup
  if (b1 == b2)
    return -1;
  
  // if different, set to 0 or 1
  if (b2 == 1) {
    return set_bit(curr_byte, bitcount);
  }
  
  return clear_bit(curr_byte, bitcount);
  
}

