// Data related to Beckhoff EL1252

// EL1252 | 2-channel digital input terminal with time stamp
#define Beckhoff_EL1252 0x00000002, 0x04e43052

// Holds values written to or read from the device
typedef struct {
    // Stores PDO entry's (byte-)offset in the process data. 
    unsigned int offset_in[2];
    unsigned int offset_status[2];
    unsigned int offset_latch_pos[2];
    unsigned int offset_latch_neg[2];

    // Store a bit position (0-7) within the above offset
    unsigned int bit_pos_in[2];
    unsigned int bit_pos_status[2];
    unsigned int bit_pos_latch_pos[2];
    unsigned int bit_pos_latch_neg[2];
    
    // Stores all the 1 bit values
    ec_slave_config_t* config;

    // Stores the configuration states of out1, out2
    ec_slave_config_state_t config_state;

} El1252;
