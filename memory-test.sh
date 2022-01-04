#!/bin/bash
tests_failed=false
echo Running Valgrind Tests...

for f in tests/*.errval ; do
	rm -f $f
done

for f in tests/*.in ; do
	valgrind --log-file="file.tmp" bin/luminous "$f" > /dev/null 
	if grep -Rq "All heap blocks were freed -- no leaks are possible" file.tmp ; then
		echo Test $(basename $f) passed.
	else
		tests_failed=true
		cp file.tmp "${f%.in}.errval"
		echo Test $(basename $f) failed.
		cat "${f%.in}.errval"
		echo ============================
	fi
done

rm file.tmp

if "$tests_failed" = true ; then
	echo Tests Failed
	exit 1
else
	echo Tests Done
fi