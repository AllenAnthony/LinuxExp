#! /bin/bash
# Project 2 - experiment 4
# 2016/8/1 qianxufeng ZJU



##################################################   管理员   #############################################3
function login_admin() #管理员登录
{ #	$1：账户 $2：密码	
   	declare -i script=-1
   	declare -i i=0

   	for (( i = 0; i < adminNum; i++ ))
   	do
   		account=$(echo ${admin[$i]}|cut -d ' ' -f1) # 因为管理员用户信息是以字符串"账户 用户名"的形式存储的
   		if [ $1 = $account ]
   			then
   			script=$i
   			break
   		fi
   	done

   	if [ $script -eq -1 ]
   		then
   		echo "该用户不存在"                             # 找不到对应的用户,返回主界面
   		return 0
   	elif [ ${adminPw[$script]} = $2 ]
   		then
   		echo "登陆成功"
   		echo "欢迎您  $1"                              # 找到用户，script即为下标，要匹配密码
   	else
   		echo "登陆失败：账户密码不匹配"                 # 密码错误返回主界面    
   		return 0
   	fi

   	declare option
   	while [ "$option" != "0" ]
   	do
   		echo "0.退出登录
1.教师管理
2.课程管理
3.学生管理
4.管理员账户管理
	
请输入选项序号："
   		read option

   		case $option in
   			"0")
                     return 0
              ;;
            "1")
                     teacher_manage                   #管理教师
              ;;
            "2")
                     course_manage                     #管理课程                    
              ;;
            "3")
                     student_manage                   #管理学生
              ;;
            "4")
                     admin_manage $script               #管理管理员
              ;;
              *)
                     echo "选项不存在"
              ;;
        esac
        echo

   	done
}


function teacher_manage() #管理员管理教师
{
	declare option
	declare -i i=0
   	while [ "$option" != "0" ]
   	do
   		echo "0.返回
1.创建教师账号 : 向数据库中添加新的一组教师工号-用户名-密码数据
2.修改教师账号 : 修改特定工号教师账户的密码
3.删除教师账号 : 删除特定工号的教师账户
4.显示教师账号 : 以工号-用户名列表显示所有的教师账户

请输入选项序号：" 
   		read option

   		case $option in
   			"0")
                     return 0
              ;;
            "1")
                     echo "请输入教师姓名："
                     read name 
                     echo "请输入密码："
                     read password
                     teacherID=$((teacherMax++))
                     add_teacher $teacherID $name $password  #向教师数组中添加信息来增加教师
              ;;
            "2")
                     echo "请输入要修改的教师工号:"
                     read teacherID
                     echo "请输入要修改的教师新密码:"
                     read password
				     find_teacher $teacherID
				     result=$?								#返回值为教师工号对应的编号
				     if [ $result = 255 ]
				     	then
				     	echo "教师工号不存在"
				     else
				     	teacherPw[$result]=$password
					    echo "修改成功"
					fi
              ;;
            "3")
                     echo "请输入要删除的教师工号"
                     read teacherID
				     find_teacher $teacherID
				     result=$?								#返回值为教师工号对应的编号
				     if [ $result = 255 ]
				     	then
				     	echo "教师工号不存在"
				     else
				     	echo "删除${teacher[$result]}成功" #若果存在则设为空值，在写入文件时会忽略空值
				     	((teacherNum--))
				     	teacher[$result]=""
				     	teacherPw[$result]=""					    
					fi
              ;;
            "4")
                     echo "账户 用户名"                 #列出所有教师
				     for ((i=0;i<${#teacher[*]};i++))
				     do
				     	echo ${teacher[$i]}
				     done
              ;;
              *)
                     echo "选项不存在"
                    
              ;;
        esac
        echo

   	done

}

function course_manage() #管理员管理课程
{
	declare option
	declare -i i=0
	while [ "$option" != "0" ]
	do
		echo "0.返回
1.新建课程 : 向数据库中添加一个新的课程(课程编号系统自动生成)
2.编辑课程 : 编辑特定编号的课程的名称
3.删除课程 : 删除特定编号的课程
4.课程-教师绑定 : 将一门课程与一个教师绑定
5.课程-教师解除绑定 : 将一门课程与一个教师解除绑定
6.显示课程列表：以课程名称-教师编号的形式列出现有课程

请输入选项序号：" 
        read option
		case $option in
			"0")
				return 0
			;;
			"1")
				read -p "请输入新课程名称:" name                   #向课程数组中添加信息来增加课程
				read -p "请输入课程介绍：" introdu
				((courseNum++))
				courseID=$((courseMax++))
				course[${#course[*]}]="$courseID $name $introdu"
				echo "成功插入课程：课程ID：$courseID 课程名：$name 介绍：$introdu"
			;;
			"2")
				read -p "请输入要修改的课程名称及其新的名称和介绍:" name newname newintrodu
				find_course $name
				result=$?									#返回值为课程名称对应的编号
				if [ $result = 255 ]
				    then
					echo "课程不存在"
				else
					course[$result]="$courseID $newname $newintrodu"
					echo "修改成功，课程ID：$courseID 课程名：$newname 介绍：$newintrodu"
				fi	
			;;
			"3")
				if [ $courseNum = 0 ]; then
					echo "当前无课程"
				else	
					read -p "请输入要删除的课程名称:" name
					find_course $name
					result=$?									#返回值为课程名称对应的编号
					if [ $result = -1 ]
					    then
						echo "课程名称不存在"
					else
						course[$result]=""
						echo "删除成功"
						((courseNum--))
					fi
				fi
			;;
			"4")
                echo "请输入要绑定的课程名称与教师姓名:"   #在课程数组中的某一项末尾添加教师工号来绑定教师
                read courseName
                read teacherName
                find_course $courseName
                result=$?
                if [ $result = -1 ]
					then
					echo "课程名称不存在"
				else
					course[$result]="${course[$result]} $teacherName"
					echo "成功绑定$courseName与$teacherName"
				fi
			;;
			"5")
                echo "请输入要接触绑定的课程名称与教师姓名:"
                read courseName
                read teacherName
                find_course $courseName
                result=$?
                if [ $result = -1 ]
					then
					echo "课程名称不存在"
				else									
					oldCourse=${course[$result]}   #存储需要解除绑定的课程原本的信息，形如"课程名称 教师编号1 教师编号2"
					oldImfor=$(oldCourse)	  #转字符串为数组以便访问单个元素
					declare newCourse
					for((i=0;i<${#oldImfor[@]};i++))
					do
						if [ oldImfor[$i] != teacherName ]
							then
							newCourse="$newCourse ${oldImfor[$i]}"
						fi
					done
					course[$result]=$newCourse
					echo "解除绑定成功， 该课程信息变为${course[$result]}"
				fi
			;;
			"6")
				echo "课程号 课程名称 课程信息  相关教师"  #列出所有课程与相关教师
				for ((i=0;i<=courseNum;i++))
				do
					echo "$i    ${course[$i]}"
				done
			;;
			*)
				echo "选项不存在"
			;;
		esac
		echo
	done
	

}

function student_manage()  #管理员管理学生
{
	declare option
	declare -i i=0
	while [ "$option" != "0" ]
	do
		echo "0.返回
1.创建学生账号 : 向数据库中添加新的一组学生学号-用户名-密码数据
2.修改学生账号 : 修改特定工号学生账户的密码
3.删除学生账号 : 删除特定工号的学生账户
4.显示学生账号 : 以工号-用户名列表显示所有的学生账户

请输入选项序号：" 
        read option
		case $option in
			"0")
				return 0
			;;
			"1")
				read -p "请输入学生名字:" name
				read -p "请输入学生密码:" password         #向学生数组中添加信息来增加学生
				((studentNum++))
				number=$((studentMax++))
				add_student $number $name $password
				echo "添加成功"
			;;
			"2")
				read -p "请输入要修改的学生学号及新的名字和密码:" number newname newpassword
				find_student $number	#返回值为学生学号对应的编号
				result=$?
				if [ $result = -1 ]
				    then
					echo "学生不存在"
				else
					student[$result]=$newname
					studentPw[$result]=$newpassword
					echo "修改成功"
				fi
			;;
			"3")
				if [ $studentNum = 0 ]
					then
					echo "当前无学生"
				else
					read -p "请输入要删除的学生学号:" number
					find_student $number		#返回值为学生学号对应的编号
					result=$?
					if [ $result = 255 ]
					    then
						echo "学生不存在"
					else
						tmp=${student[$result]}            #若果存在则设为空值，在写入文件时会忽略空值
						student[$result]=""
						studentPw[$result]=""
						stuCourse[$result]=""
						hwFinished[$result]=""
						hwFinishing[$result]=""
						echo "删除学生$tmp成功"
						((studentNum--))
					fi
				fi
			;;
			"4")
				echo "学号 学生姓名"                 #列出所有学生
				for ((i=0;i<studentNum;i++))
				do
					echo ${student[$i]}
				done
			;;
			*)
				echo "选项不存在"
			;;
		esac
	done

}

function admin_manage() #管理员管理管理员
{
	declare option
	declare -i i=0
	while [ "$option" != "0" ]
	do
		echo "0.返回
1.显示管理员列表 : 以工号-管理员名列表显示所有的管理员账户
2.修改密码 : 修改当前管理员的密码

请输入选项序号：" 
        read option
		case $option in
			"0")
				return 0
			;;
			"1")
				echo "管理员账户"                       #列出所有管理员
				for ((i=0;i<adminNum;i++))
				do
					echo ${admin[$i]}
				done
			;;
			"2")
				read -p "请输入您的新密码:" password
				adminPw[$1]=$password
				echo "密码修改成功"
			;;
			*)
				echo "选项不存在"
			;;
		esac
		echo
	done

}


###########################################    教师    ########################################################
function login_teacher() #教师登陆	$1:帐号 $2:密码
{
	declare -i account=-1
	declare -i i=0
	find_teacher $1                #返回值为教师对应的编号
	result=$?
	if [ $result = 255 ]
		then
		echo "用户不存在"
		return 0
	else
		if [ ${teacherPw[$result]} = $2 ]
            then
			echo "登陆成功"
		else
			echo "账号密码不匹配"           #只有密码匹配才会进入选择界面
			return 0
		fi
	fi

	declare option
	while [ "$option" != "0" ]
	do
		echo "0.退出登录
1.课程-学生管理
2.课程信息管理
3.作业管理
	
请输入选项序号：" 
        read option
		case $option in
			"0")
				return 0
			;;
			"1")
				course_student_manage
			;;
			"2")
				teacher_course_manage
			;;
			"3")
				homework_manage
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo
	done
}


function course_student_manage(){ #教师管理学生和课程的关系
	declare option
	while [ "$option" != "0" ]
	do
		echo "0.返回
1.课程添加学生账号
2.查找学生账号 : 根据学号查找学生并输出姓名
3.课程导入学生账户
4.课程删除学生账户

请输入选项序号：" 
        read option
		case $option in
			"0")
				return 0
			;;
			"1")
				read -p "请输入要添加的学生工号-课程号" studentID courseID
				find_student $studentID             #返回值为学生对应的编号
				result=$?
				if [ $result = "-1" ]
					then
					echo "学生不存在"
				else
					stu_course=${stuCourse[$result]}
					find_sub $stu_course $courseID              #在$1中寻找$2，若找到则返回1，不然返回0
					if [ $result = "0" ]
						then
						stu_course="$stu_course $courseID"
					fi
					stuCourse[$result]=$stu_course
				fi
			;;
			"2")
				read -p "请输入要查找的学生工号:" studentID
				find_student $studentID									#返回值为学生工号对应的编号
				result=$?
				if [ $result -eq 255 ]                 #由于函数返回值最大为255
					then
					echo "学生工号不存在"
				else
					echo "${student[$result]}"
				fi
			;;
			"3")
				read -p "请输入要绑定的课程名及学生工号:" courseName stuID
				find_course $coursename										#返回值为课程名称对应的编号
				result=$?
				if [ $result -eq -1 ]
					then
					echo "课程名称不存在"
				else
					courseID=$result											#存储需要绑定的课程编号
					find_student $stuID
					result=$?
					if [ $result -eq -1 ]
					    then
						echo "学生工号不存在"
					else
						stuCourse[$result]="${stuCourse[$result]}""$courseID"
						echo "绑定成功，该学生已绑定了${stuCourse[$result]}等课程"
					fi
				fi
			;;
			"4")
				read -p "请输入要解除绑定的课程名及学生工号:" courseName stuID
				find_course $coursename										#返回值为课程名称对应的编号
				result=$?
				if [ $result -eq -1 ]
					then
					echo "课程名称不存在"
				else
					courseID=$result											#存储需要解除的课程编号
					find_student $stuID
					result=$?
					if [ $result -eq -1 ]
					    then
						echo "学生工号不存在"
					else
						tmp=(${stuCourse[$result]})					#用数组存储所有已选课程
						tmp_new=""
						for (( i=0; i<${#tmp[@]}; i=i+1 ))
						do
							if [ ${tmp[$i]} != $courseID ]
								then
								tmp_new="$tmp_new"" ${tmp[$i]}"
							fi
						done
						stuCourse[$result]=$tmp_new
					fi
				fi
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo
	done
	return 0
}


function teacher_course_manage(){   #教师管理课程
	declare option
	declare -i number=0
	while [ "$option" != "0" ]
	do
		read -p "0.返回
1.新建课程信息 : 新建需要发布的课程信息
2.编辑课程信息 : 编辑已发布的课程信息
3.删除课程信息 : 删除已发布的课程信息
4.显示课程信息 : 显示当前教师用户发布的课程信息

请输入选项序号：" option
		case $option in
			"0")
				return 0
			;;
			"1")                                                                 #新建课程信息             
				read -p "请输入课程名称:" name 
				read -p "请输入课程要发布的课程信息:" msg 
				number=$((courseNum++))
				course[${#course[*]}]="$number""$name""$msg"
				echo "添加$number号课程信息成功"
			;;
			"2")																#编辑课程信息
				read -p "请输入要编辑的课程名称及要发布的课程信息:" name msg
				find_course $name
				result=$?
                number=$result
                first=$(echo ${course[$number]}|cut -d ' ' -f1,2)              #截取课程名之前的那部分
                last=$(echo ${course[$number]}|cut -d ' ' -f1,2,3 --complement) #截取课程名之后的那部分
				course[$number]="$first $msg $last"	
				echo "修改$first课程信息成功"
			;;
			"3")																#删除课程信息
				read -p "请输入要删除的课程名称:" name
				find_course $name
				result=$?
                number=$result
				course[$number]=""	
				echo "删除$name课程信息成功"
			;;
			"4")																#显示课程信息
				echo "编号 课程名称 课程信息内容"
				for ((i=0;i<=${#course[*]};i=i+1))
				do
					echo "${course[$i]}"
				done
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo
	done
}

function homework_manage(){  #教师布置作业和查看作业情况
	declare option
	declare -i number=0
	while [ "$option" != "0" ]
	do
		read -p "0.返回
1.新建作业 : 新建一个作业项目信息
2.编辑作业 : 编辑一个作业信息
3.删除作业 : 删除一个作业
4.显示作业 : 显示所有作业
5.查看所有学生作业完成情况 : 显示各个学生完成作业的情况

请输入选项序号：" option
		case $option in
			"0")
				return 0
			;;
			"1") 																	#新建作业
				read -p "请输入要发布的课程名称和作业名称:" couname hwname
				hwnumber=$((homeworkMax++))
				homework[${#homework[*]}]="$couname $hwnumber $hwname"
				find_course $couname
				result=$?
				courserimf=course[$result]
				courserarr=($courserimf)
				coursenum=${courserarr[0]}               #以上三句都是为了获得课程号
				for ((i=0;i<${#stuCourse[@]};i=i+1))
				do
					find_sub $coursenum                  #找到则返回学生编号，即学生的下标
					result=$?
					if [[ $result = "1" ]]
						then
						hwFinishing[$i]="${hwFinishing[$i]} $hwnumber"
					fi
				done
				echo "添加作业成功"
			;;
			"2")																	#修改作业
				read -p "请输入要编辑的作业编号及修改后的内容:" number name
				find_homework $number
				result=$?
				first=$(echo ${homework[$result]}|cut -d ' ' -f1,2)                   #截取作业名之前的那部分
                last=$(echo ${homework[$result]}|cut -d ' ' -f1,2,3 --complement)	  #截取作业名之后的那部分
				homework[$result]="$first $name $last"
				echo "修改$number号作业成功"
			;;
			"3")																	#删除作业
				read -p "请输入要删除的作业编号:" number
				find_homework $number
				result=$?
				homework[$result]=""	
				echo "删除$number号作业成功"
			;;
			"4")
				echo "编号 作业名称 作业内容"
				for ((i=0;i<${#homework[*]};i=i+1))
				do
					echo "${homework[$i]}"
				done
			;;
			"5")																	#列出所有学生已完成的作业
				echo "学生工号 已完成的作业"
				for ((i=0;i<${#hwFinished[*]};i++))
				do
					echo "${hwFinished[$i]}"
				done
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo
	done
}


############################################       学生            ###################################
function login_student() #学生登陆
{ # $account $passwd
	script="-1"
	for ((i=0;i<${#student[*]};i++))
	do
		account=$(echo ${student[$i]}|cut -d ' ' -f 1)			# 因为用户信息是以字符串"工号 用户名"的形式存储的, 因此只需要空格前的字符
		if [ $account -eq $1 ]
			then
			script=$i
		fi
	done
	if [ $script != "-1" ]
	    then									# 找不到登录帐号对应的用户,返回主界面
		echo "用户工号不存在"
		return 0
	else														# 找到用户，student即为下标，要匹配密码
		if [ ${studentPw[$script]} != $2 ]
			then
			echo "登录成功"
		else
			echo "密码错误"										# 密码错误返回主界面
			return 0
		fi
	fi

	declare option
	while [ "$option" != "0" ]
	do
		read -p "0.退出登录
1.作业管理
2.账户管理
3.查看课程信息

请输入选项序号：" option
		case $option in
			"0")
				return 0
			;;
			"1")
				finishHW $script                 #处理作业的函数
			;;
			"2")
				studentAccount $script            #处理学生账号的函数
			;;
			"3")
				echo "编号 课程名称 课程信息内容"
				for ((i=0;i<${#course[*]};i++))
				do
					echo "${course[$i]}"
				done
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo 
	done
	return 0

}	

function finishHW(){ #学生完成作业	$1->当前操作的学生编号
	declare option
	declare -a hwimf
	while [ "$option" != "0" ]
	do
		read -p "0.返回
1.新建作业 : 查看教师发布的作业并选择一个开始进行
2.编辑作业 : 提交一个正在进行的作业

请输入选项序号：" option
		case $option in
			"0")
				return 0
			;;
			"1")  											#列出所有作业，让学生选择一个来进行
				hwimf=(${hwFinishing[$1]})
				for ((i=0;i<${#hwimf[*]};i++))
				do
					echo "$i ${hwimf[$i]}"
				done
				echo "请选择一个作业开始进行："
				read number
				echo "开始作业${hwimf[$number]}"
			;;
			"2")     										#列出所有作业，让学生选择一个来提交
				Finishing=(${hwFinishing[$1]})
				if [ ${#Finishing[@]} = "0" ]
					then
					echo "当前没有进行中的作业"
				else
					echo "正在进行的作业号:"
					echo "编号 作业名称 作业内容"
					hwimf=(${hwFinishing[$1]})
					for ((i=0;i<${#hwimf[*]};i++))
				    do
					    echo "$i ${hwimf[$i]}"
				    done

					read -p "请输入要提交的作业编号:" number 						#从该学生的hwFinishing中删除该作业编号
                    declare finishing_new
                    finished_new=(${hwFinishing[$1]})
                    hwFinishing[$1]=""
					for ((i=0;i<${#finishing_new[@]};i++)) 
					do
						if [ ${finishing_new[$i]} != ${hwimf[$number]} ]
							then
							hwFinishing[$1]="${hwFinishing[$1]} ${finishing_new[$i]}"
						fi
					done
					hwFinished[$1]="${hwFinished[$1]} ${hwimf[$number]}"         #把该作业编号添加进该学生的hwFinished

					account=$(echo ${student[$1]}|cut -d ' ' -f 1)
					find_homework ${hwimf[$number]}
					result=$?
					homework[$result]="${homework[$result]} $account"			

					echo "编号为$number的作业已完成提交"
				fi
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo 
	done
}

function studentAccount(){  #学生查看账户
#	$1->编号
	declare option
	while [ "$option" != "0" ]
	do
		read -p "0.返回
1.查看已选课程 : 查看自己已选的课程
2.查看已完成作业 ： 查看自己已完成的作业
3.修改密码 : 修改当前账户的密码

请输入选项序号：" option
		case $option in
			"0")
				return 0
			;;
			"1")
				echo "学生信息：${student[$1]} 已选课程：${stuCourse[$1]}"
			;;
			"2")
				echo "学生信息：${student[$1]} 已完成的作业：${hwFinished[$1]}"
			;;
			"3")
				read -p "请输入您的新密码:" passwd
				studentPw[$1]="$passwd"
				echo "密码修改成功"
			;;
			*)
				echo "选项不存在, 请输入正确的序号"
			;;
		esac
		echo
	done
}

############################################       add / find函数    ###################################
function add_admin() #添加管理员 $1:name $2:passwd
{
	find_admin $1
	result=$?
	if [ $result = "255" ]
		then
	    admin[${#admin[*]}]="$1"
	    adminPw[${#adminPw[*]}]="$2"
	    ((adminNum++))
	    echo "管理员添加成功：$1"
	else
		echo "管理员已存在"
	fi
}	

function add_teacher() #添加教师 $1:teacherID $2:name $3:passwd
{
	teacher[${#teacher[*]}]="$1 $2"
	teacherPw[${#teacherPw[*]}]=$3
	((teacherNum++))
	echo "添加成功 您的ID 名字 密码 分别是：$teacherID $name $password"
}

function add_student() #添加学生 $1:studentID $2:name $3:passwd#要处理很多数组
{
	student[${#student[*]}]="$1 $2"
	studentPw[${#studentPw[*]}]=$3
	((studentNum++))
	stuCourse[${#student[*]}]=""
	hwFinished[${#student[*]}]=""
	hwFinishing[${#student[*]}]=""
	echo "学生添加成功：$1 $2"
}

function find_sub() #在字符串中匹配子字符串 $stu_course $courseID
{
	declare -i i=0
	declare -a string
	string=($1)
	for((i=0;i<${#string[*]};i++))
	do
		if [ ${string[$i]} = $courseID ]
			then
			return 1
		fi
	done

    return 0
}

function find_homework() # 根据作业号查找作业编号即下标 $hwnumber
{
	declare -i i=0
	declare -a string
	for((i=0;i<${#homework[*]};i++))
	do
		hwnum=$(echo ${homework[$i]}|cut -d ' ' -f2)
		if [ $hwnum = $1 ]
			then
			return $i
		fi
	done

    return 255
}

function find_course() ## 根据课程号查找课程编号即下标 $1:课程名 返回课程编号而不是课程号
{
	declare -ai result
	declare -i cou=0
	declare -i i=0
	declare -i script
	for ((i=0;i<${#course[*]};i++))
	do
		courseName=$(echo ${course[$i]}|cut -d ' ' -f2)
		if [ $1 = $courseName ]
			then
			result[$cou]=$i
			((cou++))
		fi
	done

	if [ $cou -eq 1 ]
		then
		return ${result[0]}
	elif [ $cou -eq 0 ]
		then
		return 255
	else
		echo "请从以下课程中选择一门课："
		for((i=0;i<cou;i++))
		do
			script=${result[$i]}
			echo "$i ${course[$script]}"
		done
		read script
		while [ $script -ge $cou -o $script -lt 0 ]
		do
			echo "输入有误，请重新输入："
			read script
		done
		return ${result[$script]}
	fi
}



function find_teacher() ## 根据工号查找教师编号即下标 $1:教师工号
{
	declare -i i=0
	for ((i=0;i<${#teacher[*]};i++))
	do
		teacherID=$(echo ${teacher[$i]}|cut -d ' ' -f1)
		if [ $1 = $teacherID ]
			then
			return $i
		fi
	done

	return 255
}

function find_student() ## 根据学号查找学生编号即下标 $1:学生学号
{
	declare -i i=0
	for ((i=0;i<${#student[*]};i++))
	do
		studentID=$(echo ${student[$i]}|cut -d ' ' -f1)
		if [ $1 = $studentID ]
			then
			return $i
		fi
	done

	return 255
}

function find_admin() ## 根据账户查找管理员编号即下标 $1:管理员账户
{
	declare -i i=0
	for ((i=0;i<${#admin[*]};i++))
	do
		adminID=${admin[$i]}
		if [ $1 = $adminID ]
			then
			return $i
		fi
	done

	return 255
}















##############################################   写入文件   #####################################################

function writeback #将所有信息写入文件来为下次运行程序做准备
{
> ./imfor
> ./admin
> ./adminPw
> ./teacher
> ./teacherPw
> ./student
> ./studentPw
> ./stuCourse
> ./hwFinished
> ./hwFinishing
> ./course
> ./homework

echo $adminNum >> ./imfor
echo $teacherNum >> ./imfor
echo $studentNum >> ./imfor
echo $courseNum >> ./imfor
echo $homeworkNum >> ./imfor

echo $adminMax >> ./imfor
echo $teacherMax >> ./imfor
echo $studentMax >> ./imfor
echo $courseMax >> ./imfor
echo $homeworkMax >> ./imfor

for((cou=0;cou<${#admin[*]};cou++))
do

	if [ "${admin[$cou]}" ]
		then
		echo ${admin[$cou]} >> ./admin
		echo ${adminPw[$cou]} >> ./adminPw
	fi
done

for((cou=0;cou<${#teacher[*]};cou++))
do
	if [ "${teacher[$cou]}" ]
		then
		echo ${teacher[$cou]} >> ./teacher
		echo ${teacherPw[$cou]} >> ./teacherPw
	fi
done

for((cou=0;cou<studentNum;cou++))
do
	if [ "${student[$cou]}" ]
		then
		echo ${student[$cou]} >> ./student
		echo ${studentPw[$cou]} >> ./studentPw
		echo ${stuCourse[$cou]} >> ./stuCourse
		echo ${hwFinished[$cou]} >> ./hwFinished
		echo ${hwFinishing[$cou]} >> ./hwFinishing
	fi
done


for((cou=0;cou<courseNum;cou++))
do
	if [ "${course[$cou]}" ]
		then
		echo ${course[$cou]} >> ./course
	fi
done

for((cou=0;cou<homeworkNum;cou++))
do
	if [ "${homework[$cou]}" ]
		then
		echo ${homework[$cou]} >> ./homework
	fi
done
}


#####################################      主程序	#####################################################

declare -a admin          #管理员的账户，元素的下标为该用户的编号（从0开始， 不同于工号）
declare -a teacher        #老师的工号与账户，元素的下标为该用户的编号（从0开始， 不同于工号）
declare -a student	  #学生的学号与账户，元素的下标为该用户的编号（从0开始， 不同于工号）
	
# 这三个数组元素各自存放三种用户的密码，下标为对应的用户的编号
declare -a adminPw
declare -a teacherPw
declare -a studentPw		


declare -a course		# 元素为格式"课程号 课程名称 课程信息 教师工号"的字符串
declare -a stuCourse		# 存储每个学生已选课程的课程号，下标为对应的学生的编号
declare -a homework		# 每个元素内容为课程号 作业号 已完成该作业的学生编号(空格隔开)
declare -a hwFinished		# 记录每个学生已完成的作业号，下标为对应的学生的编号
declare -a hwFinishing		# 记录每个学生未完成的作业号，下标为对应的学生的号


#管理员，老师，学生，课程，作业的现有数量					
declare -i adminNum=0
declare -i teacherNum=0
declare -i studentNum=0		
declare -i courseNum=0										
declare -i homeworkNum=0	


#管理员，老师，学生，课程，作业的最大号数					
declare -i adminMax=0
declare -i teacherMax=0
declare -i studentMax=0		
declare -i courseMax=0										
declare -i homeworkMax=0

#从文件中读取信息
declare -i number=0
declare -i cou=0
declare line

while read number
do
   case $cou in
		"0")
			adminNum=$number

		;;
		"1")
			teacherNum=$number
		;;
		"2")
			studentNum=$number
		;;
		"3")
			courseNum=$number
		;;
		"4")
			homeworkNum=$number
		;;
		"5")
			adminMax=$number
		;;
		"6")
			teacherMax=$number
		;;
		"7")
			studentMax=$number
		;;
		"8")
			courseMax=$number
		;;
		"9")
			homeworkMax=$number
		;;
		*)
			echo "imfor文件有错"
		;;
   esac 
   cou=$(($cou+1))
done < ./imfor

#################   账号文件   ####################
cou=0
while read line
do
   admin[$cou]=$line
   ((cou++))
done < ./admin

if [ $cou -ne $adminNum ]
then
   echo "admin文件有错"
fi

cou=0
while read line
do
   teacher[$cou]=$line
   cou=$(($cou+1))
done < ./teacher
if [ $cou -ne $teacherNum ]
then
   echo "teacher文件有错"
fi


cou=0
while read line
do
   student[$cou]=$line
   cou=$(($cou+1))
done < ./student

if [ $cou -ne $studentNum ]
then
   echo "student文件有错"
fi


#################   密码文件   ####################
cou=0
while read line
do
   adminPw[$cou]=$line
   cou=$(($cou+1))
done < ./adminPw

if [ $cou -ne $adminNum ]
then
   echo "adminPw文件有错"
fi

cou=0
while read line
do
   teacherPw[$cou]=$line
   cou=$(($cou+1))
done < ./teacherPw

if [ $cou -ne $teacherNum ]
then
   echo "teacherPw文件有错"
fi


cou=0
while read line
do
   studentPw[$cou]=$line
   cou=$(($cou+1))
done < ./studentPw

if [ $cou -ne $studentNum ]
then
   echo "studentPw文件有错"
fi

#########################   课程作业文件   #################################
cou=0
while read line
do
   course[$cou]=$line
   cou=$(($cou+1))
done < ./course

if [ $cou -ne $courseNum ]
then
   echo "course文件有错"
fi

cou=0
while read line
do
   stuCourse[$cou]=$line
   cou=$(($cou+1))
done < ./stuCourse

if [ $cou -ne $studentNum ]
then
   echo "stuCourse文件有错"
fi

cou=0
while read line
do
   homework[$cou]=$line
   cou=$(($cou+1))
done < ./homework

if [ $cou -ne $homeworkNum ]
then
   echo "homework文件有错"
fi

cou=0
while read line
do
   hwFinished[$cou]=$line
   cou=$(($cou+1))
done < ./hwFinished

if [ $cou -ne $studentNum ]
then
   echo $studentNum
   echo $cou
   echo "hwFinished文件有错"
fi

cou=0
while read line
do
   hwFinishing[$cou]=$line
   cou=$(($cou+1))
done < ./hwFinishing

if [ $cou -ne $studentNum ]
then
   echo "hwFinishing文件有错"
fi




# 默认测试账户, 初始帐号密码都是123
add_admin 0 0			


# echo "数量"
# echo $adminNum
# echo $teacherNum
# echo $studentNum
# echo $courseNum
# echo $homeworkNum
# echo "编号最大值"
# echo $adminMax
# echo $teacherMax
# echo $studentMax
# echo $courseMax
# echo $homeworkMax
# echo "数组"
# echo ${admin[@]}
# echo ${teacher[@]}
# echo ${student[@]}
# echo ${adminPw[@]}
# echo ${teacherPw[@]}
# echo ${studentPw[@]}

# echo ${course[@]}
# echo ${stuCourse[@]}

# echo ${homework[@]}
# echo ${hwFinished[@]}
# echo ${hwFinishing[@]}

#####################################			主界面				#######################################

declare -i option=1                            
while [ option != "0" ]
do
	read -p "0.退出系统
1.管理员登录
2.教师帐号登录
3.学生帐号登录

请输入选项序号：" option
	case $option in
		"0")			
			writeback
			echo "退出系统成功"
			exit 0
		;;
		"1")
			read -p "请输入管理员账号:" account
			read -p "请输入管理员密码:" passwd
			login_admin $account $passwd
		;;
		"2")
			read -p "请输入教师工号:" account
			read -p "请输入教师密码:" passwd
			login_teacher $account $passwd
		;;
		"3")
			read -p "请输入学生学号:" account
			read -p "请输入学生密码:" passwd
			login_student $account $passwd
		;;
		*)
			echo "选项不存在, 请输入正确的序号"
		;;
	esac
	echo 
done
