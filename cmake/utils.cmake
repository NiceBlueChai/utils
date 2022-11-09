# usage:
#   add_executable(applet main.c)
#   define_filename_macro(applet)
function(define_filename_macro targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    # 遍历源文件
    foreach(sourcefile ${source_files})
        # 获取当前源文件的编译参数
        get_property(defs SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS)
        # 获取当前文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        # 获取文件名
        get_filename_component(filepath "${sourcefile}" NAME)
        list(APPEND defs "__FILENAME__=\"${filename}\"")
        # 重新设置源文件的编译参数
        set_property(SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach(sourcefile ${source_files})
endfunction()