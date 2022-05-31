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

// ======================================================================== //
// Main function, the main body of the program
// ======================================================================== //
int main( int argc, char** argv)
{
    // Declare local variables
    uint32_t max       = 100;   //1000000;
    uint32_t upper     = 65535; //05000000;
    uint32_t lower     = 1000;
    uint64_t unsorted_list[max];
    uint64_t *sorted_list;

    // Generate unsorted list of random numbers
    srand(time(0));
    for(uint32_t i = 0; i <= max; ++i) {
        unsorted_list[i] = (rand() % (upper - lower + 1)) + lower;
    }
    
    // Print some first and last value of unsorted list
    print_head_tail_list(unsorted_list, max);

    // Sorting the number list
    sorted_list = radix_sort_16b(unsorted_list, max, 4, 'a');

    // Print sorted list
    print_head_tail_list(sorted_list, max);

    return 0;
}
