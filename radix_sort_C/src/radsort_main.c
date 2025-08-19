/**
 * @file radsort_main.c
 * @author Md. Sayeed Al Masud (planetmind@outlook.com)
 * @brief Radix sort for integer number
 * @version 0.1
 * @date 2022-02-13
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
 *
 */

// Include header
#include "radsort.h"
#include <time.h>
#include <string.h>

// ======================================================================== //
// Main function, the main body of the program
// ======================================================================== //
int main( int argc, char *argv[])
{
    // Declare local variables
    uint32_t max       = 20;   //1000000;
    uint64_t upper     = 65535; //05000000;
    // uint64_t upper     = (uint64_t) 9999999999999999999; //65535; //05000000;
    uint64_t lower     = 16300;
    uint64_t unsorted_list[max];
    uint64_t *sorted_list;
    char s_order = 'a';

    // Check if the user has provided any argument
    printf("You have entered %d arguments:\n", argc-1);
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    if (argc > 1) {
        if (strcmp(argv[1], "a") == 0) {
            printf("Sorting in ascending order.\n");
        } else if (strcmp(argv[1], "d") == 0) {
            printf("Sorting in descending order.\n");
            s_order = 'd';
        } else {
            printf("No valid sorting order specified. Defaulting to ascending order.\n");
        }
    } else {
        printf("No sorting order argument provided. Defaulting to ascending order.\n");
    }
    printf("selected Sorting order: %c\n", s_order);
    

    // Generate unsorted list of random numbers
    srand(time(0));
    uint64_t range = (upper - lower + 1);// >> 52;
    printf("upper: 0x%llu, lower: 0x%llx, range: %llu\n", 
                  upper,         lower,      range);
    for(uint32_t i = 0; i <= max; ++i) {
        unsorted_list[i] = ((rand() % range) /*<< 40*/) + lower;
    //    printf("[%d]: range:%lu, \t-> %llu(0x%llx)  \n", 
    //              i,       range, unsorted_list[i], unsorted_list[i]);
    }
    
    // Print some first and last value of unsorted list
    print_head_tail_list(unsorted_list, max); puts("");

    // Sorting the number list
    sorted_list = recur_radix_sort_hNd(unsorted_list, max, 4, s_order);
    // sorted_list = async_radix_sort_hNd(unsorted_list, max, 4, s_order);

    // Print sorted list
    print_head_tail_list(sorted_list, max);
    // printf("Sorted list: %llx\n", sorted_list[0]);

    free(sorted_list);
    return 0;
}
