#! /bin/bash
# Project 2 - experiment 3
# Program:编写一个 shell 脚本，输入一个字符串，忽略（删除）非字母后，检测该字
#符串是否为回文(palindrome)。对于一个字符串，如果从前向后读和从后向前读
#都是同一个字符串，则称之为回文串。例如，单词“mom”，“dad”和“noon”都是回文串
# 2016/7 qianxufeng ZJU
echo "请输入字符串"
read input
declare -i length=${#input}

for((i=0;i<length;i++))                  #排除不是大小写字母的字符
do
   ch=${input:$i:1}
   if [[ $ch > 'a' ]] && [[ $ch < 'z' ]] || [[ $ch > 'A' ]] && [[ $ch < 'Z' ]]
   then
      string="$string""$ch"             #排除其他字符的原字符串
      reserve="$ch""$reserve"           #排除其他字符的原字符串的逆序
   fi
done

echo "string  is $string"
echo "reserve is $reserve"
if [ $string = $reserve ]
then 
   echo "这是回文"
else
   echo "这是不回文"
fi
exit 0
