# Von-Neumann-randomness-extractor

## Overview
A true random number generator (TRNG), as opposed to a pseudo-random number generator (PRNG), requires two key components:

- an unpredictable and random entropy source
- some mechanism to transform that entropy source into a uniformly random output. 

The first requirement is not too difficult to fulfill: there are many good entropy sources such as electrical or atmospheric noise, or shot noise on a camera sensor. However transforming that entropy source into a uniform random output can prove tricky. The [Von Neumann randomness extractor](https://en.wikipedia.org/wiki/Randomness_extractor#Von_Neumann_extractor) is one method of doing this. Given uncorrelated but potentially biased input bits, the Von Neumann extractor will output a sequence of i.i.d. *unbiased* bits, i.e. a Bernoulli sequence with probability parameter 0.5.

In this project, I implement the Von Neumann extractor generally (`von_neumann.c`), and then give driver programs that can use the extractor to generate random numbers from recorded atmospheric noise (`vn_wav.c`) and keyboard input timings (`vn_keyboard.c`).

## Usage
Each file contains commented portions that describe how to compile and use each program. You will find a more detailed explanation of compilation options and usage there.

### `vn_wav`
This program takes as input a file of recorded atmospheric noise in `.wav` format, and outputs a binary file generated by the input file and processed by the Von Neumann extractor. The input and output files are given on the command line as follows: 

`$ ./vn_wav input_file.wav output_file`

### `vn_keyboard`
This program prompts the user to press the ENTER key repeatedly, and the time difference between the key inputs is used as an entropy source. Only the least significant bit of the number of nanoseconds between key strokes is used, so as long as keystrokes are a couple seconds apart, the output should be reliably random. (A potential limitation is the resolution, stability, and accuracy of the clock on your system, although as long as it supports nanosecond time resolution this should be fine). The output file to write the random data to is given on the command line:

`$ ./vn_keyboard output_file`
