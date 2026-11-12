#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// output of bship f, after about an hour.
// 30093975536 arrangements
// 13659009420 footprints
// 3380 symmetric

/*********************************************************************
An arrangement is the position of all five boats on the board.
The eight rotations and reflections of an arrangement are distinct.
This can be seen from the carrier alone.
Spin the board around until at least 3 of the carrier squares are in the upper
left quadrant, then reflect through the main diagonal if need be,
so the carrrier is horizontal.
That's 8 distinct arrangements.
The program assumes this has already been done, and thus it puts the carrier
in one of 15 positions at the upper left, and computes arrangements from there.
The result is then multiplied by 8.

A footprint is the set of holes consumed by the ships on the board,
regardless of which ships consume which holes. For example,
we can always swap the cruiser and sub without changing the footprint.
Pass an argument of f if you want this program to count footprints.
Each footprint is 17 holes.
This is convenient because it means the four rotations
of any footprint are unique.
More than half the holes are in the upper or lower half of the board,
and more than half the holes are in the left or right half of the board.

Reflection through the main diagonal could give the same footprint.
Put the carrier and battle in row J, columns 2 through 10,
and put the other three ships up column 10.
Flip this through the diagonal and it's the same.
And there are others.

To be its own reflection, an odd number of squares are on the main diagonal.
In the above example, just one.
Each ship puts at most one square on the main diagonal.
It is easy to test whether a footprint is symmetric,
through the main diagonal or the antidiagonal.
A footprint cannot be symmetric through both.
It would have an odd number of squares on the main diagonal,
but to be symmetric through the antidiagonal, it has to have an even number of squares on the main diagonal.

How might boats move about and give the same footprint?
In other words, how might the arrangements overcount the footprints?
We already mentioned the cruiser sub swap;
that overcounts the footprint by a factor of 2.
What else do we have?

If pt is isolated it can't move, because no ship ccan replace it.
If sub, cruiser, or battle is isolated, it can't move (save the sister swap).
If pt is almost isolated, and touches one ship,
on one square of its side, it can't move.
If pt touches precisely two ships, on either side of the same square, it can't move.
If sub touches one ship on its middle, or a ship on either side of its middle,
touching only at its middle square, it can't move (save the sister swap).
If battle touches ships only at its two middle squares, it can't move.

The example of ships on the bottom row and right column is an extreme overcount.
The three small ships on the right can arrange in 6 different ways.
And battle and carrier can swap, so that's 2 times 6.
Swap sub and pt with carrier, now the bottom three ships arrange in 6 ways.
And another 6 if we interchange carrier and cruiser on the right.
Swap cruiser and pt with carrier, now the bottom three ships arrange in 6 ways.
And another 6 if we interchange carrier and sub.
That's 36 so far.
Reflect through the main diagonal, so that battle is in column 10
rather than row J. We have 72 arrangements giving the same footprint.
This is in fact the highest overcount.

How to count footprints from arrangements?
Each arrangement increments its footprint and all 8 rotations of that footprint,
just as it incremented all 8 rotations of the arrangement in the simpler case.
We already showed the rotations of the footprint are distinct.
Suppose we determined, by moving ships about, that k ship arrangements
lead to the same footprint.
In this event, add 8/k.

Start with a footprint f with overcount k.
Move the ships about to find the k possibilities that make f.
Each one pulls back to a unique arrangement, as the carrier moves
back to the upper left an horizontal.
That arrangement has a footprint that is a rotation of f.
It will be computed with the same algorithm,
and the overcount will again be k.
This arrangement (indirectly) adds 1/k to f.
This happens k times over, hence f is fully counted.

We desperately need an example or two.
Put the carrier battle and pt in the upper left along row A and column 1,
so that the footprint is symmetric so far.
Put the sub and cruiser in the upper right.
This gives a footprint f,
and implies 7 other images of f.
We found f by carrier in row A, battle in the upper left, and pt below battle.
This is the first time we see f.
We found an overcount of k, and added 8/k, or 1, to f and all its images.
If 1 is spread out, we added 1/8 to f.
Three more arrangements with carrier in row A leave f in place.
We run into each of these and add 1/8 to f.
Four more arrangements, still preserving f, put carrier in column 1.
We will run into the reflections (through the main diagonal)
of these arrangements later.
These four arrangements have footprint g, where sub is in the lower left.
We add 1/8 to g, 4 times over, but in doing so we also add 1/8 to f, 4 times over.
All that means f is counted once.

Now put the sub in the upper right and cruiser in lower left,
so f is indeed symmetric.
It still has overcount k.
But there are only 4 images of f, the 4 rotations.
Add 8/8 or 1 to the images of f, and we are adding 1/4 to f.
This happens 4 times over as per the arrangements with carrier in row A.
There are 4 more arrangements producing f, with carrier in column 1.
These are nonstandard arrangements.
Flip them over to see them as we will encounter them.
But the thing is, we've already seen them, we've already counted them.
So once again, f (symmetric) is counted precisely once.

Many values of k arise, such as 46, 26, 34, etc.
We need to take the lcm of all these values, if we want to stay within
the integers and avoid floating point arithmetic.
Unfortunately the lcm is 160626866400,
32*27*25*7*11*13*17*19*23,
which is to large even for a long long.
So I'll have to settle for an lcm with most of the factors and hope
the division errors don't catch up with me, after 30 billion arrangements.
*********************************************************************/

const int lcm = 45945900;

// 0 on the board means a hole is unused, nonzero means it is occupied by a ship.
// The dimensions of 16 are important here, and more than enough for 10 by 10.
char b[16][16];
long long cnt = 0;  // count arrangements
long long cnt_foot = 0; // count footprints
long long cnt_foot_sym = 0;
static int top = 10;
char unroll = 1, foot, fix_pt;
char *bk = b[0];

void printBoard()
{
    for(int x = 1; x <= top; ++x) {
        for(int y = 1; y <= top; ++y)
            printf("%d", b[x][y]);
        printf("\n");
    }
}

char diagsym()
{
    for(int x = 1; x < top; ++x)
        for(int y = x + 1; y <= top; ++y)
            if(b[x][y] ^ b[y][x]) goto antidiag;
    return 1;
antidiag:
    for(int x = 1; x < top; ++x)
        for(int y = x + 1; y <= top; ++y)
            if(b[x][top+1-y] ^ b[y][top+1-x]) goto done;
    return 1;
done:
    return 0;
}

int compute_k()
{
    static const char holes[4] = {5, 4, 3, 2};
    char avail[4];
    char expect = 17;
    int lev = -1;
    int n, h, h1, h2, l;
    char *p = bk + 17;
    struct frame { char *p; int n, h1, h2; } stack[8];
    struct frame *sf = stack - 1;
    int k = 0;
    avail[0] = avail[1] = avail[3] = 1, avail[2] = 2;
    if(fix_pt) expect = 15, avail[3] = 0;

    push:
    ++lev, ++sf;
    while(!*p) ++p;
    sf->p = p;
    sf->n = -1;
    h1 = 1;
    if(!p[1]) goto got_h1;
    ++h1;
    if(!p[2]) goto got_h1;
    ++h1;
    if(!p[3]) goto got_h1;
    ++h1;
    if(!p[4]) goto got_h1;
    ++h1;
    if(!p[5]) goto got_h1;
    ++h1;
    if(!p[6]) goto got_h1;
    ++h1;
    if(!p[7]) goto got_h1;
    ++h1;
    got_h1:
    h2 = 1;
    if(!p[16]) goto got_h2;
    ++h2;
    if(!p[32]) goto got_h2;
    ++h2;
    if(!p[48]) goto got_h2;
    ++h2;
    if(!p[64]) goto got_h2;
    ++h2;
    if(!p[80]) goto got_h2;
    ++h2;
    if(!p[96]) goto got_h2;
    ++h2;
    if(!p[112]) goto got_h2;
    ++h2;
    got_h2:
    sf->h1 = h1, sf->h2 = h2;

    next:
    n = ++sf->n;
    if(n == 8) goto pop;
    if(!avail[n/2]) goto next;
    h = holes[n/2];
    if(h > expect) goto next;
    // try horizontal first, then vertical.
    if(!(n&1)) {
        if(h > h1) goto next;
        if(expect == h) { ++k; goto pop; }
    // there's room, but see if it leaves an isolated square
        if(h1 == h + 1 && !(p[h+16]|p[h-16])) goto next;
        if(!avail[3] && h1 == h + 2 && !(p[h+16]|p[h-16]|p[h+17]|p[h-15])) goto next;
    // place this ship here in the footprint
        for(l = 0; l < h; ++l) p[l] = 0;
    } else {
        if(h > h2) goto next;
        if(expect == h) { ++k; goto pop; }
        if(h2 == h + 1 && !(p[16*h+1]|p[16*h-1])) goto next;
        if(!avail[3] && h2 == h + 2 && !(p[16*h+1]|p[16*h-1]|p[16*h+17]|p[16*h+15])) goto next;
        // place this ship here in the footprint
        for(l = 0; l < h; ++l) p[16*l] = 0;
    }
    avail[n/2]--;
    expect -= h;
    goto push;

    pop:
    if(--lev < 0) return 2*k;
    --sf;
    p = sf->p, n = sf->n, h1 = sf->h1, h2 = sf->h2;
    h = holes[n/2];
    if(!(n&1)) {
        for(l = 0; l < h; ++l) p[l] = 1;
    } else {
        for(l = 0; l < h; ++l) p[16*l] = 1;
    }
    avail[n/2]++;
    expect += h;
    goto next;
    }

// place the ships from largest to smallest, from carrier to pt.
// The routines to place them are in reverse order, for convenience of reference.
// 40.5s -O 17s
// foot -O 49m28s
void placePT_standard()
{
    for(int x = 1; x <= top; ++x)
        for(int y = 1; y <= top; ++y) {
            if(b[x][y]) continue;
            if(y <= top-1 && !b[x][y+1]) {
                ++cnt;
                if(foot) {
                    fix_pt = 0;
// nothing at either end, and the ship is forced to run in that direction,
// any longer ship wouldn't fit.
                    if(!(b[x][y-1]|b[x][y+2]) &&
                    (!(b[x-1][y]|b[x+1][y]) ||
                    !(b[x-1][y+1]|b[x+1][y+1]) ||
                    // ship above but nothing above that.
                    !(b[x+1][y]|b[x+1][y+1]|b[x-2][y]|b[x-2][y+1]) ||
                    // ship below but nothing below that.
                    !(b[x-1][y]|b[x-1][y+1]|b[x+2][y]|b[x+2][y+1])
                    ))
                        fix_pt = 1;
                    else if(!(b[x-1][y]|b[x+1][y]|b[x-1][y+1]|b[x+1][y+1])) {
// at this point there is nothing on either side of the pt.
// See if there is nothing on one end, and the middle of a ship on the other,
// which would leave an isolated hole if the pt was longer.
                        if(!b[x][y+2] && (
                        b[x-1][y-1] && !b[x-2][y-1] && !b[x-1][y-2] ||
                        b[x+1][y-1] && !b[x+2][y-1] && !b[x+1][y-2]))
                            fix_pt = 1;
                        else if(!b[x][y-1] && (
                        b[x-1][y+2] && !b[x-2][y+2] && !b[x-1][y+3] ||
                        b[x+1][y+2] && !b[x+2][y+2] && !b[x+1][y+3]))
                            fix_pt = 1;
                    }
                    if(!fix_pt) bk[16*x+y] = bk[16*x+y+1] = 1;
                    int k = compute_k();
// we have to put pt on the board to determine symmetry
                    b[x][y] = b[x][y+1] = 1;
                    char sym = ((b[y][x]&b[y+1][x] || b[top+1-y][top+1-x]&b[top+1-(y+1)][top+1-x]) && diagsym());
                    int add = 8*lcm/k;
                    cnt_foot += add;
                    if(sym) cnt_foot_sym += add;
                    b[x][y] = b[x][y+1] = 0;
                }
            }
            if(x <= top-1 && !b[x+1][y]) {
                ++cnt;
                if(foot) {
                    fix_pt = 0;
                    if(!(b[x-1][y]|b[x+2][y]) &&
                    (!(b[x][y-1]|b[x][y+1]) ||
                    !(b[x+1][y-1]|b[x+1][y+1]) ||
                    !(b[x][y-1]|b[x+1][y-1]|b[x][y+2]|b[x+1][y+2]) ||
                    !(b[x][y+1]|b[x+1][y+1]|b[x][y-2]|b[x+1][y-2])
                    ))
                        fix_pt = 1;
                    else if(!(b[x][y-1]|b[x][y+1]|b[x+1][y-1]|b[x+1][y+1])) {
                        if(!b[x+2][y] && (
                        b[x-1][y-1] && !b[x-2][y-1] && !b[x-1][y-2] ||
                        b[x-1][y+1] && !b[x-1][y+2] && !b[x-2][y+1]))
                            fix_pt = 1;
                        else if(!b[x-1][y] && (
                        b[x+2][y-1] && !b[x+2][y-2] && !b[x+3][y-1] ||
                        b[x+2][y+1] && !b[x+2][y+2] && !b[x+3][y+1]))
                            fix_pt = 1;
                    }
                    if(!fix_pt) bk[16*x+y] = bk[16*x+16+y] = 1;
                    int k = compute_k();
                    b[x][y] = b[x+1][y] = 1;
                    char sym = ((b[y][x]&b[y][x+1] || b[top+1-y][top+1-x]&b[top+1-y][top+1-(x+1)]) && diagsym());
                    int add = 8*lcm/k;
                    cnt_foot += add;
                    if(sym) cnt_foot_sym += add;
                    b[x][y] = b[x+1][y] = 0;
                }
            }
        }
}

// unroll the loops. gcc might be able to do this,
// but I can do it better.
// 23.5s -O 7s
void placePT_fast()
{
    const char *p;
    for(int r = 1; r <= 10; ++r) {
        p = b[r];
        cnt +=
          !(p[1]|p[2]) +
          !(p[2]|p[3]) +
          !(p[3]|p[4]) +
          !(p[4]|p[5]) +
          !(p[5]|p[6]) +
          !(p[6]|p[7]) +
          !(p[7]|p[8]) +
          !(p[8]|p[9]) +
          !(p[9]|p[10]);
        p = b[0] + r;
        cnt +=
          !(p[16]|p[32]) +
          !(p[32]|p[48]) +
          !(p[48]|p[64]) +
          !(p[64]|p[80]) +
          !(p[80]|p[96]) +
          !(p[96]|p[112]) +
          !(p[112]|p[128]) +
          !(p[128]|p[144]) +
          !(p[144]|p[160]);
    }
}

void placeSub()
{
    for(int x = 1; x <= top; ++x)
        for(int y = 1; y <= top; ++y) {
            if(b[x][y]) continue;
            if(y <= top-2 && !(b[x][y+1] | b[x][y+2])) {
                b[x][y] = b[x][y+1] = b[x][y+2] = 1;
                if(unroll) placePT_fast(); else placePT_standard();
                b[x][y] = b[x][y+1] = b[x][y+2] = 0;
            }
            if(x <= top-2 && !(b[x+1][y] | b[x+2][y])) {
                b[x][y] = b[x+1][y] = b[x+2][y] = 1;
                if(unroll) placePT_fast(); else placePT_standard();
                b[x][y] = b[x+1][y] = b[x+2][y] = 0;
            }
        }
}

// this is the same code as placeSub, since both ships have three holes
void placeCruiser()
{
    for(int x = 1; x <= top; ++x)
        for(int y = 1; y <= top; ++y) {
            if(b[x][y]) continue;
            if(y <= top-2 && !(b[x][y+1] | b[x][y+2])) {
                b[x][y] = b[x][y+1] = b[x][y+2] = 1;
                placeSub();
                b[x][y] = b[x][y+1] = b[x][y+2] = 0;
            }
            if(x <= top-2 && !(b[x+1][y] | b[x+2][y])) {
                b[x][y] = b[x+1][y] = b[x+2][y] = 1;
                placeSub();
                b[x][y] = b[x+1][y] = b[x+2][y] = 0;
            }
        }
}

void placeBattle()
{
    for(int x = 1; x <= top; ++x)
        for(int y = 1; y <= top; ++y) {
        if(b[x][y]) continue;
        if(y <= top-3 && !(b[x][y+1] | b[x][y+2] | b[x][y+3])) {
            b[x][y] = b[x][y+1] = b[x][y+2] = b[x][y+3] = 1;
            placeCruiser();
            b[x][y] = b[x][y+1] = b[x][y+2] = b[x][y+3] = 0;
        }
        if(x <= top-3 && !(b[x+1][y] | b[x+2][y] | b[x+3][y])) {
            b[x][y] = b[x+1][y] = b[x+2][y] = b[x+3][y] = 1;
            placeCruiser();
            b[x][y] = b[x+1][y] = b[x+2][y] = b[x+3][y] = 0;
        }
        }
}

// symmetry; put carrier horizontal and in the upper left quadrant
void placeCarrier()
{
    for(int x = 1; x <= (top+1)/2; ++x)
        for(int y = 1; y <= (top+1)/2 - 2; ++y) {
        if(foot) printf("carrier %d,%d\n", x, y);
        b[x][y] = b[x][y+1] = b[x][y+2] = b[x][y+3] = b[x][y+4] = 1;
        placeBattle();
        b[x][y] = b[x][y+1] = b[x][y+2] = b[x][y+3] = b[x][y+4] = 0;
        }
}

void testBoard()
{
    char line[32];
    int l = 0;
    if(top != 10) { puts("top must be 10 for the test board option"); exit(1); }
    for(int x = 1; x <= 10; ++x) {
        fgets(line, sizeof(line), stdin);
        for(int y = 1; y <= 10; ++y) {
            if(line[y-1] == '0') continue;
            b[x][y] = 1;
            if(l == 17) { puts("too many holes are marked"); exit(1); }
            ++l;
        }
    }
    if(l < 17) { puts("not enough holes are marked"); exit(1); }
// this doesn't benefit from our fix_pt logic, nor does it test that logic,
// but should still give the right answer.
    fix_pt = 0;
    int k = compute_k();
    printf("k=%d\n", k);
    if(diagsym()) puts("symmetric");
    exit(0);
}

int main(int argc, const char **argv)
{
    const char *args = argc > 1 ? argv[1] : "";
    if(strchr(args, 'b')) testBoard();
    if(strchr(args, 'f')) unroll = 0, foot = 1;
    if(strstr(args, "t6")) unroll = 0, top = 6;
    if(strstr(args, "t8")) unroll = 0, top = 8;

    placeCarrier();

    printf("%lld arrangements\n", cnt * 8);
    if(foot) {
        printf("%lld footprints error %lld\n", cnt_foot/lcm, cnt_foot%lcm);
        printf("%lld symmetric error %lld\n", cnt_foot_sym/lcm, cnt_foot_sym%lcm);
    }
}

/*********************************************************************
One might reasonably ask how many arrangements, or footprints,
are there when grouped by rotations and reflections.
A clump of boats in the upper left would be considered the same as
that clump of boats rotated over into the upper right,
or reflected down into the lower left.
One might ask that question.

Arrangements is trivial, for I already proved that all 8 reflections
and rotations of a given arrangement are distinct.
So take the number and divide by 8.
30093975536 / 8 = 3761746942
Boom!

Footprints I also proved are distinct under rotations, but not always
through diagonal reflection, so there is a bit of work to do here,
but not much. It's a small step.
Call upon the Bernside Polya theorem. Recognize that the group acting on our set
is the dihedral group D4, which has order 8.
Then determine the number of footprints fixed by each group action.
We find the total under group identity, and 0 under the rotations,
and 0 under vertical and horizontal reflections.
All I have to do is count the diagonal symmetric footprints as I go,
which is part of the f option.
Apply the formula and we are there.
(13659009420 + 0 + 0 + 0 + 0 + 0 + 3380) / 8 = 1707376600
*********************************************************************/

/*********************************************************************
The software is complex and subtle, can we validate it to some extent?
Run with ft6, which restricts to a 6 by 6 board.

6687136 arrangements
2575207 footprints error 45945896
152 symmetric error 0

There are 38 symmetric footprints, up to rotations.
Let's try to confirm that by hand. It should be manageable.

Put the carrier entirely above the diagonal, A2 through A6.
This gives an imbalance of 5.
At least one ship touches the diagonal.
A1 is a candidate for that.
Put battle in A1.
If two ships are below in column 1, we can't reflect this picture in the upper right.
Thus pt is below and we are in balance.
The sisters can't touch the diagonal; that doesn't work.
They are off diagonal and on either side, in any of 6 positions. @6

111111
100001
100001
100001
100000
111100

Put pt in A1.
Battle could fill in below, giving the same situation, and the same footprints.
If battle is in Row C then sub is below and we can't complete the picture.
Let sub point down, then something has to fill in F1.
If has to be battle, with cruiser on the right. Seen before.
If sub is across in Row C then Cruiser is below.
Battle has to fill in B3 and this doesn't work.

Put sub in A1.
The only way to finish is pt below sub, battle in row F, and cruiser on the right, which we saw before.

Now A1 has to be empty.
Someone has to be in B1.
If battle runs down column 1, we still need to fill in F1.
Its counterpart is in column 6, and the last ship is on the diagonal, and that can't balance.
Let battle run across in row B.
Somebody is vertical in column 1.
This won't work.

Put sub in B1 pointing down.
If 2 ships fill in below then the last ship can't make up for that on the right.
pt is below, and the tail of battle has to be on the diagonal. @4

011111
100010
100010
100010
111110
100000

Let sub run across in row B.
Someone has to be vertical in column 1. Try battle.
Someone is in C2.
If it is horizontal it touches the diagonal, and the last ship is also on the diagonal,
and that could work with cruiser and pt. @1

011111
111000
111100
101100
100000
100000

Or cruiser could run down column 2 with pt in row B, seen before.

We finished battle in column 1, so something in column 1 is horizontal.
If it is in row F, we still need something in C2, and there are no ships left for the right side.
Put the cruiser in F1 pointing up,
and we can make this work with battle in row C and pt in row D,
but we've seen this before.
Put pt in F1, with battle in row C and cruiser in row D, no good.

Finally put pt in B1.
Let it run across, with battle below. This is in balance.
The sisters fill in. @6

011111
110000
100001
100001
100001
101110

Without battle, something in column 1 is horizontal.
It could be in row F, battle, with cruiser on the right.
Covered in the previous paragraph.
That's the only one that works, so let pt point down.
Somebody has to be vertical, so let it be sub.
This is the same situation we saw earlier.
That finishes carrier off diagonal.
We've got 17 out of 38.

Put the tail of the carrier on the main diagonal, on A1 or B2.
Assume no other ships are on the diagonal.
One side has +4 and we have to make up for that.
If the sisters are opposite, then we are left with battle and pt,
and that won't make a net gain of 4.
One way to balance is battle in column 6, and sub and pt down column 1, and cruiser in row F.
We've seen this pattern before.
Another way has the carrier in row B. @1

001111
011111
110000
110000
110000
110000

With that finished, 2 other ships, besides the carrier,
must touch the diagonal.
If battle is wholly in the upper right, we have 8 squares to make up,
but only 6 squares can be below the diagonal.
If sub is in the upper right, we have 7 to make up, and the other three ships are on or below the diagonal.
The tail of one is on and its reflection is all or part of the sub.
The tail of another is on and its reflection is all or part of the sub.
The sub can't cover both these reflections.
If a ship is wholly above the diagonal it is the pt.

If a ship is in A6, above the diagonal, it has to be the pt.
Or if the carrier is in row B and anyone is at or above it has to be the pt.
That's an imbalance of 6 squares, and we have 10 to play with, and 2 are on the diagonal.
Just one pokes across the diagonal and it is nowhere near the pt.
For proper symmetry, we need another pt in column 1, and we don't have one.
There is then no ship to the right of the carrier or above the carrier.

Put pt in A1 A2.
Something goes down from B1, and some ship is in A3, which we just showed was impossible.

With carrier in row B, put some ship in A1 going down.
This covers B1, and something has to cover A2, which we just ruled out.

Put a ship in A1 running across. It is not the pt.
Another ship is in B1 going down.
Only two ships remain, and they must fill in column 2 and touch the diagonal. @1

111100
111111
111000
110000
010000
010000

If carrier is in row B, there is nothing above it and nothing in column 1.
It now looks like a 5 by 5 grid.

Put battle below the tail of the carrier, which brings the count in balance.
There is one workable arrangement of sub cruiser pt across the diagonal.
It looks like a square with a notch taken out of it.
The notch can be upper left or lower right, and the square can move up or down the diagonal. @10

111110
100000
100000
100011
100111
000111

We can't line up all 4 ships below the tail of the carrier;
something has to be vertical and something horizontal.
Let battle run horizontal just below carrier.
We need 3 more below the first square of battle and 2 more below the second square of battle,
and one on the diagonal.
This can be done but it is the previous pattern.

Put sub vertical below the tail of the carrier.
Someone is below that, plus two more ships on the diagonal.
That can't work, so let sub run across.
That can't work either, so turn to pt.
We still want two more ships on the diagonal so pt vertical doesn't work.
Make it horizontal.
Sub runs below that and leaves battle and cruiser to make a corner on the diagonal. @4

111110
110000
101111
101000
101000
001000

That completes tail of carrier on the diagonal.

Put carrier in row B, whence it starts at B1.
Something is in A2, a ship in the A row.
Let it start in A2.
Let it be at least a sub.
We need to fill C and D 1 and 2, with none or two more ships on the diagonal.
There is one way, if sub and pt are both in row A. @1

011111
111110
110000
110000
110000
100000

Next put pt in A2 A3.
Someone runs across C, to fill in C1 C2, but we still need D2 and E2, and something else on the diagonal.
That won't work, so let the ship in row A start in A1.
If it is battle, we need C and D 1 and 2, and E2 and something else on the diagonal.
Sub in row A doesn't work out any better.
Try pt, which balances the tail of the carrier.
If another ship is in A next to the pt,
we have to fill in columns 1 and 2 plus something on the diagonal, and only two ships remain.
Let battle run down column 2, but it is longer than we need, and causes trouble.
Let sub fill in column 2, creating a balance.
Battle and cruiser then make a corner. @2

110000
111110
010001
010001
010001
001111

That's it for carrier in row B.

The carrier has to be in row C.
Start it at C1.
Somebody fills in A3 B3 D3 E3.
Let two ships run across A and B.
Let one start at A1, whence the other starts at B1.
If one is pt we need another to complete column 3.
That leaves one ship to balance below, which doesn't work.
The two ships run to at least column 3.
This would work if we were 7 by 7.
Battle going down column 2 and pt in row C.
But we're not, so this is a dead end.

Let one ship start at A2 and the other at B1.
If any ship goes past column 3, we need to fill in D3, E3, (D1 or D2), and one more square on the diagonal.
This can't work, so let the ships above be pt and sub.
We still can't complete below.

Let the ships start at A3 and B2.
We have to fill in D1 as well as D3.
This doesn't work.

Let both ships start in column 3.
We need two more ships filling out columns 1 and 2, and something in D3 E3, and that's not workable.

Let pt fill A3 and B3.
The other three ships can't fill and balance below.

Let carrier start at C2. This is the last case.
Let a ship start at A1 across,
and let another ship start at B2 or B3, since something has to cover B3.
Another ship runs down from B1.
If the ship in row B reaches B4, I need the last ship to cover D2 D3 E3 F3, which it can't.
Thus B2 is pt, B1 is sub, A1 is battle, and D3 down is cruiser. @1

111100
111000
111111
101000
001000
001000

Let two ships start at A1 and B1.
The ship in row B can't be pt.
If the ship in row A isn't pt, we need a ship going down from C1.
Cruiser must then cover D3 E3 F3.
This is the same picture as above.
So let pt fill A1.
The B ship is sub or battle, but either way we can't complete the picture below.
That completes a ship in A1 across.

Let a ship run from A1 down.
We need something for A2 and something for B3, two more ships.
That leaves one ship for below, which doesn't work.
so there is nothing in A1.

Put something in A2 across, then something in B1 across, since we must fill B1.
Since we have to fill B3 anyways, B1 cant be pt.
Something fills C1 going down.
We need something for column 3 and for the diagonal, and we are out of ships.
Put something in B1 going down, but we still have to fill B3 across.
One ship left for D2 and column 3.
That finishes a ship in A2.

If A2 goes down it is pt.
Something longer is in B1 down, and that means something is in A3 across.
These have to be battle and sub respectively.
But we have to fill B3 and D3, and it falls apart.
That finishes anything in A2.

If a ship runs at A3 across, I still need something for B3.
If this extends to B4, I need to cover C1 D2 D3 E3 F3, which can't be done with two ships.
The ship in row B is pt, in B2 B3.
The rest can't be balanced.

Put the pt in A3 B3.
Something runs down from C1, and that puts another ship across at A4.
These are battle and sub respectively, and cruiser completes the picture. @1

001111
001000
111111
101000
101000
101000

Something has to be in B3 across.
Say it starts in B3.
We must cover D2 D3 E3 F3.
We can but there is a ship left over.
Start the ship in B2, which means another ship has to touch diagonal.
Let B2 be pt, leaving battle sub cruiser.
This can work but gives a picture we've seen before.
B2 starting sub or battle doesn't work.
That completes Carrier starting at C2.

The score is 38 out of 38.
*********************************************************************/

/*********************************************************************
t6 uses a 6 by 6 board, t8 uses an 8 by 8 board.
All even numbers, as those are central to my assumptions.
However, the program could be tweaked for an odd board.
I proved all rotations and reflections of an arrangement are distinct,
using the four quadrants, and the odd number of holes in the carrier;
but it's easy to extend this to odd boards.
Spin the board around so that the carrier is horizontal and more than half is in the upper left quadrant,
where each quadrant includes the midlines.
If the carrier straddles the midline, find the battleship,
and reflect the board if necessary so the battleship is on the left.
If the battleship is on the midline, apply the same
criterion to the cruiser, the sub, and finally the pt.
They can't all be on the midline, and thence we have established
an arrangement in standard form.
This would take a bit of programming inside the various placeBoat() routines,
but not rocket science.
After that, I think it likely that the program would run as is.

Footprints can have more symmetry on an off board.
Even a 5 by 5 board.
It's possible to arrange the boats like this.

10001
10001
11011
11011
11111

You may want to count these symmetries, the way I count
diagonal symmetries today.
*********************************************************************/
