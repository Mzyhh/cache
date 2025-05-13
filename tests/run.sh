#!/bin/bash
success=0
filures=0
for i in ./*.dat
do
    pred=$(../a.out < $i)
    bn=$(echo $(basename $i) | awk -F'.' '{print $1}')
    true=$(cat lru$bn.ans)
    if [ $pred -ne $true ]
    then
        echo "Error in test $bn: $pred != $true"
        failures=$(( $failures + 1))
    else
        success=$(( $success + 1 ))
    fi
done

if [[ $failures -eq 0 ]]
then
    echo "All test passed."
else
    echo "Failure in $failures tests."
fi

echo "Total number of tests: $(( $failures + $success ))"
