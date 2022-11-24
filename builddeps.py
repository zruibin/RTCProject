#! /usr/bin/env python3
# -*- coding: utf-8 -*- 
#
# builddeps.py
#
# Created by Ruibin.Chow on 2022/01/26.
# Copyright (c) 2022年 Ruibin.Chow All rights reserved.
# 

"""

"""

import os, re, json, sys, platform
import subprocess, shutil
import datetime
import tarfile, gzip
import urllib.request


homeDir = ""
sourceDir = ""
outputDir = ""
sourceDirName = "depsSource"
outputDirName = "deps"
cmakeOther = ""
libSufixs = [".a", ".so", ".dylib", "dll"]

logList = []

def logRecord():
    with open('builddeps.log', 'w') as fileHandle:
        for logStr in logList:
            fileHandle.write(str(logStr))

def log(string="", newline=True):
    if newline:
        logList.append(str(string) + "\n")
        print(string, end="\n")
    else:
        logList.append(str(string))
        print(string, end="")
    pass

def operator(cmdString, newline=True):
    log(cmdString)
    output = os.popen(cmdString)
    for line in output.readlines():
        log(line, newline)

def operatorCMD(parameterList, newline=True):
    cmdString = " ".join(parameterList)
    operator(cmdString, newline)
    pass

def getAllFileInDirectory(DIR, beyoundDir=''):
    """返回指定目录下所有文件的集合，beyoundDir的目录不包含"""
    array = []
    print(DIR+beyoundDir)
    for root, dirs, files in os.walk(DIR):
        if len(beyoundDir) != 0 and os.path.exists(DIR+beyoundDir):
            if beyoundDir not in dirs:
                continue
        for name in files:
            path = os.path.join(root,name)
            array.append(path)
    return array


def cmakeBuild(fileName, cmakeArgs, genBuilding=True, preCmdList=[], install=True):
    os.chdir(fileName)
    if genBuilding:
        buildDir = "build"
        if os.path.exists(buildDir):
            shutil.rmtree(buildDir)
        os.makedirs(buildDir)
        os.chdir(buildDir)
    log("当前编译路径：" + os.getcwd())
    if len(preCmdList) > 0:
        operatorCMD(preCmdList, False)
    if not cmakeArgs:
        cmakeArgs = ""
    cmdList = ["cmake",
                cmakeArgs,
                "-D CMAKE_BUILD_TYPE=RELEASE", 
                "-D", "CMAKE_INSTALL_PREFIX="+outputDir, 
                "..",
                ]
    operatorCMD(cmdList, False)
    if install:
        operator("cmake --build . --target install", False)
    pass


def untar(fname, dirs):
    """
    解压tar.gz文件
    :param fname: 压缩文件名
    :param dirs: 解压后的存放路径
    :return: bool
    """
    try:
        t = tarfile.open(fname)
        t.extractall(path = dirs)
        return True
    except Exception as e:
        print(e)
        return False


def callbackfunc(blocknum, blocksize, totalsize):
    '''回调函数
    @blocknum: 已经下载的数据块
    @blocksize: 数据块的大小
    @totalsize: 远程文件的大小
    '''
    percent = 100.0 * blocknum * blocksize / totalsize
    if percent > 100:
        percent = 100
    print("文件下载:%.2f%%"% percent)

def downloadFile(url, dirPath):
    urllib.request.urlretrieve(url, dirPath, callbackfunc)
    pass

def downloadAndBuild(dict):
    fileName = dict["fileName"]
    url = dict["url"]
    cmakeArgs = None
    if "cmakeArgs" in dict:
        cmakeArgs = dict["cmakeArgs"]  
    if len(fileName) == 0:
        log("Download Was Error!")
        return  
    os.chdir(sourceDir) #进入到源代码存放的目录
    filePath = fileName + ".tar.gz"
    if not os.path.exists(filePath):
        log("Begin Download: " + fileName)
        downloadFile(url, filePath)
    if not os.path.exists(fileName):
        log("untar: " + fileName)
        untar(filePath, sourceDir)
    cmakeBuild(fileName, cmakeArgs, genBuilding=True, preCmdList=[], install=True)
    pass


def buildDeps(dict):
    fileName = dict["fileName"]
    url = dict["url"]
    cmakeArgs = None
    if "cmakeArgs" in dict:
        cmakeArgs = dict["cmakeArgs"]
    if len(fileName) == 0:
        log("Building Deps Was Error!")
        return
    log("-"*80)
    log("Start Building Deps: " + fileName)
    os.chdir(sourceDir) #进入到源代码存放的目录
    operator(url)
    cmakeBuild(fileName, cmakeArgs, genBuilding=True, preCmdList=[], install=True)
    pass


def genDepsCmakeList():
    log("-"*80)
    log("Generate Deps CmakeList in Path: " + homeDir)
    os.chdir(homeDir)

    libDir = os.path.join(outputDir, "lib")
    libs = getAllFileInDirectory(libDir)
    for lib in libs:
        if os.path.isdir(lib):
            continue
        sufix = os.path.splitext(lib)[-1]
        if sufix not in libSufixs:
            continue
        log("lib: "+ lib)
        libPath = os.path.join(libDir, lib)
        global cmakeOther
        cmakeOther = cmakeOther + "\n" + "link_libraries(\"" + libPath + "\")"

    depsCamke = "deps.cmake"
    depsContent = """
message("This is deps.cmake")

set(deps_list pthread dl)

set(DEPS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/"""+outputDirName+"""/include/")
set(DEPS_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/"""+outputDirName+"""/lib/")

message("Deps Include Directory: ${DEPS_INCLUDE_DIR}")
message("Deps Lib Directory: ${DEPS_LIB_DIR}")

include_directories("${DEPS_INCLUDE_DIR}")
link_directories("${DEPS_LIB_DIR}")
file(GLOB_RECURSE Deps_include ${DEPS_INCLUDE_DIR}**/*.h)

""" + cmakeOther
    log("Deps CmakeList content: " + depsContent)
    with open(depsCamke, "w") as fileHandler:
        fileHandler.write(depsContent)
    pass


if __name__ == '__main__':
    begin = datetime.datetime.now()
    log("更新时间：" + str(begin))

    if not os.path.exists(sourceDirName):
        os.makedirs(sourceDirName)
    if not os.path.exists(outputDirName):
        os.makedirs(outputDirName)
    
    homeDir = sys.path[0]
    log("Home Directory: " + homeDir)
    sourceDir = os.path.join(homeDir, sourceDirName)
    log("Deps Directory: " + sourceDir)
    outputDir = os.path.join(homeDir, outputDirName)
    log("Install Directory: " + outputDir)

    opensslDict = {
        "fileName": "libressl-3.6.1",
        "url": "https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.6.1.tar.gz",
    }
    # downloadAndBuild(opensslDict)

    sqliteCppDict = {
        "fileName": "SQLiteCpp",
        "url": "git clone -b 3.1.1 --depth=1 https://github.com/SRombauts/SQLiteCpp SQLiteCpp",
    }

    # buildDeps(sqliteCppDict)

    genDepsCmakeList()
    end = datetime.datetime.now()
    log(('花费时间: %.3f 秒' % (end - begin).seconds))
    logRecord()
    pass





