for count_ms in $(seq 6 1 12); do
    for count_mv in $(seq 8 1 16); do
        printf "%f\t%f\t%f\t%f\t" $alfa_1 $alfa_2 $count_ms $count_mv
        scripts/run_vad.sh $alfa_1 $alfa_2 $count_ms $count_mv | fgrep TOTAL; 
done; done | sort -t: -k2n


