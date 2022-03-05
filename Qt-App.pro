TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    3rdparty \
    base \
    plugintemplate \
    picturetool \
    mediaplayer

TRANSLATIONS += \
    translations/language.zh_cn.ts \
    translations/language.zh_en.ts

DISTFILES += \
    $$files(docs/*.png)\
    LICENSE \
    README.md \
    README_en.md

message("1.Build;")
message("2.Copy the file in the resource/ directory to the executable program directory;")
message("3.Run the program.");
