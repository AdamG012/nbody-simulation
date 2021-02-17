#!/bin/bash

times_ans=0
perf_ans=0

echo "Enter number of threads:"
read n_threads
if ! [[ "$n_threads" =~ ^[0-9]+$ ]]; then
	echo "Error not a number!"
	exit 1
fi


### CHECK FOR USER INPUT ON TIMING TESTS
while true; do
	read -p "Do you wish to run timing tests? " ans
	case $ans in
		[Yy]* ) let times_ans=1; break;;
		[Nn]* ) break;;
		* ) echo "Please answer y/N.";;
	esac
done

### CHECK FOR USER INPUT ON PERF TESTS
while true; do
	read -p "Do you wish to run perf tests? " ans
	case $ans in
		[Yy]* ) let perf_ans=1; break;;
		[Nn]* ) break;;
		* ) echo "Please answer y/N.";;
	esac
done

valid_ans=0

## CHECK FOR USER INPUT ON VALIDITY TESTS
while true; do
	read -p "Do you wish to run validity tests? " ans
	case $ans in
		[Yy]* ) let valid_ans=1; break;;
		[Nn]* ) break;;
		* ) echo "Please answer y/N.";;
	esac
done

############## PERF AND TIME TEST #############
if [ $times_ans -eq 1 ] || [ $perf_ans -eq 1 ]
then
	printf "\n***************TIMING AND PERF TESTS****************\n"

	### COMPILATION FOR TIMING
	make clean
	make nbody
	make nbody-gui

	for f in test/time/*.in 
	### For every file in the test time block loop through them an
	do
		let len=${#f}-2 		# Get the length of file without type
		fname=${f:10:len}		# Get the name of the file
		if [ $times_ans -eq 1 ]		# If user selected to test times
		then
			while IFS= read -r line
			do
				echo "Test Single Thread: $line"
				time ./nbody $line

				echo "Test Parallel: $line -t"
				time ./nbody $line -t $n_threads
			done < "$f"

		fi

				
		############ RUN PERF TESTS ##########

		if [ $perf_ans -eq 1 ]
		then
			while IFS= read -r line
			do
				name="-"
				name=${line//" "/$name}

				### SINGLE THREAD PERF TESTS
				sudo perf record -F 100 -a -g -- ./nbody $line
				sudo perf script > "out/perf/$name-single.perf"
				./stackcollapse-perf.pl "out/perf/$name-single.perf" > "out/folded/$name-single.folded"
				./flamegraph.pl --hash "out/folded/$name-single.folded" > "out/svg/$name-single.svg"

				### Multithread perf tests
				sudo perf record -F 100 -a -g -- ./nbdoy $line -t $n_threads
				sudo perf script > "out/perf/$name.perf"
				./stackcollapse-perf.pl "out/perf/$name.perf" > "out/folded/$name.folded"
				./flamegraph.pl --hash "out/folded/$name.folded" > "out/svg/$name.svg"
			done < "$f"
		fi
		rm -f perf_data.old
		rm -f perf_data
	done

	### CLEAN UP ###
	make clean
fi

##### TEST VALIDITY ########
if [ $valid_ans -eq 1 ]
then
	printf "\n************VALIDITY TESTS***************\n"
	### Compile the testing programs ###
	make test_functions
	
	### Run validity tests on test_functions
	./test_functions

	# Clean up the file after
	make clean
fi

