#!/bin/bash
tests_failed=false
echo Running Tests...

for f in tests/*.err ; do
	rm -f $f
done

for f in tests/*.in ; do
	bin/luminous "$f" > file.tmp
	if diff "${f%.in}.out" file.tmp > /dev/null ; then
		echo Test $(basename $f) passed.
	else
		tests_failed=true
		cp file.tmp "${f%.in}.err"
		echo Test $(basename $f) failed.
		diff -c "${f%.in}.out" file.tmp
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