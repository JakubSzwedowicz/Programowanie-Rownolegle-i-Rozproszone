#!/usr/bin/env bash

NM_EXEC=$(find . -name "App" -type f -executable 2>/dev/null | head -n1)
if [[ -z "$NM_EXEC" ]]; then
    echo "ERROR: Could not find an executable named 'App' in the CURRENT DIRECTORY or SUBDIRECTORIES.
     Are you calling inside /scripts/?"
    exit 1
else
    echo "Using app under path: ${NM_EXEC}"
fi


# 1) Function #1 (quadraticFunction1)
#    Known behavior:
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - By default, it starts from x_i=3 and tries to go down to near zero.
#    - For small sizes we expect to get pretty close to zero if everything is correct.

echo "=== TEST 1: function=1, size=5 (small dimension) ==="
$NM_EXEC -f 1 -s 50 -e 3

echo "=== TEST 2: function=1, size=20 (moderate dimension) ==="
$NM_EXEC -f 1 -s 100 -e 3

echo "=== TEST 3: function=1, size=100 (large dimension) ==="
$NM_EXEC -f 1 -s 200 -e 3

# 2) Function #16 (trigonometricFunction16)
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - For small sizes, we check if it converges quickly.

echo "=== TEST 4: function=16, size=5 (small dimension) ==="
$NM_EXEC -f 16 -s 50 -e 3

echo "=== TEST 5: function=16, size=15 ==="
$NM_EXEC -f 16 -s 100 -e 3

# 3) Function #17 (quarticFunction17)
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - Start from x=1, so we check how well Nelder–Mead reduces it.

echo "=== TEST 6: function=17, size=5  (small dimension) ==="
$NM_EXEC -f 17 -s 50 -e 3

echo "=== TEST 7: function=17, size=50 (bigger dimension) ==="
$NM_EXEC -f 17 -s 100 -e 3

echo "=== All tests done. ==="
