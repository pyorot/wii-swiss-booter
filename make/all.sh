# libraries
printf "\n== lib: DolLoader → _data/dolloader.dol\n"
./make/dl.sh
printf "\n== lib: librip → _lib/libruntimeiospatch.a\n"
./make/librip.sh
# main
printf "\n== main: . → _build/boot.[dol|elf]\n"
make
