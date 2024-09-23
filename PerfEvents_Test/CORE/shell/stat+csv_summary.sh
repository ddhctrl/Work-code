#!/bin/sh
# perf stat csv summary test
# SPDX-License-Identifier: GPL-2.0

set -e

echo "Starting perf stat csv summary test"

#
#     1.001364330 9224197  cycles 8012885033 100.00
#         summary 9224197  cycles 8012885033 100.00
#
echo "Running perf stat with summary"
perf stat -e cycles -x' ' -I1000 --interval-count 1 --summary 2>&1 | \
grep -e summary | \
while read summary num event run pct
do
    echo "Summary line: $summary $num $event $run $pct"
	if [ $summary != "summary" ]; then
		echo "Error: Expected 'summary', got '$summary'"
		exit 1
	fi
done

echo "First test completed successfully"

#
#     1.001360298 9148534  cycles 8012853854 100.00
#9148534  cycles 8012853854 100.00
#
echo "Running perf stat with summary and no CSV summary"
perf stat -e cycles -x' ' -I1000 --interval-count 1 --summary --no-csv-summary 2>&1 | \
grep -e summary | \
while read num event run pct
do
    echo "Unexpected summary line: $num $event $run $pct"
	exit 1
done

echo "Second test completed successfully"

echo "Perf stat csv summary test completed successfully"
exit 0

