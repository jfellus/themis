#!/bin/bash
################################################################################
# Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
#promethe@ensea.fr
#
# Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
# C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
# M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...
#
# See more details and updates in the file AUTHORS 
#
# This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
# This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
# You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info". 
# As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
# users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
# In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
# that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
# Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
# and, more generally, to use and operate it in the same conditions as regards security. 
# The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
################################################################################

SIMULATOR_PATH="$(pwd)/.."
source $SIMULATOR_PATH/scripts/COMPILE_FLAG

PACKAGES="gtk+-2.0 gmodule-2.0 vte gthread-2.0"
PROMLIB_DIR="$SIMULATOR_PATH/lib/$SYSTEM"
IVYLIB_DIR="$PROMLIB_DIR/ivy"
SCRIPTLIB_DIR="$PROMLIB_DIR/script"
GLADE_LETO_PROM_DIR="$DIR_BIN_LETO_PROM/glades"

CFLAGS="$CFLAGS -I.. -I../shared/include -I/src `pkg-config --cflags $PACKAGES` -I$SIMULATOR_PATH/enet/include -I$HOME/.local/include"  
LIBS="-L$SCRIPTLIB_DIR -lscript -L$IVYLIB_DIR -lglibivy -lpcre -lmxml `pkg-config --libs $PACKAGES` -L$SIMULATOR_PATH/lib/$SYSTEM/comm  -lm -L$HOME/.local/lib -lblc"
SOURCE_DIR="src"
SOURCES=(themis.c themis_gtk.c script_gtk.c script_gtk_cb.c themis_ivy.c themis_ivy_cb.c script.c)
ALL_GLADE_FILES=(distant_promethe.glade themis.glade themis_icon.png oscillo_kernel_icon_small.png epimethe.jpeg)

ENET=1;
OSCILLO_KERNEL_LIB="";

for i in $@
do
	case $i in
		(--disable-enet) ENET = 0;;
	esac
done

if ((ENET))
then 
 	CFLAGS="$CFLAGS -DUSE_ENET=1"; SOURCES=(${SOURCES[@]} enet_server.c);
	OSCILLO_KERNEL_LIB="$OSCILLOKERNELLIBPATH/liboscillo_kernel_debug.a"; 
	LIBS="-L$OSCILLOKERNELLIBPATH -loscillo_kernel_debug -L$SIMULATOR_PATH/lib/$SYSTEM/enet/lib -lenet  -L$GRAPHICLIBPATH -l${GRAPHICLIB} $LIBS"; 
	echo "enet:enabled";
fi

ALL_CONFIGURATIONS=(debug release)
MAKEFILE="Makefile.themis"

echo -e "
OBJS:=\"\"

default:all

$GLADE_LETO_PROM_DIR:
	mkdir -p $GLADE_LETO_PROM_DIR

$GLADE_LETO_PROM_DIR/%:glades/% $GLADE_LETO_PROM_DIR
	cp glades/\$* \$@

all:$DIR_BIN_LETO_PROM/themis $DIR_BIN_LETO_PROM/themis_debug  ${ALL_GLADE_FILES[@]/#/$GLADE_LETO_PROM_DIR/} 

$OSCILLOKERNELLIBPATH/liboscillo_kernel_debug.a:
	cd ../prom_tools&&make 
	
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
		FINAL_CFLAGS="$FLAGS_OPTIM $CFLAGS -DOSCILLO_KERNEL"		
		FINAL_LIBS="$LIBS -lcomm_release"
		TARGET="themis"
	else
		echo -e "\n#*** Debug ****" >> $MAKEFILE		
		OBJECTS_DIR="$OBJPATH/themis/debug"		
		FINAL_CFLAGS="$FLAGS_DEBUG $CFLAGS -DOSCILLO_KERNEL"		
		FINAL_LIBS="$LIBS -lcomm_debug"		
		TARGET="themis_debug" 
	fi
	mkdir -p $OBJECTS_DIR	
		
	LOCAL_OBJECTS=(${SOURCES[@]/%.c/.o})
	OBJECTS=${LOCAL_OBJECTS[@]/#/${OBJECTS_DIR}/}  # Remplace .c par respectivement .o et ajoute au debut ${OBJECT_DIR}/

echo -e "
OBJS+= ${OBJECTS}

.PHONY: $DIR_BIN_LETO_PROM/$TARGET

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
		
$DIR_BIN_LETO_PROM/$TARGET: ${OBJECTS} $OSCILLO_KERNEL_LIB| $OBJECT_DIR $DIR_BIN_LETO_PROM
	@echo Link \$@  	
	@$CC $FINAL_CFLAGS ${OBJECTS} -o $OBJECTS_DIR/themis $FINAL_LIBS
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
