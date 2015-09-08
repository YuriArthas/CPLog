# CPLog
CPLog means A cross platform Log classes

#  CPFE可以打印当前函数名以及左右大括号, 并且在函数过程中再次条用CPLog的函数时向后移动log缩进
#  CPME打印一段字符串, 并不会影响缩进
#  打印结果会同时输出在Console/文件/cocos2d的游戏屏幕上, 并且可手动定制输出目标

#  CPFE和CPME的相互辅助使用可以极大提升调试准确度, 特别是在真机测试时, 一旦程序崩溃, 
# 记录在文件内的log会帮助程序员找到问题所在


#-------------------------1-----------------------
int main()
{
    CPFE("");
    return 0;
}

#输出  :
#main
#{
#}

#------------------------------2---------------------

void func()
{
    CPFE("");
    CPME("func-cpme");
}


int main()
{
    CPFE("");
    func();
    CPME("main-cpme");
}

#输出  :
#main
#{
#    func
#    {
#        func_cpme
#    }
#    main-cpme
#}
