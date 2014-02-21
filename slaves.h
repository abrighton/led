/* Master 0, Slave 1, "EL2202"
 * Vendor ID:       0x00000002
 * Product code:    0x089a3052
 * Revision number: 0x00100000
 */

ec_pdo_entry_info_t slave_1_pdo_entries[] = {
    {0x7000, 0x01, 1}, /* Output */
    {0x7000, 0x02, 1}, /* TriState */
    {0x7010, 0x01, 1}, /* Output */
    {0x7010, 0x02, 1}, /* TriState */
};

ec_pdo_info_t slave_1_pdos[] = {
    {0x1600, 2, slave_1_pdo_entries + 0}, /* Channel 1 */
    {0x1601, 2, slave_1_pdo_entries + 2}, /* Channel 2 */
};

ec_sync_info_t slave_1_syncs[] = {
    {0, EC_DIR_OUTPUT, 2, slave_1_pdos + 0, EC_WD_ENABLE},
    {0xff}
};

/* Master 0, Slave 2, "EL1252"
 * Vendor ID:       0x00000002
 * Product code:    0x04e43052
 * Revision number: 0x00120000
 */

ec_pdo_entry_info_t slave_2_pdo_entries[] = {
    {0x6000, 0x01, 1}, /* Input */
    {0x6000, 0x02, 1}, /* Input */
    {0x0000, 0x00, 6},
    {0x1d09, 0xae, 8}, /* Status1 */
    {0x1d09, 0xaf, 8}, /* Status2 */
    {0x1d09, 0xb0, 64}, /* LatchPos1 */
    {0x1d09, 0xb8, 64}, /* LatchNeg1 */
    {0x1d09, 0xc0, 64}, /* LatchPos2 */
    {0x1d09, 0xc8, 64}, /* LatchNeg2 */
};

ec_pdo_info_t slave_2_pdos[] = {
    {0x1a00, 1, slave_2_pdo_entries + 0}, /* Channel 1 */
    {0x1a01, 2, slave_2_pdo_entries + 1}, /* Channel 2 */
    {0x1a02, 0, NULL}, /* Reserved */
    {0x1a13, 6, slave_2_pdo_entries + 3}, /* Latch */
};

ec_sync_info_t slave_2_syncs[] = {
    {0, EC_DIR_INPUT, 3, slave_2_pdos + 0, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 1, slave_2_pdos + 3, EC_WD_DISABLE},
    {2, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {0xff}
};

/* Master 0, Slave 3, "EL1252"
 * Vendor ID:       0x00000002
 * Product code:    0x04e43052
 * Revision number: 0x00120000
 */

ec_pdo_entry_info_t slave_3_pdo_entries[] = {
    {0x6000, 0x01, 1}, /* Input */
    {0x6000, 0x02, 1}, /* Input */
    {0x0000, 0x00, 6},
    {0x1d09, 0xae, 8}, /* Status1 */
    {0x1d09, 0xaf, 8}, /* Status2 */
    {0x1d09, 0xb0, 64}, /* LatchPos1 */
    {0x1d09, 0xb8, 64}, /* LatchNeg1 */
    {0x1d09, 0xc0, 64}, /* LatchPos2 */
    {0x1d09, 0xc8, 64}, /* LatchNeg2 */
};

ec_pdo_info_t slave_3_pdos[] = {
    {0x1a00, 1, slave_3_pdo_entries + 0}, /* Channel 1 */
    {0x1a01, 2, slave_3_pdo_entries + 1}, /* Channel 2 */
    {0x1a02, 0, NULL}, /* Reserved */
    {0x1a13, 6, slave_3_pdo_entries + 3}, /* Latch */
};

ec_sync_info_t slave_3_syncs[] = {
    {0, EC_DIR_INPUT, 3, slave_3_pdos + 0, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 1, slave_3_pdos + 3, EC_WD_DISABLE},
    {2, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {0xff}
};

/* Master 0, Slave 4, "EL2252"
 * Vendor ID:       0x00000002
 * Product code:    0x08cc3052
 * Revision number: 0x00120000
 */

ec_pdo_entry_info_t slave_4_pdo_entries[] = {
    {0x1d09, 0x81, 8}, /* Activate */
    {0x1d09, 0x90, 64}, /* StartTime */
    {0x7000, 0x01, 1}, /* Output */
    {0x7000, 0x02, 1}, /* TriState */
    {0x7010, 0x01, 1}, /* Output */
    {0x7010, 0x02, 1}, /* TriState */
    {0x0000, 0x00, 4},
};

ec_pdo_info_t slave_4_pdos[] = {
    {0x1602, 1, slave_4_pdo_entries + 0}, /* DC Sync Activate */
    {0x1603, 1, slave_4_pdo_entries + 1}, /* DC Sync Start */
    {0x1600, 2, slave_4_pdo_entries + 2}, /* Channel 1 */
    {0x1601, 3, slave_4_pdo_entries + 4}, /* Channel 2 */
    {0x1604, 0, NULL}, /* Reserved */
};

ec_sync_info_t slave_4_syncs[] = {
    {0, EC_DIR_INPUT, 1, slave_4_pdos + 0, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 1, slave_4_pdos + 1, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 3, slave_4_pdos + 2, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {4, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {0xff}
};

