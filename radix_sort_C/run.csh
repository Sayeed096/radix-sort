#!/usr/bin/csh -f

set prog = 'rsort'
# Compile the C code
gcc -w ./radixsort16base.c -o ${prog}
#gcc -g -lm -Wall ./radixsort16base.c -o ${prog}

# Run the program
./${prog}

#sleep 1      # Just for give a moment to stop the program
# Clear the directory
#rm -rf ${prog}

