/**
 * @file radsort.c
 * @author Md. Sayeed Al Masud (planetmind@outlook.com)
 * @brief Radix Sort Algorithm
 * @version 0.1
 * @date 2022-01-08
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

// ======================================================================== //
#include "radsort.h"

// ======================================================================== //
// Global variables
// ======================================================================== //


// ======================================================================== //
// Description of all functions
// ======================================================================== //
// ------------------------------------------------------------------------ //
// Make array of buckets with positions/indices of number list
spfifo_t* radix_pos( const uint64_t num_list [], uint32_t pos_list [], 
        uint32_t pl_sz, uint8_t digit_h ) 
{
    // Declare fifo struct type array with size of 16 for 16 bucket
    // Use calloc() to avoid garbage value
    spfifo_t *indices_list = calloc( 16, sizeof(spfifo_t));   
    // Call C-macro function to check the success of dynamic memory allocation
    check_mem_alloc(indices_list);

    // Allocate memory for fifo array(.fdata) of each bucket
    for(uint8_t i = 0; i < 16; ++i) { 
        indices_list[i].fdata = malloc( sizeof(uint32_t) * pl_sz);
        check_mem_alloc(indices_list[i].fdata);
    }
    
    // Get each item of current bucket and distribute indices into new buckets
    for( uint32_t i = 0; i < pl_sz; ++i) {
        // Take a position from pos_list (index_from_pos_list=ifpl)
        uint32_t ifpl = pos_list[i];

        // Determine the bucket number by right shift the number by 
        // (digit-1)*4 bits, then get first digit only
        uint8_t bucket_num = (num_list[ifpl] >> ((digit_h-1) << 2) ) 
                             & 0x000000000000000f;

        // Keep the index(ifpl) into the array of a new bucket
        indices_list[bucket_num].fdata[indices_list[bucket_num].wp] = ifpl;
        indices_list[bucket_num].wp += 1;

        #ifdef DEBUG_L2
        // For show the current loop data and status changes
        printf("[%d]:\tDigit:%d, Cur Pos:%u,\tValue:%8lu, Bucket No.: %x ",
            i, digit_h, ifpl, num_list[ifpl], bucket_num);
        printf("FIFO pt:%4d, Indices inCurBucket: ",
            indices_list[bucket_num].wp);
        for (uint8_t t = 0; t < indices_list[bucket_num].wp; ++t)
            printf("%2u ", indices_list[bucket_num].fdata[t]);
        puts("");  // A new line for each loop
        #endif
    }

    return indices_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Make array of buckets with positions/indices of number list for top level
spfifo_t* tp_radix_pos( const uint64_t num_list [], uint32_t nl_sz,
        uint8_t digit_h )
{
    // Declare fifo struct type array with size of 16 for 16 bucket
    // Use calloc() to avoid garbage value
    spfifo_t *indices_list = calloc( 16, sizeof(spfifo_t));   
    // Call C-macro function to check the success of dynamic memory allocation
    check_mem_alloc(indices_list);

    // Allocate memory for fifo array(.fdata) of each bucket
    for(uint8_t i = 0; i < 16; ++i) { 
        indices_list[i].fdata = malloc( sizeof(uint32_t) * nl_sz);
        check_mem_alloc(indices_list[i].fdata);
    }
    
    // Get each item of current bucket and distribute indices into new buckets
    for( uint32_t i = 0; i < nl_sz; ++i) {
        // Determine the bucket number by right shift the number by 
        // (digit-1)*4 bits, then get first digit only
        uint8_t bucket_num = (num_list[i] >> ((digit_h-1) << 2) ) 
                             & 0x000000000000000f;

        // Keep the index into the array of a new bucket
        indices_list[bucket_num].fdata[indices_list[bucket_num].wp] = i;
        indices_list[bucket_num].wp += 1;

        #ifdef DEBUG_L2
        // For show the current loop data and status changes
        printf("[%d]:\tDigit:%d, Cur Pos:%u,\tValue:%8lu, Bucket No.: %x ",
            i, digit_h, i, num_list[i], bucket_num);
        printf("FIFO pt:%4d, Indices inCurBucket: ",
            indices_list[bucket_num].wp);
        for (uint8_t t = 0; t < indices_list[bucket_num].wp; ++t)
            printf("%2u ", indices_list[bucket_num].fdata[t]);
        puts("");  // A new line for each loop
        #endif
    }

    return indices_list;
}
// ------------------------------------------------------------------------ //



// ======================================================================== //
// Sort unsorted list of integer number using radix sort algorithm
// ------------------------------------------------------------------------ //
// Maximum digit = 4
uint64_t* radix_sort_h4d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order )
{
    // Declare an array for merging indices and index of that array 
    // (ios=index of 'sequence_of_indices')
    uint32_t *sequence_of_indices = malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(sequence_of_indices);
    static uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl4 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit // For now, 4
    spfifo_t *bucket_l4 = tp_radix_pos(u_list, l_size, 4);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl3 = 0; bl3 < 16; ++bl3) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl3:%d --------\n", bl3);
        #endif
        if( bucket_l4[bl3].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl2 = 0; bl2 < 16; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
            if( bucket_l3[bl2].wp == 0 ) continue;
            spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

            for(uint8_t bl1 = 0; bl1 < 16; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
                if( bucket_l2[bl1].wp == 0 ) continue;
                spfifo_t *bucket_l1 = radix_pos(u_list, bucket_l2[bl1].fdata, bucket_l2[bl1].wp, 1);

                // Merge the indices into a fifo sequentially
                for(uint8_t m = 0; m < 16; ++m) {
                    for(uint8_t n = 0; n < bucket_l1[m].wp; ++n) {
                        // Take the index from the bucket and keep into the fifo
                        sequence_of_indices[ios] = bucket_l1[m].fdata[n];  ios += 1;
                        // printf("Number of merged indices(ios): %u\n", ios);
                    }
                }  
                free_bucket(bucket_l1);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
            }  
            free_bucket(bucket_l2);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
        }
        free_bucket(bucket_l3);
        #ifdef DEBUG
        printf("======== End  of bucket level bl3:%d =======\n", bl3);
        #endif
    }  
    free_bucket(bucket_l4);
    #ifdef DEBUG
    puts("======== End  of bucket level bl4 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);  check_mem_alloc(s_list);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        // Loop over the fifo in reverse order
        for(int64_t i = ios-1; i >= 0; --i)  s_list[i] = u_list[sequence_of_indices[i]];
    } else{
        if(sort_order != 'a')  printf("Wrong sort order input. Default ascending order use.\n");
        // Loop over the fifo forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = 5
uint64_t* radix_sort_h5d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order ) 
{
    // Declare an array for merging indices and index of that array 
    // (ios=index of 'sequence_of_indices')
    uint32_t *sequence_of_indices = malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(sequence_of_indices);
    static uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl5 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit (= 5)
    spfifo_t *bucket_l5 = tp_radix_pos(u_list, l_size, 5);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl4 = 0; bl4 < 16; ++bl4) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl4:%d --------\n", bl4);
        #endif
        if( bucket_l5[bl4].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l4 = radix_pos(u_list, bucket_l5[bl4].fdata, bucket_l5[bl4].wp, 4);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl3 = 0; bl3 < 16; ++bl3) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d --------\n", bl5, bl4, bl3);
            if( bucket_l4[bl3].wp == 0 ) continue;
            spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

            for(uint8_t bl2 = 0; bl2 < 16; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
                if( bucket_l3[bl2].wp == 0 ) continue;
                spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

                for(uint8_t bl1 = 0; bl1 < 16; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
                    if( bucket_l2[bl1].wp == 0 ) continue;
                    spfifo_t *bucket_l1 = radix_pos(u_list, bucket_l2[bl1].fdata, bucket_l2[bl1].wp, 1);

                    // Merge the indices into a fifo sequentially
                    for(uint8_t m = 0; m < 16; ++m) {
                        for(uint8_t n = 0; n < bucket_l1[m].wp; ++n) {
                            // Take the index from the bucket and keep into the fifo
                            sequence_of_indices[ios] = bucket_l1[m].fdata[n];  ios += 1;
                            // printf("Number of merged indices(ios): %u\n", ios);
                        }
                    }  
                    free_bucket(bucket_l1);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
                }  
                free_bucket(bucket_l2);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
            }  
            free_bucket(bucket_l3);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d =======\n", bl5, bl4, bl3);
        }  
        free_bucket(bucket_l4);
        #ifdef DEBUG
        printf("======== End  of bucket level bl4:%d =======\n", bl4);
        #endif
    }  
    free_bucket(bucket_l5);
    #ifdef DEBUG
    puts("======== End  of bucket level bl5 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);  check_mem_alloc(s_list);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        // Loop over the fifo in reverse order
        for(int64_t i = ios-1; i >= 0; --i)  s_list[i] = u_list[sequence_of_indices[i]];
    } else{
        if(sort_order != 'a')  printf("Wrong sort order input. Default ascending order use.\n");
        // Loop over the fifo forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = 6
uint64_t* radix_sort_h6d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order ) 
{
    // Declare an array for merging indices and index of that array 
    // (ios=index of 'sequence_of_indices')
    uint32_t *sequence_of_indices = malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(sequence_of_indices);
    static uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl6 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit // For now, 6
    spfifo_t *bucket_l6 = tp_radix_pos(u_list, l_size, 6);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl5 = 0; bl5 < 16; ++bl5) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl5:%d --------\n", bl5);
        #endif
        if( bucket_l6[bl5].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l5 = radix_pos(u_list, bucket_l6[bl5].fdata, bucket_l6[bl5].wp, 5);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl4 = 0; bl4 < 16; ++bl4) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d --------\n", bl5, bl4);
            if( bucket_l5[bl4].wp == 0 ) continue;
            spfifo_t *bucket_l4 = radix_pos(u_list, bucket_l5[bl4].fdata, bucket_l5[bl4].wp, 4);

            #ifdef DEBUG_L2
                getchar();
            #endif

            for(uint8_t bl3 = 0; bl3 < 16; ++bl3) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d --------\n", bl5, bl4, bl3);
                if( bucket_l4[bl3].wp == 0 ) continue;
                spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

                for(uint8_t bl2 = 0; bl2 < 16; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
                    if( bucket_l3[bl2].wp == 0 ) continue;
                    spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

                    for(uint8_t bl1 = 0; bl1 < 16; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
                        if( bucket_l2[bl1].wp == 0 ) continue;
                        spfifo_t *bucket_l1 = radix_pos(u_list, bucket_l2[bl1].fdata, bucket_l2[bl1].wp, 1);

                        // Merge the indices into a fifo sequentially
                        for(uint8_t m = 0; m < 16; ++m) {
                            for(uint8_t n = 0; n < bucket_l1[m].wp; ++n) {
                                // Take the index from the bucket and keep into the fifo
                                sequence_of_indices[ios] = bucket_l1[m].fdata[n];  ios += 1;
                                // printf("Number of merged indices(ios): %u\n", ios);
                            }
                        }  
                        free_bucket(bucket_l1);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
                    }  
                    free_bucket(bucket_l2);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
                }  
                free_bucket(bucket_l3);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d =======\n", bl5, bl4, bl3);
            }  
            free_bucket(bucket_l4);   //printf("======== End  of bucket level bl5:%d->bl4:%d =======\n", bl5, bl4);
        }  
        free_bucket(bucket_l5);
        #ifdef DEBUG
        printf("======== End  of bucket level bl5:%d =======\n", bl5);
        #endif
    }  
    free_bucket(bucket_l6);
    #ifdef DEBUG
    puts("======== End  of bucket level bl6 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);  check_mem_alloc(s_list);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        // Loop over the fifo in reverse order
        for(int64_t i = ios-1; i >= 0; --i)  s_list[i] = u_list[sequence_of_indices[i]];
    } else{
        if(sort_order != 'a')  printf("Wrong sort order input. Default ascending order use.\n");
        // Loop over the fifo forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = 7
uint64_t* radix_sort_h7d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order ) 
{
    // Declare an array for merging indices and index of that array 
    // (ios=index of 'sequence_of_indices')
    uint32_t *sequence_of_indices = malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(sequence_of_indices);
    static uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl7 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit (= 7)
    spfifo_t *bucket_l7 = tp_radix_pos(u_list, l_size, 7);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl6 = 0; bl6 < 16; ++bl6) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl6:%d --------\n", bl6);
        #endif
        if( bucket_l7[bl6].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l6 = radix_pos(u_list, bucket_l7[bl6].fdata, bucket_l7[bl6].wp, 6);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl5 = 0; bl5 < 16; ++bl5) {
            #ifdef DEBUG 
            printf("----------- Start  of bucket level bl5:%d --------\n", bl5);
            #endif
            if( bucket_l6[bl5].wp == 0 ) continue; // Skip the loop if the bucket is empty
            spfifo_t *bucket_l5 = radix_pos(u_list, bucket_l6[bl5].fdata, bucket_l6[bl5].wp, 5);

            #ifdef DEBUG
            getchar();
            #endif

            for(uint8_t bl4 = 0; bl4 < 16; ++bl4) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d --------\n", bl5, bl4);
                if( bucket_l5[bl4].wp == 0 ) continue;
                spfifo_t *bucket_l4 = radix_pos(u_list, bucket_l5[bl4].fdata, bucket_l5[bl4].wp, 4);

                #ifdef DEBUG_L2
                    getchar();
                #endif

                for(uint8_t bl3 = 0; bl3 < 16; ++bl3) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d --------\n", bl5, bl4, bl3);
                    if( bucket_l4[bl3].wp == 0 ) continue;
                    spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

                    for(uint8_t bl2 = 0; bl2 < 16; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
                        if( bucket_l3[bl2].wp == 0 ) continue;
                        spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

                        for(uint8_t bl1 = 0; bl1 < 16; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
                            if( bucket_l2[bl1].wp == 0 ) continue;
                            spfifo_t *bucket_l1 = radix_pos(u_list, bucket_l2[bl1].fdata, bucket_l2[bl1].wp, 1);

                            // Merge the indices into a fifo sequentially
                            for(uint8_t m = 0; m < 16; ++m) {
                                for(uint8_t n = 0; n < bucket_l1[m].wp; ++n) {
                                    // Take the index from the bucket and keep into the fifo
                                    sequence_of_indices[ios] = bucket_l1[m].fdata[n];  ios += 1;
                                    // printf("Number of merged indices(ios): %u\n", ios);
                                }
                            }
                            free_bucket(bucket_l1);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
                        }
                        free_bucket(bucket_l2);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
                    }
                    free_bucket(bucket_l3);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d =======\n", bl5, bl4, bl3);
                }
                free_bucket(bucket_l4);   //printf("======== End  of bucket level bl5:%d->bl4:%d =======\n", bl5, bl4);
            }
            free_bucket(bucket_l5);
            #ifdef DEBUG
            printf("======== End  of bucket level bl5:%d =======\n", bl5);
            #endif
        }
        free_bucket(bucket_l6);
        #ifdef DEBUG
        puts("======== End  of bucket level bl6 =======");
        #endif
    }
    free_bucket(bucket_l7);
    #ifdef DEBUG
    puts("======== End  of bucket level bl7 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);  check_mem_alloc(s_list);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        // Loop over the fifo in reverse order
        for(int64_t i = ios-1; i >= 0; --i)  s_list[i] = u_list[sequence_of_indices[i]];
    } else{
        if(sort_order != 'a')  printf("Wrong sort order input. Default ascending order use.\n");
        // Loop over the fifo forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = 8
uint64_t* radix_sort_h8d( const uint64_t u_list [], uint32_t l_size, 
        char sort_order )
{
    // Declare an array for merging indices and index of that array 
    // (ios=index of 'sequence_of_indices')
    uint32_t *sequence_of_indices = malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(sequence_of_indices);
    static uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl8 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit (= 8)
    spfifo_t *bucket_l8 = tp_radix_pos(u_list, l_size, 8);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl7 = 0; bl7 < 16; ++bl7) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl7:%d --------\n", bl6);
        #endif
        if( bucket_l8[bl7].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l7 = radix_pos(u_list, bucket_l8[bl7].fdata, bucket_l8[bl7].wp, 7);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl6 = 0; bl6 < 16; ++bl6) {
            #ifdef DEBUG 
            printf("----------- Start  of bucket level bl6:%d --------\n", bl6);
            #endif
            if( bucket_l7[bl6].wp == 0 ) continue; // Skip the loop if the bucket is empty
            spfifo_t *bucket_l6 = radix_pos(u_list, bucket_l7[bl6].fdata, bucket_l7[bl6].wp, 6);

            #ifdef DEBUG
            getchar();
            #endif

            for(uint8_t bl5 = 0; bl5 < 16; ++bl5) {
                #ifdef DEBUG 
                printf("----------- Start  of bucket level bl5:%d --------\n", bl5);
                #endif
                if( bucket_l6[bl5].wp == 0 ) continue; // Skip the loop if the bucket is empty
                spfifo_t *bucket_l5 = radix_pos(u_list, bucket_l6[bl5].fdata, bucket_l6[bl5].wp, 5);

                #ifdef DEBUG
                getchar();
                #endif

                for(uint8_t bl4 = 0; bl4 < 16; ++bl4) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d --------\n", bl5, bl4);
                    if( bucket_l5[bl4].wp == 0 ) continue;
                    spfifo_t *bucket_l4 = radix_pos(u_list, bucket_l5[bl4].fdata, bucket_l5[bl4].wp, 4);

                    #ifdef DEBUG_L2
                        getchar();
                    #endif

                    for(uint8_t bl3 = 0; bl3 < 16; ++bl3) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d --------\n", bl5, bl4, bl3);
                        if( bucket_l4[bl3].wp == 0 ) continue;
                        spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

                        for(uint8_t bl2 = 0; bl2 < 16; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
                            if( bucket_l3[bl2].wp == 0 ) continue;
                            spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

                            for(uint8_t bl1 = 0; bl1 < 16; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
                                if( bucket_l2[bl1].wp == 0 ) continue;
                                spfifo_t *bucket_l1 = radix_pos(u_list, bucket_l2[bl1].fdata, bucket_l2[bl1].wp, 1);

                                // Merge the indices into a fifo sequentially
                                for(uint8_t m = 0; m < 16; ++m) {
                                    for(uint8_t n = 0; n < bucket_l1[m].wp; ++n) {
                                        // Take the index from the bucket and keep into the fifo
                                        sequence_of_indices[ios] = bucket_l1[m].fdata[n];  ios += 1;
                                        // printf("Number of merged indices(ios): %u\n", ios);
                                    }
                                }
                                free_bucket(bucket_l1);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
                            }
                            free_bucket(bucket_l2);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
                        }
                        free_bucket(bucket_l3);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d =======\n", bl5, bl4, bl3);
                    }
                    free_bucket(bucket_l4);   //printf("======== End  of bucket level bl5:%d->bl4:%d =======\n", bl5, bl4);
                }
                free_bucket(bucket_l5);
                #ifdef DEBUG
                printf("======== End  of bucket level bl5:%d =======\n", bl5);
                #endif
            }
            free_bucket(bucket_l6);
            #ifdef DEBUG
            puts("======== End  of bucket level bl6 =======");
            #endif
        }
        free_bucket(bucket_l7);
        #ifdef DEBUG
        puts("======== End  of bucket level bl7 =======");
        #endif
    }
    free_bucket(bucket_l8);
    #ifdef DEBUG
    puts("======== End  of bucket level bl8 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);  check_mem_alloc(s_list);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        // Loop over the fifo in reverse order
        for(int64_t i = ios-1; i >= 0; --i)  s_list[i] = u_list[sequence_of_indices[i]];
    } else{
        if(sort_order != 'a')  printf("Wrong sort order input. Default ascending order use.\n");
        // Loop over the fifo forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //


// ======================================================================== //
// Sort unsorted list of integer number using radix sort algorithm
// Maximum digit get from function's argument
uint64_t* radix_sort_16b( const uint64_t u_list [], uint32_t l_size, 
        uint8_t max_digit, char sort_order )
{
    // TODO:: Will test soon
    switch(max_digit)
    {
        case 1:
        case 2:
        case 3:
        case 4:    return radix_sort_h4d(u_list, l_size, sort_order);   break;
        case 5:    return radix_sort_h5d(u_list, l_size, sort_order);   break;
        case 6:    return radix_sort_h6d(u_list, l_size, sort_order);   break;
        case 7:    return radix_sort_h7d(u_list, l_size, sort_order);   break;
        case 8:    return radix_sort_h8d(u_list, l_size, sort_order);   break;
        default:
            printf("Unsupported digit range. Currently support <= 8 digit");
            return NULL;
    }
}
// ------------------------------------------------------------------------ //


// ======================================================================== //
// Print some head part and some tail part of an array
// l_size is then number of elements (last index+1)
void print_head_tail_list( uint64_t list [], uint32_t l_size) 
{
    #ifdef DEBUG
    printf("Arguments: list[0] = %lu  list_size = %u \n", list[0], l_size);
    #endif
    // Check the array length is greater than 8
    if (l_size < 8) {
        printf("The number of elemets of array should be greater than 8.\n");
    } else {
        // Loop over the array and print first 4 elements
        for( uint8_t i = 0; i < 4; ++i)
            printf("%lu(0x%x)  ", list[i], (uint32_t)list[i] );

        printf("::  ");
        // Loop over the array and print last 4 elements
        for( uint8_t i = 4; i > 0; --i)
            printf("%lu(0x%x)  ", list[l_size-i], (uint32_t)list[l_size-i] );

        puts("");        // Print the last newline character
    }
}


// ------------------------------------------------------------------------ //
// Print array elements upto boundary
void myPrintList( uint64_t list [], uint8_t boundary) 
{
    for(uint32_t i = 0; i < boundary; ++i)  printf("%I64lu ", list[i]);
}

// ------------------------------------------------------------------------ //
// Print all data of the FIFO
void whole_FIFO_print(spfifo_t fifo) 
{
    // Check if the FIFO is not empty
    if(fifo.wp <= 0) {
        puts("FIFO is empty\n");
        return ;
    }
    // void* data = fifo->fdata;     // dereference the array from pointer of pointer
    // Loop over the array of the FIFO and point each element
    for(uint32_t i = 0; i < fifo.wp; ++i) {
        printf("%u ", *(uint32_t*)(fifo.fdata + i) );
    }
}
