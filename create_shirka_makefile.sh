#!/bin/bash

SIMULATOR_PATH="$(pwd)/.."
source $SIMULATOR_PATH/scripts/COMPILE_FLAG

ALL_CONFIGURATIONS=(debug release)

SOURCE_DIR="src"
SOURCES=(shirka.c)

PROMLIB_DIR="$SIMULATOR_PATH/lib/$SYSTEM"
IVYLIB_DIR="$PROMLIB_DIR/ivy"
SCRIPTLIB_DIR="$PROMLIB_DIR/script"


CFLAGS="$CFLAGS -I../shared/include -I/src" 
LIBS="-L$SCRIPTLIB_DIR -lscript -L$IVYLIB_DIR -livy -lpcre"

MAKEFILE="Makefile.shirka"

echo -e "
default:all

OBJS:=\"\"

all:$DIR_BIN_LETO_PROM/shirka $DIR_BIN_LETO_PROM/shirka_debug
	
clean:${ALL_CONFIGURATIONS[@]/#/clean_}

reset:${ALL_CONFIGURATIONS[@]/#/reset_}

" > $MAKEFILE		



for CONFIGURATION in ${ALL_CONFIGURATIONS[@]}
do	
	if [ $CONFIGURATION == "release" ]
	then
		echo -e "\n#*** Release ***" >> $MAKEFILE
		OBJECTS_DIR="$OBJPATH/shirka/release"
		FINAL_CFLAGS="$FLAGS_OPTIM $CFLAGS"						
		TARGET="shirka"
	else
		echo -e "\n#*** Debug ****" >> $MAKEFILE		
		OBJECTS_DIR="$OBJPATH/shirka/debug"		
		FINAL_CFLAGS="$FLAGS_DEBUG $CFLAGS"				
		TARGET="shirka_debug" 
	fi
	mkdir -p $OBJECTS_DIR	
		
	LOCAL_OBJECTS=(${SOURCES[@]/%.c/.o})
	OBJECTS=${LOCAL_OBJECTS[@]/#/${OBJECTS_DIR}/}  # Remplace .c par respectivement .o et ajoute au debut ${OBJECT_DIR}/

echo -e "
OBJS+= ${OBJECTS}

$OBJECTS_DIR:
	mkdir -p \$@
	
$OBJECTS_DIR/%.d:$SOURCE_DIR/%.c | $OBJECTS_DIR
	@echo Create dependence \$*.d
	@$CC $FINAL_CFLAGS -MM \$< -MF \$@.tmp;
	@#rules to make automaticaly recalculate the dependencies 
	@sed 's,\(\$*\)\.o[ :]*,\$(@D)/\1.o \$@ : ,g' < \$@.tmp > \$@
	
$OBJECTS_DIR/%.o: $SOURCE_DIR/%.c | $OBJECTS_DIR
	@echo Compile \$*.o 
	@$CC $FINAL_CFLAGS -MMD -MF \$(@D)/\$*.d.tmp -c \$< -o \$@;
	@#rules to make automaticaly recalculate the dependencies. (@D) correspond au repertoire de la cible que l on ajoute au nom de base \$* 
	@sed 's,\(\$*\)\.o[ :]*,\$(@D)/\1.o \$*.d : ,g' < \$(@D)/\$*.d.tmp > \$(@D)/\$*.d
		
$DIR_BIN_LETO_PROM/$TARGET: ${OBJECTS} | $OBJECTS_DIR $DIR_BIN_LETO_PROM
	@echo Link \$@
	@$CC $FINAL_CFLAGS \$^ -o $OBJECTS_DIR/shirka $LIBS
	@cp $OBJECTS_DIR/shirka \$@ -f
	
clean_${CONFIGURATION}:
	rm -f ${OBJECTS}
	rm $OBJECTS_DIR/*.d.tmp
	rm $OBJECTS_DIR/*.d

reset_${CONFIGURATION}: clean_$CONFIGURATION
	rm -f $DIR_BIN_LETO_PROM/$TARGET
	
" >> $MAKEFILE
done

echo "
#Adding all dependencies 
-include \$(OBJS:.o=.d)
" >> $MAKEFILE			
