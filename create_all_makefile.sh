#!/bin/bash

SIMULATOR_PATH="$(pwd)/.."
source $SIMULATOR_PATH/scripts/COMPILE_FLAG

ALL_PROJECTS=(themis shirka)
ALL_ACTIONS=(all clean reset)

echo -e "all:${ALL_PROJECTS[@]/#/all_}
" > Makefile
echo -e "clean: ${ALL_PROJECTS[@]/#/clean_}
" >> Makefile
echo -e "reset: ${ALL_PROJECTS[@]/#/reset_}
" >> Makefile

for PROJECT in ${ALL_PROJECTS[@]}
do
	./create_${PROJECT}_makefile.sh $@
	for ACTION in ${ALL_ACTIONS[@]}
	do
		echo "${ACTION}_${PROJECT}: | ${LOGDIR}" >> Makefile 
		echo -e "\tmake -j$NUMBER_OF_PROCESSORS -f Makefile.$PROJECT $ACTION > ${LOGDIR}/${PROJECT}_${ACTION}_make.log" >> Makefile
	done	
done 





