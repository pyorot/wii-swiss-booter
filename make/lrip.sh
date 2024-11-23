cd libruntimeiospatch
make clean
make
mkdir -p ../_lib
cp -f libruntimeiospatch.a ../_lib/libruntimeiospatch.a
make clean
