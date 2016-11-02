#!/bin/bash  
# program：
#	Calculate the current folder all the files MD5 value.
# History:
# 2016/10/28	Shang Yexin		First release

function ergodic(){  
        for file in ` ls $1 `  
        do  
                if [ -d $1"/"$file ]  
                then  
                        ergodic $1"/"$file  
                else  
                        md5sum $1"/"$file | sed "s#$INIT_PATH/##" >> $RECORDFILE
                fi  
        done  
}

#设置输出文件名
RECORDFILE=check.md5
#如果存在先删除，防止重复运行脚本时追加到记录文件
test -e $RECORDFILE && rm $RECORDFILE
#获取当前目录
INIT_PATH=`pwd`
#遍历所有文件
ergodic $INIT_PATH
#按文件名称排序排序
sort -t ' ' -k 2 $RECORDFILE -o $RECORDFILE
