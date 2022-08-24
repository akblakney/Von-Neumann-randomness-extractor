#define BYTE_SIZE 8
uint8_t set_bit(uint8_t num, uint8_t position);
uint8_t clear_bit(uint8_t num, uint8_t position);
bool get_bit(uint8_t num, uint8_t position);
int single_von_neumann(uint8_t b1, uint8_t b2, uint8_t curr_byte, uint8_t bitcount);
