/*
 * This program reads the file given in the first command line argument, processes
 * its raw byte values with the Von Neumann randomness extractor, and then writes
 * the processed (hopefully random) output to the file given in the second command
 * line argument. 
 * 
 * It is designed for use on .wav files of recorded atmospheric noise, in little-endian
 * format with 16-bit integers. Different formats could be accomodated by changing
 * the HEADER_LEN and NUM_BYTES arguments in this file or on the command line.
 * The default values provided in this program should be acceptable for most purposes.
 * 
 * To compile:
 * gcc von_neumann.c vn_wav.c -o /path/to/executable [optional flags: -D NUM_BYTES=<int> 
 *   HEADER_LEN=<int>]
 * 
 * Usage:
 * /path/to/executable /path/to/wav/file.wav /path/to/output/file
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "von_neumann.h"

/*
 * NUM_BYTES gives the distance between bytes that we check,
 * e.g. NUM_BYTES=2 means we compare every second byte for von neumann,
 * NUM_BYTES=4 means we compare every fourth byte, and so on.
 * NUM_BYTES should be set to an even value to make sure we are
 * looking at the even bytes since the .wav files are encoded in little-endian
 * format. Although the program still seems to be an effective RNG even
 * with odd values.
 * 
 * The minimum value of NUM_BYTES should be 2, with a recommended value
 * around 100 to guarantee robustness.
 */

#ifndef NUM_BYTES
  #define NUM_BYTES 128
#endif

/*
 * HEADER_LEN gives the length of the header of the .wav file to skip over.
 * The header should only be 44 bytes long, but I set the default value to
 * 100 to be safe. HEADER_LEN should always be set to an even number to 
 * make sure we get the LSB with little-endian format.
 */

#ifndef HEADER_LEN
  #define HEADER_LEN 100
#endif

bool verify_le_encoding(FILE *fp) {

  // this is what the identifier should read if the file is
  // in little-endian format (what we want)
  unsigned char target[4] = {0x52, 0x49, 0x46, 0x46};

  // seek to position zero
  int success = fseek(fp, 0, SEEK_SET);
  if (success != 0) {
    fprintf(stderr, "seek to byte 0 failed\n");
  }
  
  // check the header in the file
  unsigned char c[4];
  if (!fread(c, sizeof(c), 1, fp)) {
    fprintf(stderr, "failed to read header identifier\n");
  }

  // compare
  for (size_t i = 0; i < 4; i++) {
    if (c[i] != target[i]) {
      return false;
    }
  }
  return true;
  
}

/*
 * iterates over entire file fp, writing von neumann genereated output to of.
 * num_bytes gives the spacing between bytes to consider. Should be even.
 * returns 0 on success, 1 on failure
 */
int von_neumann_from_file(uint16_t num_bytes) {

  uint8_t buf[num_bytes * 2];
  uint8_t curr_byte = 0;
  uint8_t bitcount = 0;
  uint8_t b1;
  uint8_t b2;
  int temp;
  int readCount = 0;

  // skip over header
  while (readCount < HEADER_LEN) {
    fread(buf, sizeof(buf), 1, stdin);
    readCount++;
  }

  // main loop
  while (fread(buf, sizeof(buf), 1, stdin)) {

    b1 = buf[0] % 2;
    b2 = buf[num_bytes] % 2;

    temp = single_von_neumann(b1, b2, curr_byte, bitcount);

    if (temp >= 0) {
      curr_byte = temp;
      bitcount += 1;
    }

    // check for full curr_byte and if it is write to file
    if (bitcount == BYTE_SIZE) {
      fwrite(&curr_byte, sizeof(curr_byte), 1, stdout);
      bitcount = 0;
      curr_byte = 0;
    } 
    readCount++;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  
  int success = von_neumann_from_file(NUM_BYTES);
  
  if (!success) {
    return 1;
  }

  return 0; 

}
