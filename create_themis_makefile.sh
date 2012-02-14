#!/bin/bash

SIMULATOR_PATH="$(pwd)/.."
source $SIMULATOR_PATH/scripts/COMPILE_FLAG

PACKAGES="gtk+-2.0 gmodule-2.0 vte gthread-2.0"
PROMLIB_DIR="$SIMULATOR_PATH/lib/$SYSTEM"
IVYLIB_DIR="$PROMLIB_DIR/ivy"
SCRIPTLIB_DIR="$PROMLIB_DIR/script"
GLADE_LETO_PROM_DIR="$DIR_BIN_LETO_PROM/glades"

CFLAGS="$CFLAGS -I.. -I../shared/include -I/src `pkg-config --cflags $PACKAGES` -I$SIMULATOR_PATH/enet/include" 
LIBS="-L$SCRIPTLIB_DIR -lscript -L$IVYLIB_DIR -lglibivy -lmxml `pkg-config --libs $PACKAGES` -L$SIMULATOR_PATH/lib/$SYSTEM/comm"
SOURCE_DIR="src"
SOURCES=(themis.c themis_gtk.c script_gtk.c script_gtk_cb.c themis_ivy.c themis_ivy_cb.c)
ALL_GLADE_FILES=(distant_promethe.glade themis.glade themis_icon.png oscillo_kernel_icon.png)


for i in $@
do
	case $i in
		(--enable-enet) CFLAGS="$CFLAGS -DUSE_ENET"; SOURCES=(${SOURCES[@]} oscillo_kernel.c); ALL_GLADE_FILES=(${ALL_GLADE_FILES[@]} oscillo_kernel.glade)  LIBS="$LIBS -L$SIMULATOR_PATH/lib/$SYSTEM/enet/lib -lenet";echo "enet:enabled";;
	esac
done

ALL_CONFIGURATIONS=(debug release)
MAKEFILE="Makefile.themis"

echo -e "
OBJS:=\"\"

default:all

$DIR_BIN_LETO_PROM: 
	mkdir -p \$@

$GLADE_LETO_PROM_DIR:
	mkdir -p $GLADE_LETO_PROM_DIR

$GLADE_LETO_PROM_DIR/%:glades/% $GLADE_LETO_PROM_DIR
	cp glades/\$* \$@

all:$DIR_BIN_LETO_PROM/themis $DIR_BIN_LETO_PROM/themis_debug  ${ALL_GLADE_FILES[@]/#/$GLADE_LETO_PROM_DIR/} 
	
clean:${ALL_CONFIGURATIONS[@]/#/clean_}

reset:${ALL_CONFIGURATIONS[@]/#/reset_}
" > $MAKEFILE


echo "#On force le makefile a recompiler l'objet principal pour mettre a jour la version" >> $MAKEFILE

echo 'SVN_REVISION:=-DSVN_REVISION="$(shell svnversion -n .)"' >> $MAKEFILE

for CONFIGURATION in ${ALL_CONFIGURATIONS[@]}
do	
	if [ $CONFIGURATION == "release" ]
	then
		echo -e "\n#*** Release ***" >> $MAKEFILE
		OBJECTS_DIR="$OBJPATH/themis/release"
		FINAL_CFLAGS="$FLAGS_OPTIM $CFLAGS"		
		FINAL_LIBS="$LIBS -lcomm_release"
		TARGET="themis"
	else
		echo -e "\n#*** Debug ****" >> $MAKEFILE		
		OBJECTS_DIR="$OBJPATH/themis/debug"		
		FINAL_CFLAGS="$FLAGS_DEBUG $CFLAGS"		
		FINAL_LIBS="$LIBS -lcomm_debug"		
		TARGET="themis_debug" 
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
	@$CC $FINAL_CFLAGS -MM \$< -MF \$@.tmp
	@#rules to make automaticaly recalculate the dependencies 
	@sed 's,\(\$*\)\.o[ :]*,\$(@D)/\1.o \$@ : ,g' < \$@.tmp > \$@
	
$OBJECTS_DIR/%.o:$SOURCE_DIR/%.c | $OBJECTS_DIR
	@echo Compile \$*.o 
	@$CC $FINAL_CFLAGS \$(SVN_REVISION) -MMD -MF \$(@D)/\$*.d.tmp -c \$< -o \$@
	@#rules to make automaticaly recalculate the dependencies 
	@sed 's,\(\$*\)\.o[ :]*,\$(@D)/\1.o \$*.d : ,g' < \$(@D)/\$*.d.tmp > \$(@D)/\$*.d
		
$DIR_BIN_LETO_PROM/$TARGET: ${OBJECTS} | $OBJECT_DIR $DIR_BIN_LETO_PROM
	@echo Link \$@  	
	@$CC $FINAL_CFLAGS \$^ -o $OBJECTS_DIR/themis $FINAL_LIBS
	@cp -f $OBJECTS_DIR/themis \$@
	
clean_${CONFIGURATION}:
	rm -f ${OBJECTS}
	rm $OBJECTS_DIR/*.d.tmp
	rm $OBJECTS_DIR/*.d

reset_$CONFIGURATION:clean_$CONFIGURATION 
	rm -f $DIR_BIN_LETO_PROM/$TARGET ${ALL_GLADE_FILES[@]/#/$GLADE_LETO_PROM_DIR/}
		
" >> $MAKEFILE
done

echo "
#Adding all dependencies 
-include \$(OBJS:.o=.d)
" >> $MAKEFILE			
