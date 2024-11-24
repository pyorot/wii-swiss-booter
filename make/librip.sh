cd librip
make clean
make
mkdir -p ../_/lib
cp -f libruntimeiospatch.a ../_/lib/libruntimeiospatch.a
make clean
