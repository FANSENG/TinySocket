# 进入项目根目录
mypath=$(dirname $0)
myname=$(basename $0)
cd "$mypath"


rm -rf build
mkdir build
cd build
cmake ..
make

