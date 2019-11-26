#!/bin/bash
read N
readarray -t lines <<< $( cat | tr -s $'\t')

declare -a rates;
declare -a countries

INF=1000000

while IFS=$'\t' read -r from to rate; do
    countries+=( ${from} ${to} )
    forward=$(bc -l <<< "-l($rate)")
    backward=$(bc -l <<< "l($rate)")
    rates+=( "$from|$to|$forward" )
    rates+=( "$to|$from|$backward" )
done < <(echo "${lines[@]}" | tr ' ' $'\n')

countries=( $(echo "${countries[@]}" | tr ' ' $'\n' | sort -u) )

printkeyvalues() {
	  declare -n arr=$1
	  for key in "${!arr[@]}"; do
		    echo $1[${key}]=${arr[${key}]}
    done
}

printarray() {
	  declare -n arr=$1
	  for value in "${arr[@]}"; do
		    echo "${value}"
	  done
}

bellmanFord() {
    local -A distances;
    local -A next;
    for country in "${countries[@]}" ; do
        distances[${country}]=$INF
        next[${country}]=${country}
    done

    start=$1
    distances[${start}]=0

    echo " - running Bellman-Ford"
    for _ in $(seq ${N}); do
        while IFS='|' read -r from to rate; do
            fd=$(echo ${distances[${from}]} | tr -d $'\n')
            td=$(echo ${distances[${to}]} | tr -d $'\n')
            d=$(bc -l <<< "$fd + $rate" | tr -d $'\n')
            if [[ $fd != $INF ]] && [[ $(bc -l <<< "${td} > ${d}") -ne 0 ]] ; then
                distances[${to}]=${d}
                next[${from}]=${to}
            fi
        done < <(echo "${rates[@]}" | tr ' ' $'\n')
    done

    echo " - showing results"
    for curr in "${countries[@]}" ; do
        if [[ ${curr} == ${start} ]] ; then
            continue
        fi

        echo "    - through ${curr}"
        start_rate=$(printarray rates | grep -e "^${start}|${curr}|" | cut -d '|' -f3 | tr -d $'\n')
        amount=$(bc -l <<< "e(-1*${start_rate}) * 1000" | tr -d $'\n')
        (
            echo "1000 ${start}"
            echo "--> $(bc <<< "scale=2; ${amount}/1") ${curr}"
            for _ in $(seq ${N}) ; do
                nxt=${next[${curr}]}
                rate=$(printarray rates | grep -e "^${curr}|${nxt}|" | cut  -d '|' -f3 | tr -d $'\n')
                rate=$(bc -l <<< "e(-1*${rate})" | tr -d $'\n')
                amount=$(bc -l <<< "$amount * $rate" | tr -d $'\n')
                echo "--> $(bc <<< "scale=2; ${amount}/1") ${nxt}"
                curr=$nxt
                if [[ ${curr} == ${start} ]] ; then
                    break
                fi
            done
        ) | tr $'\n' ' ' | grep -P '\d{4,}\.\d+ '${start}' $' # only return values ending >1000
    done
}

for country in "${countries[@]}" ; do
    echo "Starting in ${country}"
    bellmanFord $country
done
