# libraries
printf "\n== lib: DolLoader → _/data/dolloader.dol\n"
./make/dl.sh || exit
printf "\n== lib: librip → _/lib/libruntimeiospatch.a\n"
./make/librip.sh || exit
# main
printf "\n== main: . → _/build/boot.[dol|elf]\n"
make || exit
# release
printf "\n== release: . → _/wii-swiss-booter.7z\n"
./make/release.sh
