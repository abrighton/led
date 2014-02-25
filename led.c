// EtherCat demo program (Based on the Etherlab "user" example).
//
// This demo assumes the following ethercat slaves (in this order):
// 0  0:0  PREOP  +  EK1100 EtherCAT-Koppler (2A E-Bus)
// 1  0:1  PREOP  +  EL2202 2K. Dig. Ausgang 24V, 0.5A
// 2  0:2  PREOP  +  EL1252 2K. Fast Dig. Eingang 24V, 1�s, DC Latch
// 3  0:3  PREOP  +  EL1252 2K. Fast Dig. Eingang 24V, 1�s, DC Latch
// 4  0:4  PREOP  +  EL2252 2K. Dig. Ausgang 24V, 0.5A, DC Time Stamp

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/****************************************************************************/

#include "ecrt.h"
#include "slaves.h" // generated with the "ethercat cstruct" command
#include "EL2202.h"
#include "EL1252.h"
#include "EL2252.h"

/****************************************************************************/

// Controls how often the cyclic_task() routine is called (in usec)
#define FREQUENCY 100

// If not 0, give this process a higher priority (requires root priv)
#define PRIORITY 0

// Optional features
#define SDO_ACCESS      0

/****************************************************************************/

// EtherCAT master
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

// EtherCAT domain
static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};

// Timer
static unsigned int sig_alarms = 0;
static unsigned int user_alarms = 0;

/****************************************************************************/

// process data (PD)
static uint8_t *domain1_pd = NULL;

// Device positions
#define BusCouplerPos  0, 0
#define Slave1Pos 0, 1
#define Slave2Pos 0, 2
#define Slave3Pos 0, 3
#define Slave4Pos 0, 4

// This demo application is hard wired to use the following devices (in this order). 
// See the generated slaves.h file.

// EK1100 | EtherCAT Coupler
#define Beckhoff_EK1100 0x00000002, 0x044c2c52

// Define a struct for each slave to hold values read or written
static El2202 el2202; // First slave
static El1252 el1252_1; // Second slave
static El1252 el1252_2; // Third slave
static El2252 el2252; // fourth slave

const static ec_pdo_entry_reg_t domain1_regs[] = {
    // Slave 1: EL2202
    {Slave1Pos, Beckhoff_EL2202, 0x7000, 0x01, &el2202.offset_out[0], &el2202.bit_pos_out[0]},
    {Slave1Pos, Beckhoff_EL2202, 0x7000, 0x02, &el2202.offset_tristate[0], &el2202.bit_pos_tristate[0]},
    {Slave1Pos, Beckhoff_EL2202, 0x7010, 0x01, &el2202.offset_out[1], &el2202.bit_pos_out[1]},
    {Slave1Pos, Beckhoff_EL2202, 0x7010, 0x02, &el2202.offset_tristate[1], &el2202.bit_pos_tristate[1]},

    // Slave 2: EL1252
    {Slave2Pos, Beckhoff_EL1252, 0x6000, 0x01, &el1252_1.offset_in[0], &el1252_1.bit_pos_in[0]},
    {Slave2Pos, Beckhoff_EL1252, 0x6000, 0x02, &el1252_1.offset_in[1], &el1252_1.bit_pos_in[1]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xae, &el1252_1.offset_status[0], &el1252_1.bit_pos_status[0]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xaf, &el1252_1.offset_status[1], &el1252_1.bit_pos_status[1]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xb0, &el1252_1.offset_latch_pos[0], &el1252_1.bit_pos_latch_pos[0]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xc0, &el1252_1.offset_latch_pos[1], &el1252_1.bit_pos_latch_pos[1]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xb8, &el1252_1.offset_latch_neg[0], &el1252_1.bit_pos_latch_neg[0]},
    {Slave2Pos, Beckhoff_EL1252, 0x1d09, 0xc8, &el1252_1.offset_latch_neg[1], &el1252_1.bit_pos_latch_neg[1]},

    // Slave 3: EL1252
    {Slave3Pos, Beckhoff_EL1252, 0x6000, 0x01, &el1252_2.offset_in[0], &el1252_2.bit_pos_in[0]},
    {Slave3Pos, Beckhoff_EL1252, 0x6000, 0x02, &el1252_2.offset_in[1], &el1252_2.bit_pos_in[1]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xae, &el1252_2.offset_status[0], &el1252_2.bit_pos_status[0]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xaf, &el1252_2.offset_status[1], &el1252_2.bit_pos_status[1]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xb0, &el1252_2.offset_latch_pos[0], &el1252_2.bit_pos_latch_pos[0]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xc0, &el1252_2.offset_latch_pos[1], &el1252_2.bit_pos_latch_pos[1]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xb8, &el1252_2.offset_latch_neg[0], &el1252_2.bit_pos_latch_neg[0]},
    {Slave3Pos, Beckhoff_EL1252, 0x1d09, 0xc8, &el1252_2.offset_latch_neg[1], &el1252_2.bit_pos_latch_neg[1]},

    // Slave 4: EL2252
    {Slave4Pos, Beckhoff_EL2252, 0x1d09, 0x81, &el2252.offset_activate, &el2252.bit_pos_activate},
    {Slave4Pos, Beckhoff_EL2252, 0x1d09, 0x90, &el2252.offset_start_time, &el2252.bit_pos_start_time},
    {Slave4Pos, Beckhoff_EL2252, 0x7000, 0x01, &el2252.offset_out[0], &el2252.bit_pos_out[0]},
    {Slave4Pos, Beckhoff_EL2252, 0x7000, 0x02, &el2252.offset_tristate[0], &el2252.bit_pos_tristate[0]},
    {Slave4Pos, Beckhoff_EL2252, 0x7010, 0x01, &el2252.offset_out[1], &el2252.bit_pos_out[1]},
    {Slave4Pos, Beckhoff_EL2252, 0x7010, 0x02, &el2252.offset_tristate[1], &el2252.bit_pos_tristate[1]},

    {}
};

static unsigned int counter = 0;
static unsigned int blink = 0;


#if SDO_ACCESS
static ec_sdo_request_t *sdo;
#endif

/*****************************************************************************/

static void check_domain1_state(void)
{
    
    ec_domain_state_t ds;

    ecrt_domain_state(domain1, &ds);

    if (ds.working_counter != domain1_state.working_counter)
        printf("Domain1: WC %u.\n", ds.working_counter);
    if (ds.wc_state != domain1_state.wc_state)
        printf("Domain1: State %u.\n", ds.wc_state);

    domain1_state = ds;
}

/*****************************************************************************/

static void check_master_state(void)
{
    ec_master_state_t ms;

    ecrt_master_state(master, &ms);

    if (ms.slaves_responding != master_state.slaves_responding)
        printf("%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != master_state.al_states)
        printf("AL states: 0x%02X.\n", ms.al_states);
    if (ms.link_up != master_state.link_up)
        printf("Link is %s.\n", ms.link_up ? "up" : "down");

    master_state = ms;
}

/*****************************************************************************/
// 
static void check_slave_config_states(char* name, ec_slave_config_t* config, ec_slave_config_state_t* state)
{
    ec_slave_config_state_t s;

    ecrt_slave_config_state(config, &s);

    if (s.al_state != state->al_state)
        printf("%s: State 0x%02X.\n", name, s.al_state);
    if (s.online != state->online)
        printf("%s: %s.\n", name, s.online ? "online" : "offline");
    if (s.operational != state->operational)
        printf("%s: %soperational.\n", name, s.operational ? "" : "Not ");

    *state = s;
}

/*****************************************************************************/

#if SDO_ACCESS
static void read_sdo(void)
{
    switch (ecrt_sdo_request_state(sdo)) {
        case EC_REQUEST_UNUSED: // request was not used yet
            ecrt_sdo_request_read(sdo); // trigger first read
            break;
        case EC_REQUEST_BUSY:
            fprintf(stderr, "Still busy...\n");
            break;
        case EC_REQUEST_SUCCESS:
            fprintf(stderr, "SDO value: 0x%04X\n",
                    EC_READ_U16(ecrt_sdo_request_data(sdo)));
            ecrt_sdo_request_read(sdo); // trigger next read
            break;
        case EC_REQUEST_ERROR:
            fprintf(stderr, "Failed to read SDO!\n");
            ecrt_sdo_request_read(sdo); // retry reading
            break;
    }
}
#endif

/****************************************************************************/
// Do the write for the EL2202: alternately blink the LEDs
// (Note: Setting a tristate bit to 1 turns the devices LED yellow and disables the output.
static void write_process_data_el2202() {
    EC_WRITE_BIT(domain1_pd + el2202.offset_tristate[0], el2202.bit_pos_tristate[0], 0x00);
    EC_WRITE_BIT(domain1_pd + el2202.offset_tristate[1], el2202.bit_pos_tristate[1], 0x00);
    EC_WRITE_BIT(domain1_pd + el2202.offset_out[0], el2202.bit_pos_out[0], blink ? 0x01 : 0x00);
    EC_WRITE_BIT(domain1_pd + el2202.offset_out[1], el2202.bit_pos_out[1], blink ? 0x00 : 0x01);
}

// Do the write for the EL2252: alternately blink the LEDs
static void write_process_data_el2252() {
    EC_WRITE_BIT(domain1_pd + el2252.offset_tristate[0], el2252.bit_pos_tristate[0], 0x00);
    EC_WRITE_BIT(domain1_pd + el2252.offset_tristate[1], el2252.bit_pos_tristate[1], 0x00);
    EC_WRITE_BIT(domain1_pd + el2252.offset_out[0], el2252.bit_pos_out[0], blink ? 0x01 : 0x00);
    EC_WRITE_BIT(domain1_pd + el2252.offset_out[1], el2252.bit_pos_out[1], blink ? 0x00 : 0x01);
}

/****************************************************************************/
static void write_process_data() {
    write_process_data_el2202();
    write_process_data_el2252();
}

/****************************************************************************/
// ONCE THE MASTER IS ACTIVATED, THE APP IS IN CHARGE OF EXCHANGING DATA THROUGH
// EXPLICIT CALLS TO THE ECRT LIBRARY (DONE IN THE IDLE STATE BY THE MASTER)
static void cyclic_task()
{
    int i;

    // receive process data
    ecrt_master_receive(master);  // RECEIVE A FRAME
    ecrt_domain_process(domain1); // DETERMINE THE DATAGRAM STATES

    // check process data state (optional)
    check_domain1_state();

    if (counter) {
        counter--;
    } else { // do this at 1 Hz
        counter = FREQUENCY;

        // calculate new process data
        blink = !blink;

        // check for master state (optional)
        check_master_state();

        // check for islave configuration state(s) (optional)
        check_slave_config_states("Slave1", el2202.config, &el2202.config_state);
        check_slave_config_states("Slave2", el1252_1.config, &el1252_1.config_state);
        check_slave_config_states("Slave3", el1252_2.config, &el1252_2.config_state);
        check_slave_config_states("Slave4", el2252.config, &el2252.config_state);

#if SDO_ACCESS
        // read process data SDO
        read_sdo();
#endif

    }

#if 0
    // read process data
    printf("Slave1 Out1: state %u value %u\n",
            EC_READ_U8(domain1_pd + slave1_out1_status),
            EC_READ_U16(domain1_pd + slave1_out1_value));
#endif

    // write process data
    write_process_data();

    // send process data
    ecrt_domain_queue(domain1); // MARK THE DOMAIN DATA AS READY FOR EXCHANGE
    ecrt_master_send(master);   // SEND ALL QUEUED DATAGRAMS
}

/****************************************************************************/

static void signal_handler(int signum) {
    switch (signum) {
        case SIGALRM:
            sig_alarms++;
            break;
    }
}

/****************************************************************************/
// Sets the timer for the cyclic task.
// Returns non-zero on error.
static int set_timer() {
    struct sigaction sa;
    struct itimerval tv;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, 0)) {
        fprintf(stderr, "Failed to install signal handler!\n");
        return -1;
    }

    printf("Starting timer...\n");
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 1000000 / FREQUENCY;
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 1000;

    if (setitimer(ITIMER_REAL, &tv, NULL)) {
        fprintf(stderr, "Failed to start timer: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

/****************************************************************************/
// Configures the PDO given the address of the slave's config pointer, syncs (from slaves.h),
// the slave's position and vendor info.
// Returns non-zero on error.
static int configure_pdo(
    ec_slave_config_t** config, // output param
    ec_sync_info_t* syncs, 
    uint16_t alias, 
    uint16_t position, 
    uint32_t vendor_id, 
    uint32_t product_code) {

    if (!(*config = ecrt_master_slave_config(master, alias, position, vendor_id, product_code))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(*config, EC_END, syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
    return 0;
}

/****************************************************************************/
int main(int argc, char **argv)
{
    ec_slave_config_t *sc;
    
    // FIRST, REQUEST A MASTER INSTANCE
    master = ecrt_request_master(0);
    if (!master)
        return -1;

    // THEN, CREATE A DOMAIN
    domain1 = ecrt_master_create_domain(master);
    if (!domain1)
        return -1;

#if SDO_ACCESS
    fprintf(stderr, "Creating SDO requests...\n");
    if (!(sdo = ecrt_slave_config_create_sdo_request(el2202.config, 
						     slave_1_pdo_entries[0].index, 
						     slave_1_pdo_entries[0].subindex, 
						     slave_1_pdo_entries[0].bitlength))) {
        fprintf(stderr, "Failed to create SDO request.\n");
        return -1;
    }
    ecrt_sdo_request_timeout(sdo, 500); // ms
#endif

    printf("Configuring PDOs...\n");
    if (configure_pdo(&el2202.config, slave_1_syncs, Slave1Pos, Beckhoff_EL2202)) return -1;
    if (configure_pdo(&el1252_1.config, slave_2_syncs, Slave2Pos, Beckhoff_EL1252)) return -1;
    if (configure_pdo(&el1252_2.config, slave_3_syncs, Slave3Pos, Beckhoff_EL1252)) return -1;
    if (configure_pdo(&el2252.config, slave_4_syncs, Slave4Pos, Beckhoff_EL2252)) return -1;

    // Create configuration for bus coupler
    sc = ecrt_master_slave_config(master, BusCouplerPos, Beckhoff_EK1100);
    if (!sc) {
        return -1;
    }

    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        return -1;
    }

    // ACTIVATE THE MASTER. DO NOT APPLY ANY CONFIGURATION AFTER THIS, IT WON'T WORK
    printf("Activating master...\n");
    if (ecrt_master_activate(master))
        return -1;

    // INITIALIZE THE PROCESS DOMAIN MEMORY (FOR USER-SPACE APPS)
    if (!(domain1_pd = ecrt_domain_data(domain1))) {
        return -1;
    }

#if PRIORITY
    pid_t pid = getpid();
    if (setpriority(PRIO_PROCESS, pid, -19))
        fprintf(stderr, "Warning: Failed to set priority: %s\n", strerror(errno));
#endif

    int timer_status = set_timer();
    if (timer_status) return timer_status;

    printf("Started.\n");
    while (1) {
        pause();

#if 0
        struct timeval t;
        gettimeofday(&t, NULL);
        printf("%u.%06u\n", t.tv_sec, t.tv_usec);
#endif

        while (sig_alarms != user_alarms) {
            cyclic_task();
            user_alarms++;
        }
    }

    return 0;
}

/****************************************************************************/
