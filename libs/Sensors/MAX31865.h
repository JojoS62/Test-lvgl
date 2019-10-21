/**************************************************************************
 * Arduino driver library for the MAX31865.
 *
 * Copyright (C) 2015 Ole Wolf <wolf@blazingangles.com>
 *
 *
 * Wire the circuit as follows, assuming that level converters have been
 * added for the 3.3V signals:
 *
 *    Arduino Uno            -->  MAX31865
 *    ------------------------------------
 *    CS: any available pin  -->  CS
 *    MOSI: pin 11           -->  SDI (mandatory for hardware SPI)
 *    MISO: pin 12           -->  SDO (mandatory for hardware SPI)
 *    SCK: pin 13            -->  SCLK (mandatory for hardware SPI)
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/


#ifndef _MAX31865_H
#define _MAX31865_H

#include <stdint.h>
#include "mbed.h"

#define MAX31865_FAULT_HIGH_THRESHOLD  ( 1 << 7 )
#define MAX31865_FAULT_LOW_THRESHOLD   ( 1 << 6 )
#define MAX31865_FAULT_REFIN           ( 1 << 5 )
#define MAX31865_FAULT_REFIN_FORCE     ( 1 << 4 )
#define MAX31865_FAULT_RTDIN_FORCE     ( 1 << 3 )
#define MAX31865_FAULT_VOLTAGE         ( 1 << 2 )

#define MAX31865_FAULT_DETECTION_NONE      ( 0x00 << 2 )
#define MAX31865_FAULT_DETECTION_AUTO      ( 0x01 << 2 )
#define MAX31865_FAULT_DETECTION_MANUAL_1  ( 0x02 << 2 )
#define MAX31865_FAULT_DETECTION_MANUAL_2  ( 0x03 << 2 )



/* RTD data, RTD current, and measurement reference
   voltage. The ITS-90 standard is used; other RTDs
   may have coefficients defined by the DIN 43760 or
   the U.S. Industrial (American) standard. */
#define RTD_A_ITS90         3.9080e-3
#define RTD_A_USINDUSTRIAL  3.9692e-3
#define RTD_A_DIN43760      3.9848e-3
#define RTD_B_ITS90         -5.870e-7
#define RTD_B_USINDUSTRIAL  -5.8495e-7
#define RTD_B_DIN43760      -5.8019e-7

#define RTD_A_NI            5.405e-3   
#define RTD_B_NI            6.65e-6

/* RTD coefficient C is required only for temperatures
   below 0 deg. C.  The selected RTD coefficient set
   is specified below. */
#define SELECT_RTD_HELPER(x) x
#define SELECT_RTD(x) SELECT_RTD_HELPER(x)
#define RTD_A         SELECT_RTD(RTD_A_NI)
#define RTD_B         SELECT_RTD(RTD_B_NI)
/*
 * The reference resistor on the hardware; see the MAX31865 datasheet
 * for details.  The values 400 and 4000 Ohm are recommended values for
 * the PT100 and PT1000.
 */
#define RTD_RREF_PT100         400 /* Ohm */
#define RTD_RREF_PT1000       4000 /* Ohm */
/*
 * The RTD resistance at 0 degrees Celcius.  For the PT100, this is 100 Ohm;
 * for the PT1000, it is 1000 Ohm.
 */
#define RTD_RESISTANCE_PT100   100 /* Ohm */
#define RTD_RESISTANCE_PT1000 1000 /* Ohm */

#define RTD_ADC_RESOLUTION  ( 1u << 15 ) /* 15 bits */


/* See the main (MAX31865.cpp) file for documentation of the class methods. */
class MAX31865_RTD
{
public:
  enum ptd_type { RTD_PT100, RTD_PT1000 };

  MAX31865_RTD( ptd_type type,PinName mosi, PinName miso, PinName sclk, PinName nss);
  void configure( bool v_bias, bool conversion_mode, bool one_shot, bool three_wire,
                  uint8_t fault_cycle, bool fault_clear, bool filter_50hz,
                  uint16_t low_threshold, uint16_t high_threshold );
  uint8_t read_all( );
  double temperature( ) const;
  uint8_t configuration( ) const { return( (measured_configuration == configuration_control_bits)? configuration_control_bits:measured_configuration); }
  uint8_t status( ) const { return( measured_status ); }
  uint16_t low_threshold( ) const { return( measured_low_threshold ); }
  uint16_t high_threshold( ) const  { return( measured_high_threshold ); }
  uint16_t raw_resistance( ) const { return( measured_resistance ); }
  double resistance( ) const
  {
    const double rtd_rref =
      ( this->type == RTD_PT100 ) ? (double)RTD_RREF_PT100 : (double)RTD_RREF_PT1000;
    return( (double)raw_resistance( ) * rtd_rref / (double)RTD_ADC_RESOLUTION );
  }

private:

        /*!
    * SPI Interface
    */
    SPI spi; // mosi, miso, sclk
    DigitalOut nss;
  /* Our configuration. */
 // uint8_t  cs_pin;
  ptd_type type;
  uint8_t  configuration_control_bits;
  uint16_t configuration_low_threshold;
  uint16_t configuration_high_threshold;
  void reconfigure( );

  /* Values read from the device. */
  uint8_t  measured_configuration;
  uint16_t measured_resistance;
  uint16_t measured_high_threshold;
  uint16_t measured_low_threshold;
  uint8_t  measured_status;
};

#endif /* _MAX31865_H */

