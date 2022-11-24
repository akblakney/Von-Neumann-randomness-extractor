/*
 * For instructions on how to compile and use this program, see the README
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "von_neumann.h"

/*
 * HEADER_LEN gives the length of the header of the .wav file to skip over.
 * The header should only be 44 bytes long, but I set the default value to
 * 96000 so that the entire first second of the recording is skipped, just to
 * be safe. HEADER_LEN should always be set to an even number to 
 * make sure we get the LSB with little-endian format.
 */

#ifndef HEADER_LEN
  #define HEADER_LEN 96000
#endif

/*
 * iterates over entire file fp, writing von neumann genereated output to of.
 * num_bytes gives the spacing between bytes to consider. Should be even.
 * returns 0 on success, 1 on failure
 */
int von_neumann_from_file(int num_bytes) {

  uint8_t header_buf[1];
  uint8_t buf[num_bytes * 2];
  uint8_t curr_byte = 0;
  uint8_t bitcount = 0;
  uint8_t b1;
  uint8_t b2;
  int temp;
  int readCount = 0;

  // skip over header
  while (readCount < HEADER_LEN) {
    fread(header_buf, sizeof(header_buf), 1, stdin);
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
  
  if (argc != 2) {
    fprintf(stderr, "ERROR: num_bytes parameter must be given as int on the command line\n");
    exit(EXIT_FAILURE);
    }

  char *a = argv[1];
  int num_bytes = atoi(a);

  int success = von_neumann_from_file(num_bytes);
  
  if (!success) {
    return 1;
  }

  return 0; 

}
