#!/bin/bash
# mkdir /home/yyd/2026
# cp /mnt/d/code/share/*  /home/yyd/2026 -rf
# chmod 777 /home/yyd/2026 -R

# echo $1
# echo $2

# echo "命令行参数的总个数" $#

# # 显示命令行所有参数
# echo $*
# echo $@

# 基本运算
echo "请输入"
read n1
echo "请输入"
read n2
echo "n1 + n2 =" `expr $n1 + $n2`
echo "n1 - n2 =" `expr $n1 - $n2`
# 特殊情况
echo "n1 * n2 =" `expr $n1 \* $n2`
echo "n1 * n2 =" $((n1 * n2))

echo "我是\"秦始皇\""