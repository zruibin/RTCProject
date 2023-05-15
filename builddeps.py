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


import os, re, json, sys, platform, fnmatch, stat
import subprocess, shutil, json
import datetime
import tarfile, gzip, zipfile, bz2
import urllib.request
from pathlib import Path
import multiprocessing
import inspect
from enum import Enum


IS_DEBUG = True

homeDir = ""
thirdPartyDir = ""
sourceDir = ""
outputDir = ""
thirdPartyDirName = "third_party"
sourceDirName = "depsSource"
outputDirName = "deps"
depsName = "deps.json"
depsCamke = "deps.cmake"
sourceLock = sourceDirName + ".lock"
buildDir = "buildGen" # cmake构建目录
cmakeOther = ""
libSufixs = [".a", ".lib", ".so", ".dylib", ".dll"]
depsSourceFlag = False #True if IS_DEBUG else False

depsSourceCamke = sourceDirName + ".cmake"
cmakeList = "CMakeLists.txt"
configure = "configure"
makefile = "Makefile"
ninjaBuild = "build.ninja"


CPU_COUNT = multiprocessing.cpu_count()
DEPS_ARCH = "DEPS_ARCH"


logList = []

def logRecord():
    with open(os.path.join(homeDir, "builddeps.log"), "a+") as fileHandle:
        for logStr in logList:
            fileHandle.write(str(logStr))
    logList.clear()
    pass

class Color(Enum):
    Black = 30
    Red = 31
    Green = 32
    Yellow = 33
    Blue = 34
    Fuchsia = 35 # 紫红色
    Cyan = 36 # 青蓝色
    White = 37

def log(string="", newline=True, color=None, write=True):
    if len(string) == 0:
        return

    function = inspect.stack()[1][3]
    line = inspect.stack()[1][2]
    string = "[" + str(function) + ":" + str(line) + "] " + string

    if color != None:
        # 见: https://www.cnblogs.com/ping-y/p/5897018.html
        string = "\033[" + str(color.value) + "m" + string + "\033[0m"
    if newline:
        if write: logList.append(str(string) + "\n")
        print(string, end="\n")
    else:
        if write: logList.append(str(string))
        print(string, end="")
    pass

def operator(cmdString, newline=True):
    log(cmdString)
    # output = os.popen(cmdString)
    # for line in output.readlines():
    #     log(line, newline)
    res = subprocess.Popen(cmdString, 
                            shell=True, 
                            stdout=subprocess.PIPE,
                            stdin=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    sout, serr = res.communicate() # res.stdout, res.stderr
    if serr:
        hasError = False
        datas = str(serr, "utf-8").split("\n")
        for data in datas: 
            log(data)
            if "error:" in data: hasError = True
        if hasError:
            errStr = "\033[" + str(31) + "m" + "stdout->Error." + "\033[0m"
            log(errStr, color=Color.Red)
            logRecord()
            raise Exception(errStr)
    if sout:
        hasError = False
        datas = str(sout, "utf-8").split("\n")
        for data in datas: 
            log(data)
            if "error:" in data: hasError = True
        if hasError:
            errStr = "\033[" + str(31) + "m" + "stdout->Error." + "\033[0m"
            log(errStr, color=Color.Red)
            logRecord()
            raise Exception(errStr)
    pass

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

def checkAndReplaceName(originName, destName):
    baseName = os.path.basename(destName)
    suffix = ".tar.gz"
    if suffix in baseName:
        baseName = baseName.replace(suffix, "")
    suffix = ".tar.bz2"
    if suffix in baseName:
        baseName = baseName.replace(suffix, "")
    suffix = ".zip"
    if suffix in baseName:
        baseName = baseName.replace(suffix, "")
    
    if originName != baseName:
        print(baseName)
        print(originName)
        os.rename(originName, baseName)
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
        originName = t.getnames()[0]
        checkAndReplaceName(originName, fname)
        return True
    except Exception as e:
        print(e)
        return False

def zipExtract(path_zip, path_aim):
    z = zipfile.ZipFile(path_zip, 'r')
    for p in z.namelist():
        z.extract(p, path_aim)
    z.close()
    depressName = os.path.split(z.namelist()[0])[0]
    desName = os.path.splitext(path_zip)[0]
    if depressName != desName:
        os.rename(depressName, desName)

def bz2Extract(path_bz2, path_aim):
    print("bz2Extract: " + path_bz2)
    archive = tarfile.open(path_bz2,'r:bz2')
    originName = archive.getnames()[0]
    # archive.debug = 1    # Display the files beeing decompressed.
    for tarinfo in archive:
        archive.extract(tarinfo, path_aim) 
    archive.close()
    checkAndReplaceName(originName, path_bz2)

def callbackfunc(blocknum, blocksize, totalsize):
    '''回调函数
    @blocknum: 已经下载的数据块
    @blocksize: 数据块的大小
    @totalsize: 远程文件的大小
    '''
    percent = 100.0 * blocknum * blocksize / totalsize
    if percent > 100:
        percent = 100
    print("文件下载:%.2f%%"% percent, end="\r")

def downloadFile(url, dirPath):
    try:
        urllib.request.urlretrieve(url, dirPath, callbackfunc)
    except Exception as e:
        log(str(e), color=Color.Red)
        logRecord()
        raise e
    pass

#------------------------------------------------------------------------------------

def swapDepsArgs(args):
    if args == None:
        return ""

    if "//" in args:
        args = args.replace("//", homeDir+os.path.sep)

    if DEPS_ARCH in args:
        args = args.replace(DEPS_ARCH, platform.machine())
    
    return args

#------------------------------------------------------------------------------------

def configBuild(fileName, configArgs, debugArgs, targetDir=None, genBuilding=True, install=True):
    os.chdir(fileName)
    inode = "."

    if targetDir != None:
        os.chdir(targetDir)

    if genBuilding:
        if os.path.exists(buildDir):
            shutil.rmtree(buildDir)
        os.makedirs(buildDir)
        os.chdir(buildDir)
        inode = ".."

    log("-"*80)
    log("当前编译路径：" + os.getcwd())
    log("configBuild: " + fileName)

    if IS_DEBUG and debugArgs != None:
        configArgs = configArgs + " " + debugArgs

    configArgs = swapDepsArgs(configArgs)

    configureFile = os.path.join(inode, "configure")
    os.chmod(configureFile, stat.S_IEXEC|stat.S_IRWXU|stat.S_IRWXG|stat.S_IRWXO)
    cmdStr = configureFile + " --prefix=" + outputDir + " " + configArgs
    makeStr = "make -j" + str(CPU_COUNT)
    makeInstall = "make install"

    if platform.machine() == "arm64" and platform.system() == "Darwin":
        cmdStr = "arch -arm64 " + cmdStr
        makeStr = "arch -arm64 " + makeStr
        makeInstall = "arch -arm64 " + makeInstall

    operator(cmdStr, False)
    operator(makeStr, False)
    operator(makeInstall, False)
    pass

def cmakeBuild(fileName, cmakeArgs, debugArgs, targetDir, genBuilding=True, preCmdList=[], install=True):
    os.chdir(fileName)
    inode = "."

    if targetDir != None:
        log("切换路径：" + str(targetDir))
        os.chdir(targetDir)

    if genBuilding:
        if os.path.exists(buildDir):
            shutil.rmtree(buildDir)
        os.makedirs(buildDir)
        os.chdir(buildDir)
        inode = ".."

    log("-"*80)
    log("当前编译路径：" + os.getcwd())
    if len(preCmdList) > 0:
        operatorCMD(preCmdList, False)

    if cmakeArgs == None:
        cmakeArgs = ""

    if IS_DEBUG:
        if debugArgs != None: # debug
            cmakeArgs = cmakeArgs + debugArgs
        cmakeArgs = cmakeArgs + " -DCMAKE_BUILD_TYPE=DEBUG "
    else:
        cmakeArgs = cmakeArgs + " -DCMAKE_BUILD_TYPE=RELEASE "

    cmakeArgs = swapDepsArgs(cmakeArgs)

    otherCmakeArgs = ""
    outputBinDir = os.path.join(outputDir, "bin")
    if os.path.exists(outputBinDir):
        exeFiles = os.listdir(outputBinDir)
        for exeFile in exeFiles:
            if fnmatch.fnmatch(exeFile, "ninja*"):
                otherCmakeArgs = otherCmakeArgs + "-GNinja -DCMAKE_MAKE_PROGRAM="
                otherCmakeArgs = otherCmakeArgs + os.path.join(outputBinDir, exeFile) + " "
                break

    operatePrefix = ""
    osName = platform.system()
    if(osName == 'Windows'):
        log("Warning Windows.")
    elif(osName == 'Linux'):
        log("Warning Linux.")
    elif(osName == 'Darwin'):
        if platform.machine() == "arm64":
            operatePrefix = "arch -arm64"
            otherCmakeArgs = otherCmakeArgs + "-DCMAKE_OSX_ARCHITECTURES=arm64 "
            otherCmakeArgs = otherCmakeArgs + "-DCMAKE_HOST_SYSTEM_PROCESSOR=arm64 "
            # otherCmakeArgs = otherCmakeArgs + "-DCMAKE_SYSTEM_PROCESSOR=arm64 "
            # otherCmakeArgs = otherCmakeArgs + "-DCMAKE_HOST_SYSTEM_PROCESSOR=arm64 "

    cmdList = [operatePrefix, "cmake",
                cmakeArgs,
                otherCmakeArgs,
                "-DCMAKE_INSTALL_PREFIX="+outputDir, 
                inode,
                ]
    operatorCMD(cmdList, False)
    if install:
        operator("cmake --build . --target install", False)
    pass

#------------------------------------------------------------------------------------

def getDepsJson():
    depsJson = None
    with open(depsName, 'r', encoding='utf-8') as fw:
        # json.dump(json_str, fw, indent=4, ensure_ascii=False)
        jsonString = json_minify(fw.read())
        depsJson = json.loads(jsonString)
    return depsJson

def isDesps(fileName):
    lockFile = os.path.join(homeDir, sourceLock)
    if not os.path.exists(lockFile):
        return False
    with open(lockFile, "r") as f:
        for line in f:
            line = line.replace(os.linesep, "")
            if line == fileName:
                return True
            else:
                continue
    return False

def updateDesps(fileName):
    lockFile = os.path.join(homeDir, sourceLock)
    with open(lockFile, "a") as f:
        f.writelines(fileName + os.linesep)
    pass

def getDictValues(depsDict):
    fileName = depsDict["fileName"]
    action = depsDict["action"]
    url = depsDict["url"]

    # 默认跟随debug，debug需要在源目录生成才有库符号链接
    buildDir = False if IS_DEBUG else True
    if "build_dir" in depsDict and IS_DEBUG:
        buildDir = depsDict["build_dir"]

    buildAction = "cmake"
    if "build" in depsDict:
        buildAction = depsDict["build"]

    args = None
    if "args" in depsDict:
        args = depsDict["args"]

    debugArgs = None
    if "debug_args" in depsDict:
        debugArgs = depsDict["debug_args"]

    targetDir = None
    if "target_dir" in depsDict:
        targetDir = depsDict["target_dir"]

    return {
        "fileName": fileName,
        "action": action,
        "url": url,
        "buildAction": buildAction,
        "args": args,
        "debugArgs": debugArgs,
        "targetDir": targetDir,
        "buildDir": buildDir
    }

def downloadAndBuild(depsDict):
    depsDict = getDictValues(depsDict)
    fileName = depsDict["fileName"]
    action = depsDict["action"]
    url = depsDict["url"]
    buildAction = depsDict["buildAction"]
    args = depsDict["args"]
    debugArgs = depsDict["debugArgs"]
    targetDir = depsDict["targetDir"]
    buildDir = depsDict["buildDir"]

    if action == "gz": action = "tar.gz"
    if action == "bz2": action = "tar.bz2" 

    if len(fileName) == 0:
        log("Download Was Error!")
        return

    os.chdir(sourceDir) # 进入到源代码存放的目录

    filePath = fileName + "." + action
    if not os.path.exists(filePath):
        log("Begin Download: " + fileName)
        downloadFile(url, filePath)
    if not os.path.exists(fileName):
        log("解压: " + filePath)
        fileType = os.path.splitext(filePath)[-1]
        if fileType == ".gz": untar(filePath, sourceDir)
        if fileType == ".zip": zipExtract(filePath, sourceDir)
        if fileType == ".bz2": bz2Extract(filePath, sourceDir)

    if buildAction == "config":
        configBuild(fileName, args, debugArgs, targetDir, buildDir)
    else:
        cmakeBuild(fileName, args, debugArgs, targetDir, buildDir)
    pass

def buildDeps(depsDict):
    depsDict = getDictValues(depsDict)
    fileName = depsDict["fileName"]
    action = depsDict["action"]
    url = depsDict["url"]
    buildAction = depsDict["buildAction"]
    args = depsDict["args"]
    debugArgs = depsDict["debugArgs"]
    targetDir = depsDict["targetDir"]
    buildDir = depsDict["buildDir"]

    # log(str(depsDict))

    if len(fileName) == 0:
        log("Building Deps Was Error!")
        return
    log("-"*80)
    log("Start Building Deps: " + fileName)
    os.chdir(sourceDir) #进入到源代码存放的目录
    operator(url)

    if buildAction == "config":
        configBuild(fileName, args, debugArgs, targetDir, buildDir)
    else:
        cmakeBuild(fileName, args, debugArgs, targetDir, buildDir)
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
        log("-"*80)
        log("folder: " + str(folder))
        fileName = str(folder)
        cmakeArgs = "-DCMAKE_CXX_STANDARD=14"
        cmakeBuild(fileName, cmakeArgs, None, None)
        os.chdir(thirdPartyDir) #回到第三方代码存放的目录
    pass


def buildFromDepsFile():
    os.chdir(homeDir)
    if not os.path.exists(depsName):
        log("Error: " + str(depsName) + " was not exist.")
        return

    depsJson = getDepsJson()
    if depsJson is None: 
        return
    for depsDict in depsJson:
        # log("depDict: " + str(depsDict))
        action = depsDict["action"]
        fileName = depsDict["fileName"]

        if isDesps(fileName): # 已经下载安装好则跳过
            continue

        if action == "git": buildDeps(depsDict)
        if action == "gz": downloadAndBuild(depsDict)
        if action == "zip": downloadAndBuild(depsDict)
        if action == "bz2": downloadAndBuild(depsDict)

        updateDesps(fileName)
    pass

#------------------------------------------------------------------------------------

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
        # cmakeOther = cmakeOther + "\n" + "link_libraries(\"" + libPath + "\")"
        cmakeOther = cmakeOther + "\n" + "list(APPEND DEPS_LIBS \"" + libPath + "\")"

    depsInclude = """
list(APPEND HEADERS ${Deps_Include})
"""

    depsSource = """
file(GLOB_RECURSE Deps_Source
    "depsSource/**/*.c"
    "depsSource/**/*.cc"
    "depsSource/**/*.h"
    "depsSource/**/*.hpp"
    "depsSource/**/*.h++"
    "depsSource/**/*.asm"
)
sourceGroup("" ${Deps_Source})
#set_source_files_properties(${Deps_Source} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${Deps_Include} ${Deps_Source})
add_library(Deps STATIC ${Deps_Source})
"""
    if depsSourceFlag:
        depsInclude = ""
    else:
        depsSource = ""

    depsContent = """
message("This is deps.cmake")

set(deps_list pthread dl)

set(DEPS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/"""+outputDirName+"""/include/")
set(DEPS_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/"""+outputDirName+"""/lib/")

message("Deps Include Directory: ${DEPS_INCLUDE_DIR}")
message("Deps Lib Directory: ${DEPS_LIB_DIR}")

include_directories("${DEPS_INCLUDE_DIR}")
link_directories("${DEPS_LIB_DIR}")
file(GLOB_RECURSE Deps_Include ${DEPS_INCLUDE_DIR}**)

""" + depsInclude + depsSource + cmakeOther
    log("Deps CmakeList content: " + depsContent)
    with open(depsCamke, "w") as fileHandler:
        fileHandler.write(depsContent)
    pass

def addDebugDepsCmake(destDir, name):
    depCmakeList = os.path.join(destDir, cmakeList)
    depConfigure = os.path.join(destDir, configure)
    depMakefile = os.path.join(destDir, makefile)
    depNinjaBuild = os.path.join(destDir, ninjaBuild)
    log("cmakeList: " + depCmakeList, color=Color.Yellow)
    log("configure: " + depConfigure, color=Color.Yellow)
    log("makefile: " + depMakefile, color=Color.Yellow)

    cmakeBuild = False
    hasBuildDir = False
    if os.path.exists(depCmakeList):
        cmakeBuild = True
        if not os.path.exists(depNinjaBuild):
            if not os.path.exists(os.path.join(destDir, buildDir)):
                log(os.path.basename(depCmakeList) + " was exist! remove it and try again!", color=Color.Red)
                return
            else:
                hasBuildDir = True

    if not cmakeBuild and not os.path.exists(depConfigure):
        log(os.path.basename(depConfigure) + " was not exist!", color=Color.Red)
        return

    if not cmakeBuild and not os.path.exists(depMakefile):
        log(os.path.basename(depMakefile) + " was not exist!", color=Color.Red)
        return

    content = r"""
message(WARNING "This Target(PROJECT_NAME) Create By Ruibin.Chow.")

set(TARGET_ERROR "PROJECT_NAME_error")
set(TARGET_BUILD "PROJECT_NAME_building")


file(GLOB_RECURSE PROJECT_NAME_Source
    "/source_path/**/*.c"
    "/source_path/**/*.cc"
    "/source_path/**/*.cpp"
    "/source_path/**/*.h"
    "/source_path/**/*.hpp"
    "/source_path/**/*.h++"
    "/source_path/**/*.m"
    "/source_path/**/*.mm"
    "/source_path/**/*.asm"
    "/source_path/**/*.S"
    "/source_path/*.c"
    "/source_path/*.cc"
    "/source_path/*.cpp"
    "/source_path/*.h"
    "/source_path/*.hpp"
    "/source_path/*.h++"
    "/source_path/*.asm"
    "/source_path/*.S"
    "/source_path/*.cmake"
)
sourceGroup("" ${PROJECT_NAME_Source})
#message("sources: ${PROJECT_NAME_Source}")

add_library(${TARGET_ERROR} STATIC ${PROJECT_NAME_Source})

set(command_string [[

import subprocess, os

depsDir = "/path/deps"
PATH = depsDir
PATH = PATH + ":" + os.path.join(depsDir, "bin")
PATH = PATH + ":" + os.path.join(depsDir, "include")
PATH = PATH + ":" + os.path.join(depsDir, "lib")
PATH = PATH + ":" + "/system_path"
os.environ["PATH"] = os.getenv("PATH") + ":" + PATH

result = subprocess.getstatusoutput("ARCH RUN")
print("action: " + "ARCH RUN")
action = int(result[0])
if action == 0:
    msg = str(result[1])
    print(msg)
    if "HINT" not in msg:
        print("action: " + "ARCH INSTALL")
        result = subprocess.getstatusoutput("ARCH INSTALL")
        if len(result) > 1:
            print("install error: " + str(result[1]))
        exit(int(result[0]))
else:
    print("error: " + str(action))
    exit(action)

]])

include(FindPythonInterp)
add_custom_target(${TARGET_BUILD}
    VERBATIM
    COMMAND ${PYTHON_EXECUTABLE} -c "${command_string}"
    COMMAND echo "${TARGET_BUILD} done."
    WORKING_DIRECTORY /source_path
)

"""
    content = content.replace("PROJECT_NAME", name)
    content = content.replace("/path", homeDir)
    content = content.replace("/source_path", destDir)
    content = content.replace("/system_path", os.getenv("PATH"))

    arch = ""
    if platform.machine() == "arm64" and platform.system() == "Darwin":
        arch = "arch -arm64"

    directory = buildDir if hasBuildDir else "."
    hint = "no work to do" if cmakeBuild else "Nothing to be done"
    make = "cmake --build " + directory if cmakeBuild else "make"
    install = "cmake --install " + directory if cmakeBuild else "make install"

    content = content.replace("ARCH", arch)
    content = content.replace("HINT", hint)
    content = content.replace("RUN", make)
    content = content.replace("INSTALL", install) 
    
    log(content, write=False)

    cmakeName = name + ".cmake"
    outputCmakeFile = os.path.join(destDir, cmakeName)

    with open(outputCmakeFile, "w") as fileHandle:
        fileHandle.write(str(content))

    genDebugDepsCmake()
    pass

def removeDebugDepsCmake(destDir, name):
    cmakeName = name + ".cmake"
    debugCmakeFile = os.path.join(destDir, cmakeName)
    if os.path.exists(debugCmakeFile):
        os.remove(debugCmakeFile)

    genDebugDepsCmake()
    pass

def genDebugDepsCmake():
    depsJson = getDepsJson()
    if depsJson is None: 
        return

    depsData = ""
    for depsDict in depsJson:
        depsDict = getDictValues(depsDict)
        targetDir = depsDict["targetDir"]
        name = depsDict["fileName"]
        file = name + ".cmake"

        debugCmakePath = os.path.join(sourceDir, name)
        if targetDir != None:
            name = name + "_" + targetDir
            file = name + ".cmake"
            debugCmakePath = os.path.join(debugCmakePath, targetDir)

        debugCmakeFile = os.path.join(debugCmakePath, file)
        if os.path.exists(debugCmakeFile):
            log("debugCmakeFile: " + debugCmakeFile)
            cmakeData = """
include("%s")
list(APPEND Deps_Source_Targets %s_building)
    """  % (debugCmakeFile, name)
            depsData = depsData + cmakeData

    if len(depsData) > 0:
        log("depsSourceCamke: " + depsSourceCamke, color=Color.Yellow)
        with open(depsSourceCamke, "w") as fileHandle:
            fileHandle.write(str(depsData))
    pass

#------------------------------------------------------------------------------------

def genDirs():
    if not os.path.exists(sourceDirName):
        os.makedirs(sourceDirName)
    if not os.path.exists(outputDirName):
        os.makedirs(outputDirName)
    if not os.path.exists(thirdPartyDirName):
        os.makedirs(thirdPartyDirName)

    log("Home Directory: " + homeDir)
    log("Deps Directory: " + sourceDir)
    log("ThirdParty Directory: " + thirdPartyDir)
    log("Install Directory: " + outputDir)
    # log("-"*80)

    PATH = outputDir
    PATH = PATH + ":" + os.path.join(outputDir, "bin")
    PATH = PATH + ":" + os.path.join(outputDir, "include")
    PATH = PATH + ":" + os.path.join(outputDir, "lib")
    log("PATH:" + PATH)
    os.environ["PATH"] = os.getenv("PATH") + ":" + PATH
    os.environ["PKG_CONFIG_PATH"] = os.path.join(outputDir, "bin", "pkg-config")
    operator("echo $PATH")
    operator("echo $PKG_CONFIG_PATH")
    operator("which nasm")
    pass

#------------------------------------------------------------------------------------

def deps():
    # 生成目录
    genDirs()

    # 构建第三方库
    buildFromDepsFile()

    # 构建本地第三方库
    buildThirdParty()
    
    # 生成cmake文件
    genDepsCmakeList()
    pass

def debugDepsCmake():
    path = None
    action = None
    if len(sys.argv) > 2:
        action = sys.argv[1]
        path = sys.argv[2]
    else:
        log("\033[5;31m" + "Error: It was need path!" + "\033[0m")
        return

    destDir = os.path.join(homeDir, path)
    depsSourceDir = os.path.join(homeDir, "depsSource")
    name = destDir.replace(depsSourceDir, "").replace("/", "_")
    if name.startswith("_"):
        name = name[1:]
    if name.endswith("_"):
        name = name[:-1]
    print(name)

    if action == "add" or action == "-a":
        addDebugDepsCmake(destDir, name)
    elif action == "remove" or action == "-r":
        removeDebugDepsCmake(destDir, name)
    pass

def help():
    helpStr = """
Command:
    deps           根据deps.json安装依赖
    add dep_dir    根据库dep_dir添加依赖调试
    remove dep_dir 根据库dep_dir删除依赖调试
    help           说明
Default:
    deps"""
    log(helpStr)
    pass

def init():
    global homeDir, sourceDir, thirdPartyDir, outputDir
    homeDir = sys.path[0]
    sourceDir = os.path.join(homeDir, sourceDirName)
    thirdPartyDir = os.path.join(homeDir, thirdPartyDirName)
    outputDir = os.path.join(homeDir, outputDirName)

    global depsSourceCamke
    depsSourceCamke = os.path.join(homeDir, depsSourceCamke)
    pass


#------------------------------------------------------------------------------------


if __name__ == '__main__':
    log("-"*80)
    begin = datetime.datetime.now()
    log("开始时间：" + str(begin))

    init()

    if len(sys.argv) == 1:
        deps()
    if len(sys.argv) == 2:
        if sys.argv[1] == "help" or sys.argv[1] == "-h":
            help()
        elif sys.argv[1] == "deps":
            deps()
    elif len(sys.argv) > 2:
        debugDepsCmake()

    end = datetime.datetime.now()
    log(('花费时间: %.3f 秒' % (end - begin).seconds))
    log("-"*80)
    logRecord()
    pass





