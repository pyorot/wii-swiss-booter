# requires p7zip: run "sudo apt install p7zip-full"
mkdir -p _/temp/apps/Swiss
cd _/temp
cp -f ../build/boot.dol apps/Swiss && {
    rm -f ../wii-swiss-booter.7z
    cp -f ../../meta/* apps/Swiss
    7z a ../wii-swiss-booter apps | grep -e "archive:" -e "Ok"
}
cd ../..
rm -r _/temp
