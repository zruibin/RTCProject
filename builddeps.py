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
import subprocess, shutil, json
import datetime
import tarfile, gzip
import urllib.request
from pathlib import Path


homeDir = ""
thirdPartyDir = ""
sourceDir = ""
outputDir = ""
thirdPartyDirName = "thirdParty"
sourceDirName = "depsSource"
outputDirName = "deps"
depsName = "deps.json"
buildDir = "buildGen" # cmake构建目录
cmakeOther = ""
libSufixs = [".a", ".lib", ".so", ".dylib", "dll"]

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

def json_minify(string, strip_space=True):
    """
    A port of the `JSON-minify` utility to the Python language.
    Based on JSON.minify.js: https://github.com/getify/JSON.minify
    """
    tokenizer = re.compile('"|(/\*)|(\*/)|(//)|\n|\r')
    end_slashes_re = re.compile(r'(\\)*$')

    in_string = False
    in_multi = False
    in_single = False

    new_str = []
    index = 0
    for match in re.finditer(tokenizer, string):
        if not (in_multi or in_single):
            tmp = string[index:match.start()]
            if not in_string and strip_space:
                # replace white space as defined in standard
                tmp = re.sub('[ \t\n\r]+', '', tmp)
            new_str.append(tmp)
        elif not strip_space:
            # Replace comments with white space so that the JSON parser reports
            # the correct column numbers on parsing errors.
            new_str.append(' ' * (match.start() - index))

        index = match.end()
        val = match.group()

        if val == '"' and not (in_multi or in_single):
            escaped = end_slashes_re.search(string, 0, match.start())

            # start of string or unescaped quote character to end string
            if not in_string or (escaped is None or len(escaped.group()) % 2 == 0):  # noqa
                in_string = not in_string
            index -= 1  # include " character in next catch
        elif not (in_string or in_multi or in_single):
            if val == '/*':
                in_multi = True
            elif val == '//':
                in_single = True
        elif val == '*/' and in_multi and not (in_string or in_single):
            in_multi = False
            if not strip_space:
                new_str.append(' ' * len(val))
        elif val in '\r\n' and not (in_multi or in_string) and in_single:
            in_single = False
        elif not ((in_multi or in_single) or (val in ' \r\n\t' and strip_space)):  # noqa
            new_str.append(val)

        if not strip_space:
            if val in '\r\n':
                new_str.append(val)
            elif in_multi or in_single:
                new_str.append(' ' * len(val))

    new_str.append(string[index:])
    return ''.join(new_str)



def cmakeBuild(fileName, cmakeArgs, genBuilding=True, preCmdList=[], install=True):
    os.chdir(fileName)
    if genBuilding:
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
                "-DCMAKE_BUILD_TYPE=RELEASE", 
                "-DCMAKE_INSTALL_PREFIX="+outputDir, 
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


def buildThirdParty():
    os.chdir(homeDir)
    os.chdir(thirdPartyDir) #进入到第三方存放的目录
    # runDir = Path(thirdPartyDir)
    # folders = runDir.iterdir()
    folders = os.listdir(thirdPartyDir)
    for folder in folders:
        if not os.path.exists(os.path.join(folder, "CMakeLists.txt")):
            continue
        # os.chdir(folder)
        log("folder: " + str(folder))
        fileName = str(folder)
        cmakeArgs = "-DCMAKE_CXX_STANDARD=14"
        cmakeBuild(fileName, cmakeArgs, genBuilding=True, preCmdList=[], install=True)
        os.chdir(thirdPartyDir) #回到第三方代码存放的目录
    pass


def buildFromDepsFile():
    os.chdir(homeDir)
    depsJson = None
    with open(depsName, 'r', encoding='utf-8') as fw:
        # json.dump(json_str, fw, indent=4, ensure_ascii=False)
        jsonString = json_minify(fw.read())
        depsJson = json.loads(jsonString)
    if depsJson is None: 
        return
    for depsDict in depsJson:
        log("depDict: " + str(depsDict))
        action = depsDict["action"]
        if action == "git": buildDeps(depsDict)
        if action == "gz": downloadAndBuild(depsDict)
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
        # log("lib: "+ lib)
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


def genDirs():
    if not os.path.exists(sourceDirName):
        os.makedirs(sourceDirName)
    if not os.path.exists(outputDirName):
        os.makedirs(outputDirName)
    if not os.path.exists(thirdPartyDirName):
        os.makedirs(thirdPartyDirName)
    
    global homeDir, sourceDir, thirdPartyDir, outputDir

    homeDir = sys.path[0]
    log("Home Directory: " + homeDir)
    sourceDir = os.path.join(homeDir, sourceDirName)
    log("Deps Directory: " + sourceDir)
    thirdPartyDir = os.path.join(homeDir, thirdPartyDirName)
    log("ThirdParty Directory: " + thirdPartyDir)
    outputDir = os.path.join(homeDir, outputDirName)
    log("Install Directory: " + outputDir)
    pass



if __name__ == '__main__':
    begin = datetime.datetime.now()
    log("更新时间：" + str(begin))

    # 生成目录
    genDirs()

    # 构建本地第三方库
    buildThirdParty()

    # 构建第三方库
    buildFromDepsFile()

    # 生成cmake文件
    genDepsCmakeList()

    end = datetime.datetime.now()
    log(('花费时间: %.3f 秒' % (end - begin).seconds))
    logRecord()
    pass





