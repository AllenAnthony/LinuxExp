#! /bin/bash
# Project 2 - experiment 2
# Program:用 bash 编写程序，该程序从键盘输入的 100 个整数，计算其平均值、最大值和最小值并输出，从小到大排序并输出
# 2016/7 qianxufeng ZJU


declare -r size=5
declare -a integer[100]
echo "Please input $size intergers:"
declare -i ave=0

for ((cou=0;cou<size;cou++))
do
	read integer[$cou]
	ave=$(($ave+${integer[$cou]}))    #计算总和

        for((num=cou;num>0;num--))  #每当输入一个数，将其从小到大差入队列中
        do
             if [ ${integer[$num]} -lt ${integer[$(($num-1))]} ]
             then
                  tmp=${integer[$num]}
                  integer[$num]=${integer[$(($num-1))]}
                  integer[$num-1]=$tmp
             fi
        done

done

echo "The average is $(($ave/$size))"              #输出结果
echo "The maximum is ${integer[$(($size-1))]}"
echo "The minimum is ${integer[0]}"

for ((cou=0;cou<size;cou++))
do
    echo ${integer[$cou]}
done

exit 0
