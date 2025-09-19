/**
 * @file restart_helper.h
 * @brief ��ƽ̨Ӧ�ó�����������������
 *
 * �ṩ Windows �� Linux/Unix �µ�Ӧ������ʵ�֣��Զ����ɲ�ִ�нű������������
 *
 * @author NiceBlueChai
 * @email bluechai@qq.com
 * @date 2025-09-19
 */
#pragma once
#include <QtGlobal>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include <QFileInfo>
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
#include <unistd.h>

/**
 * @brief ������ǰӦ�ã�Linux/Unix����
 */
inline void restartAppLinux()
{
    QString appPath = QCoreApplication::applicationFilePath();
    QStringList args = QCoreApplication::arguments();
    QString shFile = QDir::temp().filePath("restart_app.sh");
    int pid = getpid();
    QFile file(shFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "#!/bin/bash\n";
        out << "while kill -0 " << pid << " 2>/dev/null; do sleep 1; done\n";
        out << "nohup \"" << appPath << "\"";
        for (int i = 1; i < args.size(); ++i)
            out << " \"" << args[i].replace("\"", "\\\"") << "\"";
        out << " &\n";
        out << "rm -- \"$0\"\n";
        file.close();
        QFile::setPermissions(shFile, QFile::permissions(shFile) | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther);
        QProcess::startDetached("/bin/bash", QStringList() << shFile);
    }
    QCoreApplication::quit();
}
#endif

/**
 * @brief ������ǰӦ�ã�Windows����
 */
inline void restartAppWin()
{
    QString appPath = QCoreApplication::applicationFilePath();
    QStringList args = QCoreApplication::arguments();
    QString batFile = QDir::temp().filePath("restart_app.bat");
    QFile file(batFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "@echo off\n";
        out << ":loop\n";
        out << "tasklist | findstr /I \"" << QFileInfo(appPath).fileName() << "\" >nul\n";
        out << "if not errorlevel 1 (\n";
        out << "  timeout /T 1 /NOBREAK >nul\n";
        out << "  goto loop\n";
        out << ")\n";
        out << "start \"\" \"" << appPath << "\"";
        for (int i = 1; i < args.size(); ++i)
            out << " \"" << args[i].replace("\"", "\\\"") << "\"";
        out << "\n";
        out << "del \"%~f0\"\n";
        out << "exit\n";
        file.close();
        QProcess::startDetached("cmd.exe", QStringList() << "/C" << batFile);
    }
    QCoreApplication::quit();
}