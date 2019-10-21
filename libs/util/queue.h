/* mbed Queue Library
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

#ifndef MBED_QUEUE_H
#define MBED_QUEUE_H

#include "mbed.h"

/** queue modle
 *
 * Example:
 * @code
 *
 * #include "mbed.h"
 *
 * // include the queue library
 * #include "queue.h"
 *
 * Serial myPc( USBTX, USBRX );
 * Queue myQueue( 1, 5 );
 *
 * int main() 
 * {
 *    unsigned char   nTemp;
 *   
 *    // display the number of items in the queue
 *    myPc.printf( "Items in queue: %d\r", myQueue.GetNumberOfItems( ));
 *    
 *    // add item to queue
 *    nTemp = 5;
 *    myQueue.Put( &nTemp );
 *    nTemp = 6;
 *    myQueue.Put( &nTemp );
 *    nTemp = 7;
 *    myQueue.Put( &nTemp );
 *    
 *    // display the number of items in the queue
 *    myPc.printf( "Items in queue: %d\r", myQueue.GetNumberOfItems( ));
 *    
 *    // peek at item at the top of the queue
 *    myQueue.Peek( &nTemp );
 *    myPc.printf( "Peek: %d\r", nTemp );
 *    
 *    // get an item from queue
 *    myQueue.Get( &nTemp );
 *    myPc.printf( "Item 0 = %d\r", nTemp );
 *    myQueue.Get( &nTemp );
 *    myPc.printf( "Item 1 = %d\r", nTemp );
 *    
 *    // queue should be empty
 *    if ( !myQueue.Get( &nTemp ))
 *    {
 *        // queue is empty
 *        myPc.printf( "Queue empty!\r" );
 *    }
 * }
 *
 * @endcode
 */

class Queue {
public:
    /** Create a Queue object
     *
     * @param iSize size of the object in queue
     * @param iCount number of items in the queue
     */
    Queue( int iSize, int iCount );

    /** destruction
     *
     */
    virtual ~Queue( void );

    /** Add item to queue
     *
     * @param pvItem item to add
     * @returns true if item added, false if queue full
     */
    bool Put( void* pvItem );

    /** get an item from the queue in an IRQ handler
     *
     * @param pvItem pointer to the tiem to retrieve
     * @returns true if item fetched, false if queue is empty
     */
    bool GetIrq( void* pvItem );
    
    /** Add item to queue from an IRQ handler
     *
     * @param pvItem item to add
     * @returns true if item added, false if queue full
     */
    bool PutIrq( void* pvItem );

    /** get an item from the queue
     *
     * @param pvItem pointer to the tiem to retrieve
     * @returns true if item fetched, false if queue is empty
     */
    bool Get( void* pvItem );
    
    /** get the number of items in the queue
     * 
     * @returns the number of items in the queue
     */
    int GetNumberOfItems( void );
    
    /** peek at the entry at the top of the queue
     * 
     * @returns the entry at the top of the queue
     */
    bool Peek( void* pvItem );    

    /** flush the queue
     *
     */
    void Flush( void );


protected:
    int              m_iSize;        // size of each item in queue
    int              m_iCount;        // number of items in the queue
    unsigned char*   m_pnHead;        // pointer to the head of the queue
    unsigned char*   m_pnTail;        // pointer to the tail of the queue
    unsigned char*   m_pnRdIndex;    // read index
    unsigned char*   m_pnWrIndex;    // write index
    int              m_iLclCount;    // number of items in queue
};
#endif
