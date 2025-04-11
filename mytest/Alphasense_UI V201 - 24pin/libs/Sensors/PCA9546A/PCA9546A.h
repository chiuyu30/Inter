/**
 *  PCA9546A library
 *
 *  @author  Tedd OKANO
 *  @version 0.1
 *  @date    Feb-2015
 *
 *  PCA9546A: an I2C bus switch control library
 *
 *  The PCA9546A is a quad bidirectional translating switch controlled 
 *  via the I2C-bus. The SCL/SDA upstream pair fans out to four downstream 
 *  pairs, or channels. Any individual SCx/SDx channel or combination of 
 *  channels can be selected, determined by the contents of the programmable 
 *  control register.
 *
 *  For more information about PCA9546A:
 *    http://www.nxp.com/documents/data_sheet/PCA9546A.pdf
 *
 */

#ifndef MBED_PCA9546A_H
#define MBED_PCA9546A_H

#include "mbed.h"

/** PCA9546A class
 *
 *  PCA9546A: an I2C bus switch control library
 *
 *  The PCA9546A is a quad bidirectional translating switch controlled 
 *  via the I2C-bus. The SCL/SDA upstream pair fans out to four downstream 
 *  pairs, or channels. Any individual SCx/SDx channel or combination of 
 *  channels can be selected, determined by the contents of the programmable 
 *  control register.
 *
 *  For more information about PCA9546A:
 *    http://www.nxp.com/documents/data_sheet/PCA9546A.pdf
 *
 *  Example:
 *  @code
 *  #include "mbed.h"
 *  #include "LM75B.h"
 *  #include "PCA9546A.h"
 *  
 *  PCA9546A    i2c_sw( p28, p27, 0xE0 );
 *  
 *  int main()
 *  {
 *      //  all PCA9546A's downstream ports are OFF after power-up and hardware-reset
 *      
 *      i2c_sw.on( 0 );     //  turn-ON  the channel 0
 *  
 *      LM75B   tmp0( p28, p27 );   //  making instance after a branch of I2C bus (which is connecting the LM75B) enabled
 *  
 *      i2c_sw.off( 0 );    //  turn-OFF the channel 0
 *      i2c_sw.on( 1 );     //  turn-ON  the channel 1
 *  
 *      LM75B   tmp1( p28, p27 );   //  making instance after a branch of I2C bus (which is connecting the LM75B) enabled
 *  
 *      while(1) {
 *          
 *          i2c_sw.off( 1 );    //  turn-OFF the channel 1
 *          i2c_sw.on( 0 );     //  turn-ON  the channel 0
 *          printf( "%.3f\r\n", tmp0.read() );
 *  
 *          i2c_sw.off( 0 );    //  turn-OFF the channel 0
 *          i2c_sw.on( 1 );     //  turn-ON  the channel 1
 *          printf( "%.3f\r\n", tmp1.read() );
 *  
 *          wait( 1.0 );
 *      }
 *  }
 *  @endcode
 */

class PCA9546A
{
public:

    /** Create a PCA9546A instance connected to specified I2C pins with specified address
     *
     * @param sda I2C-bus SDA pin
     * @param scl I2C-bus SCL pin
     * @param i2c_address I2C-bus address (default: 0xE0)
     */
    PCA9546A( PinName sda, PinName scl, char i2c_address = 0xE0 );

    /** Create a PCA9546A instance connected to specified I2C pins with specified address
     *
     * @param &i2c_ I2C object (instance)
     * @param i2c_address I2C-bus address (default: 0xE0)
     */
    PCA9546A( I2C &i2c_, char i2c_address = 0xE0 );

    /** Destructor of PCA9546A
     */
    ~PCA9546A();

    /** Turning-ON a channel
     *
     *  Switching a channel ON
     *
     * @param channel channel number
     */
    void on( char channel );

    /** Turning-OFF a channel
     *
     *  Switching a channel OFF
     *
     * @param channel channel number
     */
    void off( char channel );

    /** Switching by bit pattern
     *
     *  Switch setting by bit pattern. 
     *  Set '1' for ON, set '0' for OFF
     *  LSB is channel 0. Channels 1, 2 and 3 are mapped in shifted position from LSB
     *
     *  @param pattern ON/OFF bit pattern for channels
     */
    void bitpattern( char pattern );

private:
    I2C     *i2c_p;
    I2C     &i2c;
    char    i2c_addr;
    char    enable_pattern;
};

#endif  //  MBED_PCA9546A_H