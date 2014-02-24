led
===

A test EtherCat/EtherLab based application that turns LEDs on and off (based on EtherLab/examples/user/main.c).

The code here assumes the following setup for the ethercat slaves (`ethercat slaves` command):

	0  0:0  PREOP  +  EK1100 EtherCAT-Koppler (2A E-Bus)
	1  0:1  PREOP  +  EL2202 2K. Dig. Ausgang 24V, 0.5A
	2  0:2  PREOP  +  EL1252 2K. Fast Dig. Eingang 24V, 1us, DC Latch
	3  0:3  PREOP  +  EL1252 2K. Fast Dig. Eingang 24V, 1us, DC Latch
	4  0:4  PREOP  +  EL2252 2K. Dig. Ausgang 24V, 0.5A, DC Time Stamp

The application sets a timer to run the `cyclic_task()` routine `FREQUENCY` times per second.
This routine should normally check the current input and output states.
A call to `ecrt_master_send(master)` also needs to be made this frequently to avoid a watchdog timeout.
Note that the EL2202 will reset its outputs to 0 after 100ms if not updated.

