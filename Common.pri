CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

#Release编译,生成*.pdb调试文件
QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

#将所有的警告当成错误处理
#QMAKE_CXXFLAGS += \
#    -Werror = return-type \#函数有返回值
#    -Werror = return-local-addr \#返回局部变量地址
#    -Werror = missing-field-initializers \#缺少初始值设定项
#    -Werror = maybe-uninitialized \#变量可能没有被初始化
#    -Werror = delete-non-virtual-dtor \#
#    -Werror = unused-but-set-variable \#设置了但未使用的变量
#    -Werror = parentheses \#括号不匹配
#    -Werror = pointer-arith \#指针用在了算术运算
#    -Werror = reorder \#警告构造函数的顺序不会被使用
#    -Werror = format-extra-args \#格式不对
#    -Werror = format= \#格式不对
#    -Werror = unused-variable \#未使用的变量

#忽略该警告
#QMAKE_CXXFLAGS += \
#    -Wno-unused-function \#未使用的函数
#    -Wno-unused-parameter \#设置了但未使用的参数
#    -Wno-comment \#注释使用不规范。
#    -Wno-sequence-point \#如出现i=i++这类代码，则报警告

contains(QT_ARCH, i386) {
    BIN = bin-32
}else{
    BIN = bin-64
}

CONFIG(debug, debug|release) {
    APP_OUTPUT_PATH = $$PWD/$$BIN/debug
}else{
    APP_OUTPUT_PATH = $$PWD/$$BIN/release
}

INCLUDEPATH += $$PWD/base/
DEPENDPATH  += $$PWD/base/
INCLUDEPATH += $$PWD/3rdparty/
DEPENDPATH  += $$PWD/3rdparty/

defineReplace(replaceLibName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

isEmpty(RC_LANG): RC_LANG = 0x0004
isEmpty(VERSION): VERSION = 0.0.0.1
CONFIG += skip_target_version_ext

isEmpty(QMAKE_TARGET_COMPANY): QMAKE_TARGET_COMPANY = The Youth.
isEmpty(QMAKE_TARGET_DESCRIPTION): QMAKE_TARGET_DESCRIPTION = Youth Application Development Framework
isEmpty(QMAKE_TARGET_COPYRIGHT): QMAKE_TARGET_COPYRIGHT = Copyright (C) 2021 Youth.
isEmpty(QMAKE_TARGET_PRODUCT): QMAKE_TARGET_PRODUCT = AppPlugin
isEmpty(QMAKE_TARGET_ORIGINAL_FILENAME): QMAKE_TARGET_ORIGINAL_FILENAME = AppPlugin
isEmpty(QMAKE_TARGET_INTERNALNAME): QMAKE_TARGET_INTERNALNAME = AppPlugin
isEmpty(QMAKE_TARGET_COMMENTS): QMAKE_TARGET_COMMENTS = Youth Application Development Framework
isEmpty(QMAKE_TARGET_TRADEMARKS): QMAKE_TARGET_TRADEMARKS = Youth
