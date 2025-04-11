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

#include "PCA9546A.h"

PCA9546A::PCA9546A( PinName sda, PinName scl, char i2c_address ) 
    : i2c_p( new I2C( sda, scl ) ), i2c( *i2c_p ), i2c_addr( i2c_address ), enable_pattern( 0x00 )
{
    //  do nothing.
    //  leave it in default state.
}

PCA9546A::PCA9546A( I2C &i2c_, char i2c_address )
    : i2c_p( NULL ), i2c( i2c_ ), i2c_addr( i2c_address ), enable_pattern( 0x00 )
{
    //  do nothing.
    //  leave it in default state.
}

PCA9546A::~PCA9546A()
{
    if ( NULL != i2c_p )
        delete  i2c_p;
}

void PCA9546A::on( char channel )
{
    enable_pattern  |= 0x1 << channel;

    bitpattern( enable_pattern );
}

void PCA9546A::off( char channel )
{
    enable_pattern  &= ~(0x1 << channel);

    bitpattern( enable_pattern );
}

void PCA9546A::bitpattern( char pattern )
{
    i2c.write( i2c_addr, &pattern, 1 );
}
