/* mbed queue Library
 * Copyright (c) 2010 William Basser ( wbasser [at] gmail [dot] com )
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// inlcude the defs
#include "queue.h"

// construction 
Queue::Queue( int iSize, int iCount )
{
    // allocate space for the queue
    if (( m_pnHead = ( unsigned char* )malloc( iSize * iCount )) != NULL )
    {
        // set the tail/clear the indices/set the size and length
        m_pnTail     = m_pnHead + ( iSize * iCount );
        m_iSize     = iSize;
        m_iCount     = iCount;
        m_pnRdIndex    = m_pnHead;
        m_pnWrIndex    = m_pnHead;
        m_iLclCount    = 0;
    }
}

// destruction
Queue::~Queue( )
{
    // free the memory
    free( m_pnHead );
}

// put an item into the queue
bool Queue::Put( void* pvItem )
{
    bool bResult = false;
    
    // check for room
    if ( m_iLclCount < m_iCount )
    {
        // copy the item/adjust the pointer/check for overflow
        memcpy( m_pnWrIndex, pvItem, m_iSize );

        __disable_irq( );
        m_pnWrIndex += m_iSize;
        if ( m_pnWrIndex >= m_pnTail )
            m_pnWrIndex = m_pnHead;

        // increment the count
        m_iLclCount++;
        __enable_irq( );

        // set the result to 0k
        bResult = true;
    }

    // return the status
    return( bResult );
}

// get an item from the queue
bool Queue::Get( void* pvItem )
{
    bool bResult = false;
    
    // check for room
    if ( m_iLclCount != 0 )
    {
        // copy the item/adjust the pointer/check for overflow
        memcpy( pvItem, m_pnRdIndex, m_iSize );

        __disable_irq( );
        m_pnRdIndex += m_iSize;
        if ( m_pnRdIndex >= m_pnTail )
            m_pnRdIndex = m_pnHead;

        // decrement the count
        m_iLclCount--;
        __enable_irq( );
        // set the result to 0k
        bResult = true;
    }

    // return the status
    return( bResult );
}

// put an item into the queue
bool Queue::PutIrq( void* pvItem )
{
    bool bResult = false;
    
    // check for room
    if ( m_iLclCount < m_iCount )
    {
        // copy the item/adjust the pointer/check for overflow
        memcpy( m_pnWrIndex, pvItem, m_iSize );
        m_pnWrIndex += m_iSize;
        if ( m_pnWrIndex >= m_pnTail )
            m_pnWrIndex = m_pnHead;

        // increment the count
        m_iLclCount++;

        // set the result to 0k
        bResult = true;
    }

    // return the status
    return( bResult );
}

// get an item from the queue
bool Queue::GetIrq( void* pvItem )
{
    bool bResult = false;
    
    // check for room
    if ( m_iLclCount != 0 )
    {
        // copy the item/adjust the pointer/check for overflow
        memcpy( pvItem, m_pnRdIndex, m_iSize );
        m_pnRdIndex += m_iSize;
        if ( m_pnRdIndex >= m_pnTail )
            m_pnRdIndex = m_pnHead;

        // decrement the count
        m_iLclCount--;

        // set the result to 0k
        bResult = true;
    }

    // return the status
    return( bResult );
}

// get the number of items in the queue
int Queue::GetNumberOfItems( void )
{
    // return the count
    return( m_iLclCount );
}

// peeks at the item at the top of the queue
bool Queue::Peek( void* pvItem )
{
    bool bResult = false;
    
    // check for room
    if ( m_iLclCount != 0 )
    {
        // copy the item
        memcpy( pvItem, m_pnRdIndex, m_iSize );

        // set the result to 0k
        bResult = true;
    }

    // return the status
    return( bResult );
}

// flush all items from the queue
void Queue::Flush( void )
{
    // reset the indices
    m_iLclCount = 0;
    m_pnRdIndex = m_pnWrIndex = m_pnHead;
}

