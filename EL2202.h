// Data related to Beckhoff EL2202

// EL2202 | 2-channel digital output terminal 24 V DC, TON/TOFF 1 µs, push-pull
#define Beckhoff_EL2202 0x00000002, 0x089a3052

// Holds values written to or read from the device
typedef struct {
    // Stores PDO entry's (byte-)offset in the process data. 
    unsigned int offset_out[2];
    unsigned int offset_tristate[2];

    // Store a bit position (0-7) within the above offset
    unsigned int bit_pos_out[2];
    unsigned int bit_pos_tristate[2];
    
    // Stores all the 1 bit values
    ec_slave_config_t* config;

    // XXX add tristate value and state

    // Stores the configuration states of out1, out2
    ec_slave_config_state_t config_state;

} El2202;
