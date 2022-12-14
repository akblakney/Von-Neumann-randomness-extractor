# Von-Neumann-randomness-extractor

## Overview
A true random number generator (TRNG), as opposed to a pseudo-random number generator (PRNG), requires two key components:

- an unpredictable and random entropy source
- some mechanism to transform that entropy source into a uniformly distributed random output. 

The first requirement is not too difficult to fulfill: there are many good entropy sources such as electrical or atmospheric noise, or shot noise on a camera sensor. However, transforming that entropy source into a uniform random output can prove tricky. The [Von Neumann randomness extractor](https://en.wikipedia.org/wiki/Randomness_extractor#Von_Neumann_extractor) is one method of doing this. Given i.i.d. but potentially biased input bits, the Von Neumann extractor will output a sequence of i.i.d. *unbiased* bits, i.e. a Bernoulli sequence with probability parameter 0.5.

In this project, I implement the Von Neumann extractor generally (`von_neumann.c`), and then give driver programs that can use the extractor to generate random numbers from recorded atmospheric noise (`vn_wav.c`) and keyboard input timings (`vn_keyboard.c`). As long as the inputs to the Von Neumann extractor are i.i.d. (independent and identically distributed), the output of the Von Neumann extractor is guaranteed to be an i.i.d. sequence of uniform random bits. With any physical system, it is impossible to be sure with 100% certainty that some quantity is i.i.d. However, here we have some rationale for making this assumption. First, if the measurements are taken far enough apart (whether we measure the values of the waveform of atmospheric noise, or keyboard timing inputs for sufficiently spaced out keyboard taps), they should be independent. In other words, different measurement values should not depend on one another. Second, as long as the quantities we are measuring are consistent across long periods of time, i.e., the behavior of our entropy source is stable, the measurements should share the same probability distribution, and are therefore identically distributed. Lastly, because we only use the least significant bit of the measurements of the waveform of atmospheric noise, these requirements needn't be as stringent: if slight instability is present in the waveform of atmospheric noise over long periods of time, it is unlikely to affect the least significant bits of those measurements with much impact. Therefore I believe both of these conditions to be satisfied for the two entropy sources I use in this project. 

For those interested in these types of design concerns of TRNGs, you could take a look at section 1.3 of [this paper](https://www.boazbarak.org/Papers/trng.pdf) by Barak, Shaltiel, and Tromer, which briefly discusses the Von Neumann extractor, and the use of "far away bits" as sources of i.i.d. inputs to a Von Neumann extractor.

## Usage
Here you will find a detailed explanation on compiling and using each of the programs.

### `vn_wav`
This program reads binary data from `stdin`, processes the raw byte values with the Von Neumann randomness extractor, and then writes the processed (hopefully random) output to `stdout`. It takes one command line argument, an integer `num_bytes`, which gives the distance between bytes processed by the Von Neumann randomness extractor. Only *even* values should be used for this parameter due to the way `.wav` files are formated (16-bit integers). While testing the output of the extractor, I have found that any positive even integer is sufficient to produce a quality random output. (More formally, I have found no evidence that any positive even integer does *not* produce a random output). Nonetheless, I recommend a larger value (perhaps 64 or 128) to be safe.

Note that the quality of randomness of the output is heavily dependent on the quality of the input, i.e. the recorded atmospheric noise must contain enough entropy and be formatted appropriately. For this reason, users must test their own setup with programs like [ent](https://www.fourmilab.ch/random/), or any other randomness testing software.

To compile, navigate to the `src/` directory and execute the following command:

`gcc von_neumann.c vn_wav.c -o /path/to/executable [optional flags: -D HEADER_LEN=<int>]`

For Linux systems, if you want a binary executable in your `$PATH`, then you can use (you may need `sudo` privileges):

`gcc von_neumann.c vn_wav.c -o /usr/local/bin/von-neumann`

Then, `von-neumann` will work anywhere on your system.

Using the executable is then simple: if you have recorded atmospheric noise in `noise.wav`, you can output random data to a file `random_data` with:

`cat noise.wav | von-neumann 64 > random_data`.

(The above command assumes you put an executable called `von-neumann` somewhere in your `$PATH`, otherwise adjust the executable name accordingly). You can also change the value `num_bytes`, which is set to 64 in the above command but can be adjusted to any positive even integer, with larger values giving more assurance that the output is random.
 
### `vn_keyboard`
This program prompts the user to press the ENTER key repeatedly, and the time difference between the key inputs is used as an entropy source. Only the least significant bit of the number of nanoseconds between key strokes is used, so as long as keystrokes are a couple seconds apart, the output should be reliably random. (A potential limitation is the resolution, stability, and accuracy of the clock on your system, although as long as it supports nanosecond time resolution this should be fine). The output file to write the random data to is given on the command line:

To compile:

`gcc von_neumann.c vn_keyboard.c -o /path/to/executable [optional:-D NUM_BYTES=<int> -D VERBOSE]`

To use: 

`$ ./vn_keyboard output_file`

Note: this is not a very efficient way to generate random numbers, as you have to press enter 32 times on average to generate a single byte of random data. This is because there are 8 bits in a byte, two bits must be considered at a time to produce one bit of output, and on average half of the attempts to generate a bit will fail. Thus 8 x 2 x 2 = 32. I think it is interesting nonetheless.
