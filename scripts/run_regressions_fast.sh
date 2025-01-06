#!/usr/bin/env bash

source "$(dirname "$0")/utils.sh"

get_log_dir_and_time_cmd_and_nm_exec "regressions" LOG_DIR TIME_CMD NM_EXEC
NM_EXEC=$(find_executable "app")
[ $? -ne 0 ] && exit $?
echo "Using executable: $NM_EXEC"

# 1) Function #1 (quadraticFunction1)
#    Known behavior:
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - By default, it starts from x_i=3 and tries to go down to near zero.
#    - For small sizes we expect to get pretty close to zero if everything is correct.

echo
echo "=== TEST 1: function=1, size=5 (small dimension) ==="
$NM_EXEC -f 1 -s 50 -e 3 -l "$LOG_DIR/run0.log"

echo
echo "=== TEST 2: function=1, size=20 (moderate dimension) ==="
$NM_EXEC -f 1 -s 100 -e 3 -l "$LOG_DIR/run1.log"

echo
echo "=== TEST 3: function=1, size=100 (large dimension) ==="
$NM_EXEC -f 1 -s 200 -e 3 -l "$LOG_DIR/run2.log"

# 2) Function #16 (trigonometricFunction16)
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - For small sizes, we check if it converges quickly.

echo
echo "=== TEST 4: function=16, size=5 (small dimension) ==="
$NM_EXEC -f 16 -s 50 -e 3 -l "$LOG_DIR/run3.log"

echo
echo "=== TEST 5: function=16, size=15 ==="
$NM_EXEC -f 16 -s 100 -e 3 -l "$LOG_DIR/run4.log"

# 3) Function #17 (quarticFunction17)
#    - The global min is f(x)=0 at x=(0,0,...,0).
#    - Start from x=1, so we check how well Nelder–Mead reduces it.

echo
echo "=== TEST 6: function=17, size=5  (small dimension) ==="
$NM_EXEC -f 17 -s 50 -e 3 -l "$LOG_DIR/run5.log"

echo
echo "=== TEST 7: function=17, size=50 (bigger dimension) ==="
$NM_EXEC -f 17 -s 100 -e 3 -l "$LOG_DIR/run6.log"

echo
echo "Logs are saved in: $LOG_DIR"
echo "=== All tests done. ==="
