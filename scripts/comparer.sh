for umbral1 in $(seq 4 0.1 6); do
    printf "%.1f\t" $umbral1
    scripts/run_vad.sh $umbral1 | fgrep TOTAL
done | sort -t: -k2n