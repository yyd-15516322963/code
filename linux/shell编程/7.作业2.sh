#!/bin/bash

fun()
{
    cat > $file << 'EOF'
#include <stdio.h>
int main() 
{
    return 0;
}
EOF

}

file=$1

if test -e $file
then
    if test -w $file && test -r $file
    then
        fun
#         cat > $file << 'EOF'
# #include <stdio.h>
# int main() 
# {
#     return 0;
# }
# EOF
    echo "文件存在，可读可写，写入成功！"
    cat $file
    else
        chmod 777 $file
        fun
#         cat > $file << 'EOF'
        
# #include <stdio.h>
# int main() 
# {
#     return 0;
# }
# EOF
    echo "文件存在，不可读可写，修改权限为777，写入成功！"
    cat $file
    fi
else
    touch $file
    chmod 777 $file 
    fun
#     cat > $file << 'EOF'
# #include <stdio.h>
# int main() 
# {
#     return 0;
# }
# EOF
    echo "文件不存在，创建并修改权限为777，写入成功！"
    cat $file
fi