#ifndef	_USI_TWI_SLAVE_H_
#define	_USI_TWI_SLAVE_H_

/*
Copyright: Erik Slagter <erik@slagter.name>
License: GPLv2

Derived from works by:
	Atmel (Application note 312, licensing unknown, probably public domain)
	Donald R. Blake (licensing unknown, probably public domain)
*/

#include <stdint.h>

enum
{
	USI_TWI_BUFFER_SIZE = 4
};

void		usi_twi_slave(uint8_t slave_address, uint8_t use_sleep,
				void (*data_callback)(uint8_t input_buffer_length, const uint8_t *input_buffer,
				uint8_t *output_buffer_length, uint8_t *output_buffer),
				void (*idle_callback)(void));

void		usi_twi_enable_stats(uint8_t onoff);
uint16_t	usi_twi_stats_start_conditions(void);
uint16_t	usi_twi_stats_stop_conditions(void);
uint16_t	usi_twi_stats_error_conditions(void);
uint16_t	usi_twi_stats_overflow_conditions(void);
uint16_t	usi_twi_stats_local_frames(void);
uint16_t	usi_twi_stats_idle_calls(void);

#endif
