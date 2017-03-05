#! /bin/bash
# Project 2 - experiment 1
# Program:
# ͳ��ָ��Ŀ¼�µ���ͨ�ļ�����Ŀ¼����ִ���ļ�����Ŀ��
#ͳ��ָ��Ŀ¼�������ļ��ֽ����ܺͣ�Ŀ¼��·�������ɲ������롣
# 2016 7/11 qianxufeng ZJU
# ls -l | wc -c
# ls -F | grep '[^/|*=@]$' | wc -l
# ls -F | grep '@$' | wc -l
# ls -F | grep '/$' | wc -l

declare destdir #����dstdir�洢Ŀ��Ŀ¼
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
echo "Total number of bytes:     $(ls -l | wc -c)"                       #���ֽ���
echo "number of Ordinary file:   $(ls -F | grep '[^/|*=@]$' | wc -l)"    #����ͨ�ļ���   
echo "number of Executable file: $(ls -F | grep '*$' | wc -l)"           #�ܿ�ִ���ļ���
echo "number of Sub directory:   $(ls -F | grep '/$' | wc -l)"           #����Ŀ¼��
echo $destdir

exit 0
