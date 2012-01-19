include ../scripts/variables.mk
all:all_themis all_shirka

clean: clean_themis clean_shirka

reset: reset_themis reset_shirka

all_themis: | /tmp/arnablan/logs
	make -j -f Makefile.themis all > /tmp/arnablan/logs/themis_all_make.log
clean_themis: | /tmp/arnablan/logs
	make -j -f Makefile.themis clean > /tmp/arnablan/logs/themis_clean_make.log
reset_themis: | /tmp/arnablan/logs
	make -j -f Makefile.themis reset > /tmp/arnablan/logs/themis_reset_make.log
all_shirka: | /tmp/arnablan/logs
	make -j -f Makefile.shirka all > /tmp/arnablan/logs/shirka_all_make.log
clean_shirka: | /tmp/arnablan/logs
	make -j -f Makefile.shirka clean > /tmp/arnablan/logs/shirka_clean_make.log
reset_shirka: | /tmp/arnablan/logs
	make -j -f Makefile.shirka reset > /tmp/arnablan/logs/shirka_reset_make.log
