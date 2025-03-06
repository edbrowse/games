/*********************************************************************
seedpattern.c: count the distinct seed patterns for calculation solitaire.
given four distinct cards, you can always divide by the first mod 13
to find a new pattern that starts with an ace.
Do this four times, and arrange them in order,
and keep the smallest.
Run the output through sort -u, no need for me to reinvent that wheel.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

static int modinv[] = {0,1,7,9,10,8,11,2,5,3,4,6,12};

static int copy[4]; /* copy of the pattern */
static int best[4];

static void normalize(int j)
{
int m = modinv[copy[j]];
int i, change, swap;

for(i=0; i<4; ++i)
copy[i] = copy[i] * m % 13;

/* bubble sort */
change = 1;
while(change) {
change = 0;
for(i=0; i<3; ++i) {
if(copy[i] < copy[i+1]) continue;
swap = copy[i], copy[i] = copy[i+1], copy[i+1] = swap;
change = 1;
}
}

for(i=0; i<4; ++i) {
int d = copy[i] - best[i];
if(d > 0) return;
if(d < 0) break;
}

memcpy(best, copy, sizeof(copy));
}

int main()
{
int seed[4];
int j;
static const char letter[] = " 123456789ejq";

seed[0] = 1;
for(seed[1]=2; seed[1]<11; ++seed[1]) {
for(seed[2]=seed[1]+1; seed[2]<12; ++seed[2]) {
for(seed[3]=seed[2]+1; seed[3]<13; ++seed[3]) {
memcpy(best, seed, sizeof(seed));
for(j=1; j<4; ++j) {
memcpy(copy, seed, sizeof(seed));
normalize(j);
}
for(j=0; j<4; ++j)
printf("%c", letter[best[j]]);
printf("\n");
}
}
}

return 0;
}

