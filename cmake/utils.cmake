# usage:
#   add_executable(applet main.c)
#   define_filename_macro(applet)
function(define_filename_macro targetname)
    # 包含必要的模块
    include(CheckCXXSourceCompiles)
    # 测试 __FILE_NAME__ 宏是否可用
    check_cxx_source_compiles("
        #ifndef __FILE_NAME__
            #error \"__FILE_NAME__ is not defined\"
        #endif
        int main() { return 0; }
    " HAVE_FILE_NAME_MACRO)

    if(HAVE_FILE_NAME_MACRO)
        message(STATUS "Compiler supports __FILE_NAME__ macro")
        return()
    else()
        message(STATUS "Compiler does NOT support __FILE_NAME__ macro, we will define it manually")
    endif()

    get_target_property(source_files "${targetname}" SOURCES)
    # 遍历源文件
    foreach(sourcefile ${source_files})
        # 获取当前源文件的编译参数
        get_property(defs SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS)
        # 获取当前文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        # 获取文件名
        get_filename_component(filepath "${sourcefile}" NAME)
        list(APPEND defs "__FILE_NAME__=\"${filename}\"")
        # 重新设置源文件的编译参数
        set_property(SOURCE "${sourcefile}" PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach(sourcefile ${source_files})
endfunction()