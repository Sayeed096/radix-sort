/**
 * @file radsort.h
 * @author Md. Sayeed Al Masud (planetmind@outlook.com)
 * @brief Radix Sort Algorithm with different implementation
 * @version 0.1
 * @date 2022-05-31
 * 
 * @details This sorting program use radix sort algorithm in a little different
 * way. In stead of copy the values from the original list of numbers, it keep 
 * the location/index of the number of the list, and put them into different 
 * buckets according to the values in the list. Then copy the numbers according
 * to sequence of bucket number and sort order.
 * Currently it supports only integer numbers upto 16,777,215 (or 0xFF_FFFF).
 *
 * @remark Since shifting hexadecimal number is more faster than division of 
 * decimal number, so getting digit value of a hexadecimal number is faster 
 * than decimals number.
 * 
 * @copyright Copyright (c) 2022
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// Header guard
#ifndef __RADSORT_H__
#define __RADSORT_H__

// For C++ compiler compatibility
#ifdef __cplusplus
extern "C" {
#endif

// ======================================================================== //
// Include library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// ======================================================================== //
// Define macros
// ======================================================================== //
// Checking after memory allocation
#define check_mem_alloc(x)                                                   \
    if(x == NULL) {                                                          \
        printf("Failed to allocate memory.\n");                              \
        return NULL;                                                         \
    }  //else {  printf("Allocate Memory successfully.\n");  }

// Free allocated memory of a bucket and it's array(fdata)
#define free_bucket(a)                                                       \
    for(uint8_t i = 0; i < 16; ++i) {                                        \
        free(a[i].fdata);                                                    \
        a[i].wp = 0;                                                         \
    }                                                                        \
    free(a);

// ======================================================================== //
// Structure/Union and Type declaration
// ======================================================================== //
/**
 * @brief Special type of FIFO for bucket.
 * @details Define a FIFO in a very specific way to use for implementing bucket 
 * for this program. This structure have two member, one for hold the address 
 * of FIFO(fdata) and another for write pointer (wp). FIFO is a dynamically 
 * allocated memory which size depends on the amount of number of the current
 * bucket. Since, a bucket doesn't need a read pointer, it was not implemented.
 */
typedef struct {
    uint32_t *fdata;       // pointer of array
    uint32_t wp;           // write pointer
} spfifo_t;

// ======================================================================== //
// Function declaration
// ======================================================================== //

/**
 * @brief The function make an array of buckets with positions/indices(ifpl) 
 * of number list for top hierarchy buckets.
 * 
 * @details The function first allocate memory for bucket list, then for the 
 * array for FIFO. After that, get each item of current bucket, get the value
 * of the digit according the radix (100x, 10x, 1x etc.). Next the location/index
 * of that number is store into the new buckets. The bucket No. was previously
 * calculated from the value of digit.
 * 
 * @param num_list Unsorted list
 * @param nl_sz The number of element of num_list
 * @param digit_h Current digit. base 16 or hexadecimal digit
 * @return spfifo_t* Array of buckets
 */
spfifo_t* tp_radix_pos( const uint64_t num_list [], uint32_t nl_sz, 
        uint8_t digit_h );

/**
 * @brief The function make an array of buckets with positions/indices(ifpl) 
 * of number list.
 * 
 * @details The function first allocate memory for bucket list, then for the 
 * array for FIFO. After that, get each item of current bucket, get the value
 * of the digit according the radix (100x, 10x, 1x etc.). Next the location/index
 * of that number is store into the new buckets. The bucket No. was previously
 * calculated from the value of digit.
 * 
 * @param num_list Unsorted list
 * @param pos_list The positions of the unsorted list which are available in
 * current bucket
 * @param pl_sz The number of element of current bucket
 * @param digit_h Current digit. base 16 or hexadecimal digit
 * @return spfifo_t* Array of buckets
 */
spfifo_t* radix_pos( const uint64_t num_list [], uint32_t pos_list [], 
        uint32_t pl_sz, uint8_t digit_h);


/**
 * @brief The function sort unsorted list of integer number using radix sort
 * algorithm.
 * 
 * @details The function first make an array of buckets for top hierarchical
 * level using 1s digit (assumed length of digit are same with leading zeros).
 * Then iterate over all bucket and make low hierarchical buckets. After that
 * make a single sequence of position/index array recursively. Next, allocate
 * memory for sorted list. Then copy the numbers according to the sequence of
 * positions/indices and sort order from unsorted list.
 * The maximum length of digit is 6 in hexadecimal (0xFF_FFFF or 65,535)
 * 
 * @param u_list Unsorted list
 * @param l_size The size of the unsorted list
 * @param sort_order The order of sorting (Ascending or Descending)
 * @return uint64_t* Array of sorted numbers
 */
uint64_t* radix_sort_h4d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order );

/**
 * @brief The function sort unsorted list of integer number using radix sort
 * algorithm.
 * 
 * @details Please see the description of "radix_sort_h4d", line #105
 * The maximum length of digit is 5 in hexadecimal (0xF_FFFF or 1,048,575)
 */
uint64_t* radix_sort_h5d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order );

/**
 * @brief The function sort unsorted list of integer number using radix sort
 * algorithm.
 * 
 * @details Please see the description of "radix_sort_h4d", line #105
 * The maximum length of digit is 6 in hexadecimal (0xFF_FFFF or 16,777,215)
 */
uint64_t* radix_sort_h6d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order );

/**
 * @brief The function sort unsorted list of integer number using radix sort
 * algorithm.
 * 
 * @details Please see the description of "radix_sort_h4d", line #105
 * The maximum length of digit is 7 in hexadecimal (0xFFF_FFFF or 268,435,455)
 */
uint64_t* radix_sort_h7d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order );

/**
 * @brief The function sort unsorted list of integer number using radix sort
 * algorithm.
 * 
 * @details Please see the description of "radix_sort_h4d", line #105
 * The max length of digit is 8 in hexadecimal (0xFFFF_FFFF or 4,294,967,295)
 */
uint64_t* radix_sort_h8d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order );


/**
 * @brief This is the generic function which call other sorting function
 * according to maximum length of digit.
 * 
 * @details The function select sorting function which will be called for
 * sorting according to maximum length of digit of number in unsorted list.
 * 
 * @param u_list Unsorted list
 * @param l_size The size of the unsorted list
 * @param max_digit Maximum length of digit of number in unsorted list
 * @param sort_order The order of sorting (Ascending or Descending order)
 * @return uint64_t* Array of sorted numbers
 */
uint64_t* radix_sort_16b( const uint64_t u_list [], uint32_t l_size, 
        uint8_t max_digit, char sort_order );


/**
 * @brief The function print first and last 4 elements of an array.
 * 
 * @param list The array which elements will be printed
 * @param l_size The size of that array
 * @return void Return nothing
 */
void print_head_tail_list( uint64_t list [], uint32_t l_size );




// Compatibility and header guard
#ifdef __cplusplus
}
#endif
#endif  // __RADSORT_H__