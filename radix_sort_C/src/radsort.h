/**
 * @file radsort.h
 * @author Md. Sayeed Al Masud (planetmind@outlook.com)
 * @brief Radix Sort Algorithm with different implementation
 * @version 0.3
 * @date 2025-08-22
 * 
 * @details This sorting program use radix sort algorithm in a little different
 * way. In stead of copy the values from the original list of numbers, it keep 
 * the location/index of the number of the list, and put them into different 
 * buckets according to the values in the list. Then copy the numbers according
 * to sequence of bucket number and sort order.
 * Currently it supports only unsigned integer numbers upto 
 * 18,446,744,073,709,551,615 (or 0xFFFF_FFFF_FFFF_FFFF).
 *
 * @remark Since shifting hexadecimal number is more faster than division of 
 * decimal number, so getting digit value of a hexadecimal number is faster 
 * than decimals number.
 * 
 * @copyright Copyright (c) 2025
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
// ======================================================================== //
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// ======================================================================== //
// Define macros
// ======================================================================== //
// #define DEBUG 
// #define DEBUG_L2 

// #define ASYNC_SORT_ENABLED  // Enable multi thread sorting feature

/**
 * @brief The macro for number of buckets
 * @details This macro define the number of buckets for radix sort. Since this
 * program is designed for hexadecimal number, so the number of buckets is
 * 16. If you want to use different number, then you must change it.
 */
#define NUMBER_OF_BUCKETS 16

/**
 * @brief Check the success of memory allocation
 * @details This macro check the success of memory allocation. If the
 * memory allocation is failed, it will print an error message and return
 * NULL pointer from the function.
 * @param x The pointer which will be checked
 */
#define check_mem_alloc(x)                                                   \
    if(x == NULL) {                                                          \
        printf("Failed to allocate memory.\n");                              \
        return NULL;                                                         \
    }  //else {  printf("Allocate Memory successfully.\n");  }

/**
 * @brief Check the success of memory allocation
 * @details This macro check the success of memory allocation. If the
 * memory allocation is failed, it will print an error message, then free the 
 * previously allocated memory and then return NULL pointer from the function.
 * @param x The pointer which will be checked
 * @param y The pointer which will be freed
 */
#define check_mem_alloc2(x, y)                                               \
    if(x == NULL) {                                                          \
        printf("Failed to allocate memory.\n");                              \
        free(y); y = NULL;  /*free previously allocated memory*/             \
        return NULL;                                                         \
    }  //else {  printf("Allocate Memory successfully.\n");  }

/**
 * @brief Check the success of memory allocation for a specific bucket
 * @details This macro check the success of memory allocation for a specific
 * bucket. If the memory allocation is failed, it will print an error message
 * and free the previously allocated memory for all buckets, then return NULL
 * pointer from the function.
 * @param x The pointer of bucket array which will be checked
 * @param n The index of the bucket up to which memory was allocated
 */
#define check_mem_alloc_partial(x, n)                                        \
    if(x[n].fdata == NULL) {                                                 \
        printf("Failed to allocate memory for bucket %d.\n", n);             \
        for (uint8_t i = 0; i < n; ++i) {                                    \
            free(x[i].fdata);                                                \
            x[i].fdata = NULL;                                               \
        }                                                                    \
        free(x); x = NULL;                                                   \
        return NULL;                                                         \
    }  //else {  printf("Allocate Memory successfully.\n");  }


/**
 * @brief Free allocated memory of buckets and their array(fdata)
 * @details This macro iterarte through the bucket array and free the memory
 * of each bucket's FIFO array(fdata). After that, free the memory of the
 * bucket itself. After freeing the memory, it will set the pointer to NULL to
 * avoid dangling pointer or use-after-free error.
 * @param a The bucket array which will be freed
 * @param n The number of buckets from the begining
 */
#define free_buckets(a, n)                                                   \
    for(uint8_t i = 0; i < n; ++i) {                                         \
        free(a[i].fdata);                                                    \
        a[i].fdata = NULL;                                                   \
        a[i].wp = 0;                                                         \
    }                                                                        \
    free(a); a = NULL;


#ifdef ASYNC_SORT_ENABLED
/**
 * @brief Check memory allocation for sequence of indices FIFO array.
 * @details This macro checks if the memory allocation for the sequence of
 * indices FIFO array (soi_fifos) at a specific index has succeeded. If the
 * allocation fails, it prints an error message, frees all previously allocated
 * memory for the sequence of indices FIFOs and the top-level buckets, and
 * returns NULL from the function.
 * @param sf The pointer to the sequence of indices FIFO array (soi_fifos)
 * @param idx The index of the current FIFO being checked
 * @param bl The pointer to the top-level bucket array to be freed on failure
 */
#define check_soi_f_fdata(sf, idx, bl)                                       \
    if (sf[idx].fdata == NULL) {                                             \
        printf("Memory allocation failed for soi_fifos[%d]\n", idx);         \
        free_buckets(sf, idx)                                                \
        free_buckets(bl, NUMBER_OF_BUCKETS);  /*Free top level buckets*/     \
        return NULL;                                                         \
    }

/**
 * @brief Fully free two buckets
 * @details This macro call other macro to free the memory allocation of two 
 * bucket entirly
 * @param bkt1 The pointer of first bucket
 * @param bkt1 The pointer of second bucket
 */
#define free_2buckets(bkt1, bkt2)  \
    free_buckets(bkt1, NUMBER_OF_BUCKETS)  \
    free_buckets(bkt2, NUMBER_OF_BUCKETS)  \

#endif


// ======================================================================== //
// Structure/Union and Type declaration
// ======================================================================== //
/**
 * @brief Special type of FIFO for bucket.
 * @details Define a FIFO in a very specific way to use for implementing bucket
 * for this program. This structure have two member, one for hold the address
 * of FIFO(fdata) and another for write pointer (wp). FIFO is a dynamically
 * allocated memory which size depends on the amount of number of the current
 * bucket. Since, a bucket doesn't need a read pointer, it wasn't included.
 */
typedef struct {
    uint32_t *fdata; // pointer of array
    uint32_t wp;     // write pointer
} spfifo_t;

// ======================================================================== //
// Function declaration
// ======================================================================== //

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
 * current bucket or NULL pointer if it is the top level bucket
 * @param nl_sz The number of element of current bucket or num_list
 * @param digit_h Current digit's place value. base 16 or hexadecimal number
 * @return spfifo_t* Array of buckets
 */
// static spfifo_t* radix_pos( const uint64_t num_list [],
//         const uint32_t *pos_list, uint32_t nl_sz, const uint8_t digit_h );

/**
 * @brief The function recursively check and merge the indices of all buckets
 * into a single array.
 *
 * @details The function first check number of item of the current bucket, if 
 * single item, then store the index and skip the rest. For multiple items, 
 * it check the current digit's place value. If it is less than or equal to 1, 
 * then it will merge the indices of the current bucket into the array. If 
 * it's greater than 1, then it will call itself recursively with the next 
 * lower digit's place value. This will continue until the digit's place value 
 * is less than or equal to 1.
 *
 * @param u_list Unsorted list
 * @param cur_bucket The current level of buckets which will be checked and
 * merged
 * @param cur_digit_h The current digit's place value
 * @param soi_f The fifo pointer which contain an array of indices where the
 * merged indices will be stored and write point to count the stored items
 *
 * @return void
 */
// static void recur_bucket_merge(const uint64_t u_list[],
//         const spfifo_t *cur_buckets, const uint8_t cur_digit_h,
//         spfifo_t *soi_f );

/**
 * @brief The function sort unsorted list of integer numbers using radix sort
 * algorithm.
 *
 * @details The function first make an array of buckets for top hierarchical
 * level using 1s digit (assumed length of digit are same with leading zeros).
 * Then iterate over all bucket and make low hierarchical buckets. After that
 * make a single sequence of position/index array recursively. Next, allocate
 * memory for sorted list. Then copy the numbers according to the sequence of
 * positions/indices and sort order from unsorted list.
 * The maximum length of digit is 4 in hexadecimal (0xFFFF or 65,535)
 *
 * @param u_list Unsorted list
 * @param l_size The size of the unsorted list
 * @param sort_order The order of sorting (Ascending or Descending)
 * @return uint64_t* Array of sorted numbers
 */
uint64_t *radix_sort_h4d(const uint64_t u_list[], uint32_t l_size,
                            char sort_order);

/**
 * @brief The function recursively sort unsorted list of integer number using
 * radix sort algorithm.
 *
 * @details This function first make an array of buckets for top hierarchical
 * level using the maximum length of digit (assumed length of digit of all
 * numbers are same with leading zeros). Then it will iterate over all buckets
 * recursivly and make low hierarchical buckets. After that, it will make a
 * single sequence of position/index array recursively. Next, it will allocate
 * memory for sorted list. Then it will copy the numbers from unsorted list
 * according to the sequence of positions/indices and sort order.
 * The maximum length of digit is 16 in hexadecimal (0xFFFF_FFFF_FFFF_FFFF or
 * 18,446,744,073,709,551,615)
 *
 * @param u_list Unsorted list
 * @param l_size The size of the unsorted list
 * @param digit_h_N The maximum length of digit of number in unsorted list
 * @param sort_order The order of sorting (Ascending or Descending order)
 *
 * @return uint64_t* Array of sorted numbers
 */
uint64_t *recur_radix_sort_hNd(const uint64_t u_list[], uint32_t l_size,
                                uint8_t digit_h_N, char sort_order);



#ifdef ASYNC_SORT_ENABLED
#include <pthread.h>
#include <string.h>
/**
 * @brief Structure for thread function arguments in asynchronous radix sort.
 * @details This structure is used to pass multiple arguments to a thread 
 * function when performing asynchronous radix sort. It contains pointers to 
 * the unsorted list, the current bucket, the current digit's place value, and 
 * the sequence of indices FIFO where the merged indices will be stored.
 */
typedef struct _thread_args_t {
    const uint64_t *u_list;  // Pointer to the unsorted list of numbers
    const spfifo_t *bucket;  // Pointer to the current bucket being processed
    uint8_t digit_h_N;  // The current digit's place value
    spfifo_t *soi_f;  // Pointer to the sequence of indices for store indices
} thread_args_t;

/**
 * @brief The function asynchronously sort unsorted list of integer numbers 
 * using radix sort algorithm.
 *
 * @details This function performs radix sort in a multi-threaded 
 * (asynchronous) manner. It first creates an array of top-level buckets based 
 * on the maximum digit length. For each non-empty bucket, it spawns a 
 * separate thread to recursively process and merge indices using a thread 
 * argument structure. After all threads complete, it merges the sorted 
 * indices from all buckets into a single sorted list. This approach leverages 
 * parallelism to improve sorting performance for large datasets. The maximum 
 * length of digit is 16 in hexadecimal (0xFFFF_FFFF_FFFF_FFFF or 
 * 18,446,744,073,709,551,615).
 *
 * @param u_list Unsorted list
 * @param l_size The size of the unsorted list
 * @param digit_h_N The maximum length of digit of number in unsorted list
 * @param sort_order The order of sorting (Ascending or Descending order)
 *
 * @return uint64_t* Array of sorted numbers
 */
uint64_t* async_radix_sort_hNd( const uint64_t u_list [], uint32_t l_size, 
        uint8_t digit_h_N, char sort_order );


#endif  // ASYNC_SORT


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