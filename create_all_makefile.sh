#!/bin/bash

source ../scripts/COMPILE_FLAG


ALL_PROJECTS=(themis shirka)
ALL_ACTIONS=(all clean reset)

echo "include ../scripts/variables.mk" > Makefile

echo -e "all:${ALL_PROJECTS[@]/#/all_}\n" >> Makefile

echo -e "clean: ${ALL_PROJECTS[@]/#/clean_}\n" >> Makefile

echo -e "reset: ${ALL_PROJECTS[@]/#/reset_}\n" >> Makefile

for PROJECT in ${ALL_PROJECTS[@]}
do
	./create_${PROJECT}_makefile.sh $@
	for ACTION in ${ALL_ACTIONS[@]}
	do
		echo "${ACTION}_${PROJECT}: | ${LOGDIR}" >> Makefile 
		echo -e "\tmake --jobs=$NUMBER_OF_CORES -f Makefile.$PROJECT $ACTION > ${LOGDIR}/${PROJECT}_${ACTION}_make.log" >> Makefile
	done	
done 





