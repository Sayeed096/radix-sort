/**
 * @file radsort.c
 * @author Md. Sayeed Al Masud (planetmind@outlook.com)
 * @brief Radix Sort Algorithm
 * @version 0.2
 * @date 2025-08-19
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

// ======================================================================== //
#include "radsort.h"

// ======================================================================== //
// Description of all functions
// ======================================================================== //
// ------------------------------------------------------------------------ //
// Make array of buckets with positions/indices of number list with top level
// ------------------------------------------------------------------------ //
static spfifo_t* radix_pos( const uint64_t num_list [], 
        const uint32_t *pos_list, uint32_t nl_sz, const uint8_t digit_h )
{
    // Declare fifo struct type array with size of 16 for 16 buckets
    // Use calloc() to avoid garbage value
    spfifo_t *indices_list = calloc( NUMBER_OF_BUCKETS, sizeof(spfifo_t));
    // Call C-macro function to check the success of dynamic memory allocation
    check_mem_alloc(indices_list);

    // Allocate memory for fifo array(.fdata) of each bucket
    for(uint8_t b = 0; b < NUMBER_OF_BUCKETS; ++b) {
        indices_list[b].fdata = (uint32_t *)malloc( sizeof(uint32_t) * nl_sz);
        check_mem_alloc_partial(indices_list, b);
    }

    // Calculate right shift amount of a number to get a specific hex digit
    uint8_t shift_base = (digit_h - 1) << 2;  // (digit_h-1)*4 bits
    
    // Get each item of current bucket and distribute indices into new buckets
    for( uint32_t i = 0; i < nl_sz; ++i) {
        // If top list then take all position one by one (i) otherwise,
        // take a position from pos_list (index_from_pos_list=ifpl)
        uint32_t ifpl = (pos_list == NULL) ? i : pos_list[i];
        // Determine the bucket number by right shift the number by 
        // (digit-1)*4 bits, then get first digit only
        uint8_t bucket_num = (num_list[ifpl] >> shift_base) & 0x0F;

        // Keep the index into the array of a new bucket
        indices_list[bucket_num].fdata[indices_list[bucket_num].wp] = ifpl;
        indices_list[bucket_num].wp += 1;

        #ifdef DEBUG_L2
        // For show the current loop data and status changes
        printf("[%d]:\tDigit:%d, Cur Pos:%u,\tValue:%8lu(0x%x),\t",
            i, digit_h, ifpl, num_list[ifpl], num_list[ifpl]);
        printf("Bucket No.: %x, FIFO pt:%4d, Indices inCurBucket: ",
            bucket_num, indices_list[bucket_num].wp);
        for (uint8_t t = 0; t < indices_list[bucket_num].wp; ++t)
            printf("%2u ", indices_list[bucket_num].fdata[t]);
        puts("");  // A new line for each loop
        #endif
    }
    // De-allocate some buckets' memory when wp=0
    for (uint8_t b = 0; b < NUMBER_OF_BUCKETS; b++) {
        if (indices_list[b].wp == 0) {
            free(indices_list[b].fdata);
            indices_list[b].fdata = NULL; //printf("Dealloc bucket %d\n", b);
        }
    }
    // TODO: Check if possible to de-allocate some memory of fdata by wp
    return indices_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
static void merge_indices( const spfifo_t *lastL_buckets, spfifo_t *soi_f )
{
    // Merge the indices into a fifo sequentially from last level bucket
    for (uint8_t m = 0; m < NUMBER_OF_BUCKETS; ++m) {
        for (uint8_t n = 0; n < lastL_buckets[m].wp; ++n) {
            // Take an index from the bucket and keep it into the 
            // sequence of indices fifo (soi_f)
            soi_f->fdata[(soi_f->wp)++] = lastL_buckets[m].fdata[n];
            #ifdef DEBUG_L2
            printf("\t->Number of merged indices(ios): %u\n", soi_f->wp);
            #endif
        }
    }
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
static void recur_bucket_merge(const uint64_t u_list[], 
        const spfifo_t *cur_buckets, const uint8_t cur_digit_h, 
        spfifo_t *soi_f )
{
    if (cur_digit_h == 0 || cur_digit_h > 15) return;
    for (uint8_t bl = 0; bl < NUMBER_OF_BUCKETS; ++bl) {
        if (cur_buckets[bl].wp == 0) continue; // skip it for empty bucket
        if (cur_buckets[bl].wp == 1) {
            // For single item bucket, just keep the index, skip the rest
            soi_f->fdata[(soi_f->wp)++] = cur_buckets[bl].fdata[0];
            #ifdef DEBUG_L2
            printf("  Number of merged indices(ios): %u\n", soi_f->wp);
            #endif
            continue;  // Skip to next bucket
        }
        spfifo_t *newL_buckets = radix_pos(
            u_list, cur_buckets[bl].fdata, cur_buckets[bl].wp, cur_digit_h
        );
        if (cur_digit_h <= 1) {
            merge_indices(newL_buckets, soi_f);
        } else {
            recur_bucket_merge(
                u_list, newL_buckets, (cur_digit_h-1), soi_f
            );
        }        
        free_buckets(newL_buckets, NUMBER_OF_BUCKETS);
    }
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
    uint32_t ios = 0;
    
    #ifdef DEBUG
    puts("------------------ Start of bucket level: bl4 ------------------");
    getchar();
    #endif

    // Make top level bucket by the value of number of digit // For now, 4
    spfifo_t *bucket_l4 = radix_pos(u_list, NULL, l_size, 4);

    #ifdef DEBUG
    puts("End of top level buckets making.");    getchar();
    #endif
    
    // Start the main loop which check and sort according to radix position
    for(uint8_t bl3 = 0; bl3 < NUMBER_OF_BUCKETS; ++bl3) {
        #ifdef DEBUG 
        printf("----------- Start  of bucket level bl3:%d --------\n", bl3);
        #endif
        if( bucket_l4[bl3].wp == 0 ) continue; // Skip the loop if the bucket is empty
        spfifo_t *bucket_l3 = radix_pos(u_list, bucket_l4[bl3].fdata, bucket_l4[bl3].wp, 3);

        #ifdef DEBUG
        getchar();
        #endif

        for(uint8_t bl2 = 0; bl2 < NUMBER_OF_BUCKETS; ++bl2) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d --------\n", bl5, bl4, bl3, bl2);
            if( bucket_l3[bl2].wp == 0 ) continue;
            spfifo_t *bucket_l2 = radix_pos(u_list, bucket_l3[bl2].fdata, bucket_l3[bl2].wp, 2);

            for(uint8_t bl1 = 0; bl1 < NUMBER_OF_BUCKETS; ++bl1) {    //printf("----------- Start  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d --------\n", bl5, bl4, bl3, bl2, bl1);
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
                free_buckets(bucket_l1, NUMBER_OF_BUCKETS);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d->bl1:%d =======\n", bl5, bl4, bl3, bl2, bl1);
            }
            free_buckets(bucket_l2, NUMBER_OF_BUCKETS);   //printf("======== End  of bucket level bl5:%d->bl4:%d->bl3:%d->bl2:%d =======\n", bl5, bl4, bl3, bl2);
        }
        free_buckets(bucket_l3, NUMBER_OF_BUCKETS);
        #ifdef DEBUG
        printf("======== End  of bucket level bl3:%d =======\n", bl3);
        #endif
    }  // --------------------------------------- Nested for loop ends here
    free_buckets(bucket_l4, NUMBER_OF_BUCKETS);
    #ifdef DEBUG
    puts("======== End  of bucket level bl4 =======");
    #endif

    // Dynamically allocate memory for sorted list which must be same size and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);
    check_mem_alloc2(s_list, sequence_of_indices);
    
    // Copy the unsorted array into sorted_array according to the sequence of 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        ios -= 1;
        // Iterate through the fifo in reverse order
        for(int32_t i = ios; i >= 0; --i)  s_list[(ios-i)] = u_list[sequence_of_indices[i]];
    } else {
        if(sort_order != 'a')  printf("Wrong sort order input '%c'. Default ascending order used.\n", sort_order);
        // Iterate through the fifo in forward order
        for(uint32_t i = 0; i < ios; ++i)  s_list[i] = u_list[sequence_of_indices[i]];
    }
    free(sequence_of_indices);  sequence_of_indices = NULL;  //puts("");
    return s_list;
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = N (<=16)
uint64_t* recur_radix_sort_hNd( const uint64_t u_list [], uint32_t l_size, 
        uint8_t digit_h_N, char sort_order )
{
    if(digit_h_N == 0 || digit_h_N > 16) {
        puts("Maximum hexadcimal digit can be 16. Zero digit is invalid.");
        printf("Current digit is %d.\n", digit_h_N);
        return NULL;
    }
    // Declare a fifo for merging indices into a sequence of indices(soi)
    spfifo_t soi_fifo;
    soi_fifo.fdata = (uint32_t *)malloc( sizeof(uint32_t) * l_size);
    check_mem_alloc(soi_fifo.fdata);
    soi_fifo.wp = 0;

    #ifdef DEBUG
    puts("------------------ Start of bucket level: blN ------------------");
    //getchar();
    #endif
    // Make top level buckets by the value of number of digit
    spfifo_t *bucket_lN = radix_pos(u_list, NULL, l_size, digit_h_N);
    #ifdef DEBUG
    puts("End of top level buckets making.");
    //getchar();
    #endif
    // Start the main loop which check and sort according to radix position
    recur_bucket_merge( u_list, bucket_lN, (digit_h_N-1), &soi_fifo );
    free_buckets(bucket_lN, NUMBER_OF_BUCKETS);  // Free the top level bucket
    #ifdef DEBUG
    puts("======== End  of bucket level blN =======");
    #endif
    
    // Dynamically allocate memory for sorted list which must be same size and
    // length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);
    check_mem_alloc2(s_list, soi_fifo.fdata);
    
    // Copy the unsorted array into sorted_array according to the sequence of
    // 'sequence_of_indice' fifo
    if(sort_order == 'd') {
        soi_fifo.wp -= 1;
        // Iterate through the fifo in reverse order
        for(int32_t i = soi_fifo.wp; i >= 0; --i)
            s_list[(soi_fifo.wp-i)] = u_list[soi_fifo.fdata[i]];
    } else {
        if(sort_order != 'a')
            printf("Wrong sort order input '%c'. Default ascending order used.\n",
                sort_order);
        // Iterate through the fifo in forward order
        for(uint32_t i = 0; i < soi_fifo.wp; ++i)
            s_list[i] = u_list[soi_fifo.fdata[i]];
    }
    return s_list;
}
// ------------------------------------------------------------------------ //


// ======================================================================== //
// ------------------------------------------------------------------------ //
#ifdef ASYNC_SORT_ENABLED
// ------------------------------------------------------------------------ //
static void async_recur_bucket_merge( thread_args_t *const targs )
{
    /** This function is needed for async call, due to the issue arise when a 
     * function with multiple parameters called by pthread_create(). A struct
     * is used to pass multiple argument to the single parameter function.
     */
    if (targs->digit_h_N == 0 ||
        targs->digit_h_N > 15 || 
        targs->bucket->wp == 0
    )  return;
    if (targs->bucket->wp == 1) {
        // For single item bucket, just keep the index, skip the rest
        targs->soi_f->fdata[(targs->soi_f->wp)++] = targs->bucket->fdata[0];
        #ifdef DEBUG_L2
        printf(" ~Number of merged indices(ios): %u\n", targs->soi_f->wp);
        #endif
        return;  // Skip the rest of the function
    }
    spfifo_t *newL_buckets = radix_pos(
        targs->u_list, targs->bucket->fdata, targs->bucket->wp, 
        targs->digit_h_N
    );
    recur_bucket_merge(
        targs->u_list, newL_buckets, (targs->digit_h_N-1), targs->soi_f
    );
    free_buckets(newL_buckets, NUMBER_OF_BUCKETS);
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
static void async_merge_sList( const uint64_t u_list [], 
    const spfifo_t *const sf, char s_order, uint64_t *new_list )
{
    uint32_t s_cnt = 0;  // sorting counter for copy data sequentially
    // Copy the unsorted array into sorted_array according to the sequence of 
    // 'sequence_of_indice' fifo's index list
    if ( s_order == 'd' ) {
        for(int8_t bn = NUMBER_OF_BUCKETS-1; bn >= 0; --bn) {
            if( sf[bn].fdata == NULL || sf[bn].wp == 0 )  continue;
            // Iterate through the fifo in reverse order, until first element
            for(uint32_t i = (sf[bn].wp-1); i > 0; --i) {
                new_list[s_cnt++] = u_list[sf[bn].fdata[i]];
            }
            new_list[s_cnt++] = u_list[sf[bn].fdata[0]]; // copy first element
        }
    } else {
        if(s_order != 'a') {
            printf("Wrong sort order input '%c'. ", s_order);
            printf("Default ascending sort order is being used.\n");
        }
        for(int8_t bn = 0; bn < NUMBER_OF_BUCKETS; ++bn) {
            if (sf[bn].fdata == NULL || sf[bn].wp == 0 )  continue;
            // Iterate through the fifo in forward order
            for(uint32_t i = 0; i < sf[bn].wp; ++i) {
                new_list[s_cnt++] = u_list[sf[bn].fdata[i]];
            }
        }
    }
}
// ------------------------------------------------------------------------ //

// ------------------------------------------------------------------------ //
// Maximum digit = N (<=16)
uint64_t* async_radix_sort_hNd( const uint64_t u_list [], uint32_t l_size, 
        const uint8_t digit_h_N, char sort_order )
{
    // Check if the digit is in between 0 to 16
    if( digit_h_N == 0 || digit_h_N > 16 ) {
        printf("Maximum hexadcimal digit can be 16. Current digit is %d.\n",
            digit_h_N);
        return NULL;
    }
    // Declare an array of pointers (2D array), for merging indices and index
    // of that array (ios=index of 'sequence_of_indices')
    spfifo_t *soi_fifos = calloc(NUMBER_OF_BUCKETS, sizeof(spfifo_t));
    check_mem_alloc(soi_fifos);
    #ifdef DEBUG
    puts("------------------ Start of bucket level: blN ------------------");
    //getchar();
    #endif
    // Make top level bucket by the value of number of digit
    spfifo_t *bucket_lN = radix_pos(u_list, NULL, l_size, digit_h_N);
    #ifdef DEBUG
    puts("End of top level buckets making.");
    //getchar();
    #endif
    // Initialize with Null or Allocate memory for sequence_of_indices 
    // accoding to the bucket_lN[b].wp. Initialize each ios[b] to 0.
    for(uint8_t b = 0; b < NUMBER_OF_BUCKETS; ++b) {
        if( bucket_lN[b].wp == 0 ) {
            soi_fifos[b].fdata = NULL; // Set to NULL if bucket is empty
        } else {
            soi_fifos[b].fdata = (uint32_t *)malloc(
                sizeof(uint32_t) * bucket_lN[b].wp);
            check_soi_f_fdata(soi_fifos, b, bucket_lN);
        }
        soi_fifos[b].wp = 0; // Initialize ios to 0 for each bucket
    }
    // Start the thread pool and assign the tasks to the threads
    pthread_t threads[NUMBER_OF_BUCKETS];
    thread_args_t th_args_list[NUMBER_OF_BUCKETS];
    for(uint8_t b = 0; b < NUMBER_OF_BUCKETS; ++b) {
        if( bucket_lN[b].wp == 0 ) continue; // Skip the loop for empty bucket
        #ifdef DEBUG
        printf("Creat 'thread' for bucket %2d\n", b);
        #endif
        // Prepare each thread's arguments
        th_args_list[b].u_list = u_list;
        th_args_list[b].bucket = &bucket_lN[b];
        th_args_list[b].digit_h_N = digit_h_N -1;
        th_args_list[b].soi_f = &soi_fifos[b];
        // Start a thread for each bucket with argument
        int rc = pthread_create(
            &threads[b], NULL,
            (void *(*)(void *))async_recur_bucket_merge,
            (void *)&th_args_list[b]
        );
        if(rc) {
            printf("Error creating thread %d: %s\n", b, strerror(rc));
            free_2buckets(soi_fifos, bucket_lN);  // Free memory before return
            return NULL; // Return NULL on error
        }
    }
    // Wait for all threads to finish
    for(uint8_t b = 0; b < NUMBER_OF_BUCKETS; ++b) {
        if( bucket_lN[b].wp == 0 ) continue; // Skip the loop for empty bucket
        int rc = pthread_join(threads[b], NULL);
        if(rc) {
            printf("Error joining thread %d: %s\n", b, strerror(rc));
            free_2buckets(soi_fifos, bucket_lN);  // Free memory before return
            return NULL; // Return NULL on error
        }
    }
    //-
    #ifdef DEBUG_L2
    for(uint8_t b = 0; b < NUMBER_OF_BUCKETS; ++b) {
        printf(" --> bucket[%d]: ios:%u  seq_i:", b, soi_fifos[b].wp);
        if( soi_fifos[b].fdata == NULL ) {puts(""); continue;}
        for(uint32_t i = 0; i < soi_fifos[b].wp; ++i) {
            printf("%u, ",soi_fifos[b].fdata[i]);
        }
        puts("");
    }
    #endif
    free_buckets(bucket_lN, NUMBER_OF_BUCKETS);
    // Dynamically allocate memory for sorted list which must be same size 
    // and length as unsorted list
    uint64_t *s_list = malloc(sizeof(uint64_t) * l_size);
    if (s_list == NULL) {
        printf("Memory allocation failed for sorted list.\n");
        free_buckets(soi_fifos, NUMBER_OF_BUCKETS);  // Free before return
        return NULL; // Return NULL on error
    }
    async_merge_sList(u_list, soi_fifos, sort_order, s_list);
    //-
    free_buckets(soi_fifos, NUMBER_OF_BUCKETS);
    return s_list;
}
#endif // ASYNC_SORT_ENABLED
// ------------------------------------------------------------------------ //


// ======================================================================== //
// Print some head part and some tail part of an array
// l_size is then number of elements (last index+1)
void print_head_tail_list( uint64_t list [], uint32_t l_size) 
{
    #ifdef DEBUG
    printf("Arguments: list[0] = %llu  list_size = %u \n", list[0], l_size);
    #endif
    uint8_t limit = 4;
    // Check the array length is greater than 8
    if (l_size < limit*2) {
        printf("The number of elemets of array should be greater than 8.\n");
    } else {
        puts("= = = = = = = = = = = = = = = = = = = = = = = = = = = = = =");
        // Loop over the array and print first 4 elements
        for( uint8_t i = 0; i < limit; ++i)
            printf("%llu(0x%llx)  ", list[i], list[i] );

        printf("\n::  ");
        // Loop over the array and print last 4 elements
        for( uint8_t i = limit; i > 0; --i)
            printf("%llu(0x%llx)  ", list[l_size-i], list[l_size-i] );

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
    // Iterate through the array of the FIFO and point each element
    for(uint32_t i = 0; i < fifo.wp; ++i) {
        printf("%u ", *(uint32_t*)(fifo.fdata + i) );
    }
}
