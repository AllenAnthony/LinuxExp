#! /bin/bash
# Project 2 - experiment 1
# Program:
# 统计指定目录下的普通文件、子目录及可执行文件的数目，
#统计指定目录下所有文件字节数总和，目录的路径名字由参数传入。
# 2016 7/11 qianxufeng ZJU
# ls -l | wc -c
# ls -F | grep '[^/|*=@]$' | wc -l
# ls -F | grep '@$' | wc -l
# ls -F | grep '/$' | wc -l

declare destdir #常量dstdir存储目标目录
if [ $# -eq 0 ] 
    then
   	destdir=./
    elif [ $# -eq 1 ]
    then
        destdir="$1"
    else
        echo "please input one path"
        exit 1
fi
		
cd $destdir
echo "Total number of bytes:     $(ls -l | wc -c)"                       #总字节数
echo "number of Ordinary file:   $(ls -F | grep '[^/|*=@]$' | wc -l)"    #总普通文件数   
echo "number of Executable file: $(ls -F | grep '*$' | wc -l)"           #总可执行文件数
echo "number of Sub directory:   $(ls -F | grep '/$' | wc -l)"           #总子目录数
echo $destdir

exit 0
