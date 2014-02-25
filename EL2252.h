// Data related to Beckhoff EL2252

// EL2252 | 2-channel digital output terminal with time stamp, tri-state
#define Beckhoff_EL2252 0x00000002, 0x08cc3052

// Holds values written to or read from the device
typedef struct {
    // Stores PDO entry's (byte-)offset in the process data. 
    unsigned int offset_activate;
    unsigned int offset_start_time;
    unsigned int offset_out[2];
    unsigned int offset_tristate[2];

    // Store a bit position (0-7) within the above offset
    unsigned int bit_pos_activate;
    unsigned int bit_pos_start_time;
    unsigned int bit_pos_out[2];
    unsigned int bit_pos_tristate[2];
    
    // Stores all the 1 bit values
    ec_slave_config_t* config;

    // Stores the configuration states of out1, out2
    ec_slave_config_state_t config_state;

} El2252;
