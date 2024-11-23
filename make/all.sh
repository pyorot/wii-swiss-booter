# libraries
printf "\n== lib: DolLoader → _data/dolloader.dol\n"
./make/dl.sh
printf "\n== lib: libruntimeiospatch → _lib/libruntimeiospatch.a\n"
./make/lrip.sh
# main
printf "\n== main: . → _build/boot.[dol|elf]\n"
make
