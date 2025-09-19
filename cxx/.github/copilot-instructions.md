# Copilot 使用说明

- 本项目优先使用现代 C++17 语法，禁止使用裸指针，推荐智能指针。
- 日志统一采用 spdlog或者qt的qDebug，qInfo之类的，不允许直接用 printf和iostream。
- 网络通信层全部用 asio，不要用 boost::asio。
- 所有异常必须记录日志后再抛出，不允许裸抛异常。
- 工程结构需遵循 src/include/tests 目录标准。
- 如果涉及 Qt UI，请使用 Qt Designer 生成的 .ui 文件配合代码实现。
- 避免生成与当前项目风格不符的代码。
- 我的名字是 "NiceBlueChai"，邮箱是"bluechai@qq.com", 今天是 2025年9月19日， 请在代码注释中使用中文。
- 所有注释使用doxygen风格，不要增加多余的注释。