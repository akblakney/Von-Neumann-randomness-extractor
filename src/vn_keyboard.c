/*
 * This program collects entropy by measuring the time between keystrokes (between
 * the times the user presses the ENTER key), and taking the least significant bits
 * of those time differences.
 * The raw bits collected by the timings are then passed through the Von Neumann
 * extractor, and the random output is written  to an output file given on the
 * command line.
 *
 * To work correctly, the user must ONLY press the ENTER key repeatedly. Otherwise,
 * the output timings will correspond to the times at which characters are read in
 * by getchar() in between ENTER keys, which negates the purpose of using user
 * input as a source of entropy. The slower the user presses the ENTER key, the
 * better. If a few seconds are given in between each key press, this should be
 * a robust entropy source, with the only limitation being the accuracy, stability,
 * and resolution of the system clock. Users will have to verify themselves that 
 * this program works appropriately on their own specific system.
 *
 * To compile:
 * gcc von_neumann.c vn_keyboard.c -o /path/to/executable [optional:
 * -D NUM_BYTES=<int> -D VERBOSE]
 *
 * The optional NUM_BYTES definition allows the user to specify how many total bytes
 * are generated (the default is 16 = 128 bits). The verbose flag will cause the
 * program to print whenever a bit or byte has been generated.
 *
 * Usage:
 * /path/to/executable /path/to/output/file
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "von_neumann.h"

#ifndef NUM_BYTES
  #define NUM_BYTES 16
#endif

int main(int argc, char* argv[]) {

  // setup output file
  FILE *of = fopen(argv[1], "wb");
  if (of == NULL) {
    fprintf(stderr, "cannot open output file\n");
    return 1;
  }

  printf("press ONLY the ENTER key until the program terminates. Do not press it too quickly (no more than once per second)\n");

  // to keep track of how many bytes to write to output
  int N = NUM_BYTES;
  uint16_t byte_count = 0;

  unsigned char c;

  // time stuff
  long int curr_time;
  struct timespec ts;

  // von neumann stuff
  uint8_t curr_byte;
  uint8_t bitcount = 0;
  uint8_t bits[2];
  int temp;
  
  // main loop
  while (byte_count < N) {
    
    // get two bits from two keyboard reads
    // populates bits array.
    for (uint8_t j = 0; j < 2; j++) {
      
      c = getchar();
      
      if (clock_gettime(CLOCK_REALTIME, &ts) < 0) {
        fprintf(stderr, "could not read clock");
        return 1;
      }
      
      curr_time = (ts.tv_nsec);
      //curr_secs = ts.tv_sec;
  
      bits[j] = 0x01 & curr_time;
    }

    // do single von neumann with two bits
    temp = single_von_neumann(bits[0], bits[1], curr_byte, bitcount);

    if (temp >= 0) {
#ifdef VERBOSE
      printf("generated one bit\n");
#endif
      curr_byte = temp;
      bitcount += 1;
    }

    // we have an entire byte, write to file and reset
    if (bitcount == BYTE_SIZE) {
#ifdef VERBOSE
      printf("generated one byte\n");
#endif
      fwrite(&curr_byte, sizeof(curr_byte), 1, of);
      bitcount = 0;
      curr_byte = 0;
      byte_count++;
    }
  }

  fclose(of);
  return 0;

}
  
