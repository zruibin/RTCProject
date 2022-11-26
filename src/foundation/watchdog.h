/*
 * watchdog.h
 *
 * Created by Ruibin.Chow on 2022/11/26.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <map>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <filesystem>
#include "foundation/foundation_define.h"

/*
 
 Note:
    The API is very small an only expose those 4 functions to watch,
    unwatch or touch files or directories:
    
    Watchdog::Watch(const fs::path &path, const std::function<void(const fs::path&)> &callback);
    Watchdog::Unwatch( const fs::path &path );
    Watchdog::UnwatchAll();
    Watchdog::Touch();
    
 Example:
    
    You can use a wildcard character to only watch for the desired files in a directory :
 
     Watchdog::Watch("images/images.*", [](const fs::path &path) {
         // do something
     });

    If you need to be able to iterate through a watched directory you
    can change the callback signature to this :
 
     Watchdog::Watch("images/*.png", [](const vector<fs::path> &paths) {
         for(auto p : paths) {
             cout << p << " has changed" << endl;
         }
     });
 
    In the context of cinder both absolute path and path relative to
    the asset folder are accepted.

    There's is also a method to update the last write time of a file or
    directory which is usefull if you want to force the update of some files:

     Watchdog::Watch("shaders/include/*", [](const fs::path &path) {
         // this will trigger any watched asset callback in "shaders"
         Watchdog::Touch("shaders");
     });
     Watchdog::Watch("shaders/lighting.*", [](const fs::path &path) {
         // triggered on changes any from "lighting.*" and "include/*"
     });
     Watchdog::Watch("shaders/wireframe.*", [](const fs::path &path) {
         // triggered on changes any from "wireframe.*" and "include/*"
     });
 */

namespace foundation {

#if HAS_CPP_17

namespace fs = std::filesystem;

class WatchedFileSystemException : public std::exception {
    
public:
    WatchedFileSystemException(const fs::path &path) {
        sprintf(mMessage, "Failed to find file or directory at: %s", path.c_str());
    }
    
    virtual const char* what() const throw() { return mMessage; }
    
    char mMessage[4096];
};

class Watchdog {
    
public:
    static void Watch(const fs::path &path,
                      const std::function<void(const fs::path&)> &callback) {
        WatchImpl(path, callback, std::function<void(const std::vector<fs::path>&)>());
    }
    
    static void Watch(const fs::path &path,
                      const std::function<void(const std::vector<fs::path>&)> &callback) {
        WatchImpl(path, std::function<void(const fs::path&)>(), callback);
    }
    
    static void Unwatch(const fs::path &path) {
        WatchImpl(path);
    }
    
    static void UnwatchAll() {
        WatchImpl(fs::path());
    }
    
    static void Touch(const fs::path &path,
                      std::filesystem::file_time_type time) {
        // if the file or directory exists change its last write time
        if (fs::exists(path)) {
            fs::last_write_time(path, time);
            return;
        }
        // if not, visit each path if there's a wildcard
        if (path.string().find("*") != std::string::npos) {
            VisitWildCardPath(path, [time](const fs::path &p) {
                fs::last_write_time(p, time);
                return false;
            });
        } else {
            throw WatchedFileSystemException(path);
        }
    }
    
protected:
    
    Watchdog() : mWatching(false) {}
    
    void Close() {
        // remove all watchers
        UnwatchAll();
        
        // stop the thread
        mWatching = false;
        if (mThread->joinable()) mThread->join();
    }
    
    void Start() {
        mWatching = true;
        mThread = std::unique_ptr<std::thread>(new std::thread([this]() {
            // keep watching for modifications every ms milliseconds
            auto ms = std::chrono::milliseconds(500);
            while (mWatching) {
                do {
                    // iterate through each watcher and check for modification
                    std::lock_guard<std::mutex> lock(mMutex);
                    auto end = mFileWatchers.end();
                    for(auto it = mFileWatchers.begin(); it != end; ++it) {
                        it->second.Watch();
                    }
                    // lock will be released before this thread goes to sleep
                } while(false);
                
                // make this thread sleep for a while
                std::this_thread::sleep_for(ms);
            }
        }));
    }
    
    static void WatchImpl(const fs::path &path,
                          const std::function<void(const fs::path&)> &callback = std::function<void(const fs::path&)>(),
                          const std::function<void(const std::vector<fs::path>&)> &listCallback = std::function<void(const std::vector<fs::path>&)>()) {
        // create the static Watchdog instance
        static Watchdog wd;
        // and start its thread
        if (!wd.mWatching) {
            wd.Start();
        }
        
        const std::string key = path.string();
        
        // add a new watcher
        if (callback || listCallback) {
            std::string filter;
            fs::path p = path;
            // try to see if there's a match for the wildcard
            if (path.string().find("*") != std::string::npos) {
                bool found = false;
                std::pair<fs::path,std::string> pathFilter = VisitWildCardPath(path, [&found](const fs::path &p) {
                    found = true;
                    return true;
                });
                if (!found) {
                    throw WatchedFileSystemException(path);
                } else {
                    p       = pathFilter.first;
                    filter  = pathFilter.second;
                }
            }
            
            std::lock_guard<std::mutex> lock(wd.mMutex);
            if (wd.mFileWatchers.find(key) == wd.mFileWatchers.end()) {
                wd.mFileWatchers.emplace(make_pair(key, Watcher(p, filter, callback, listCallback)));
            }
        }
        // if there is no callback that means that we are unwatching
        else {
            // if the path is empty we unwatch all files
            if (path.empty()) {
                std::lock_guard<std::mutex> lock(wd.mMutex);
                for(auto it = wd.mFileWatchers.begin(); it != wd.mFileWatchers.end();) {
                    it = wd.mFileWatchers.erase(it);
                }
            }
            // or the specified file or directory
            else {
                std::lock_guard<std::mutex> lock(wd.mMutex);
                auto watcher = wd.mFileWatchers.find(key);
                if (watcher != wd.mFileWatchers.end()) {
                    wd.mFileWatchers.erase(watcher);
                }
            }
        }
    }
    
    static std::pair<fs::path,std::string> GetPathFilterPair(const fs::path &path) {
        // extract wildcard and parent path
        std::string key     = path.string();
        fs::path p          = path;
        size_t wildCardPos  = key.find("*");
        std::string filter;
        if (wildCardPos != std::string::npos) {
            filter  = path.filename().string();
            p       = path.parent_path();
        }
        
        // throw an exception if the file doesn't exist
        if (filter.empty() && !fs::exists(p)) {
            throw WatchedFileSystemException(path);
        }
        
        return std::make_pair(p, filter);
    }
    
    static std::pair<fs::path, std::string> VisitWildCardPath(const fs::path &path,
                                                              const std::function<bool(const fs::path&)> &visitor) {
        std::pair<fs::path, std::string> pathFilter = GetPathFilterPair(path);
        if (!pathFilter.second.empty()) {
            std::string full    = (pathFilter.first / pathFilter.second).string();
            size_t wildcardPos  = full.find("*");
            std::string before  = full.substr(0, wildcardPos);
            std::string after   = full.substr(wildcardPos + 1);
            fs::directory_iterator end;
            for(fs::directory_iterator it(pathFilter.first); it != end; ++it) {
                std::string current = it->path().string();
                size_t beforePos    = current.find(before);
                size_t afterPos     = current.find(after);
                if ((beforePos != std::string::npos || before.empty())
                    && (afterPos != std::string::npos || after.empty())) {
                    if (visitor(it->path())) {
                        break;
                    }
                }
            }
        }
        return pathFilter;
    }
    
    class Watcher {
    public:
        Watcher(const fs::path &path,
                const std::string &filter,
                const std::function<void(const fs::path&)> &callback,
                const std::function<void(const std::vector<fs::path>&)> &listCallback)
        : mPath(path), mFilter(filter), mCallback(callback), mListCallback(listCallback) {
            // make sure we store all initial write time
            if (!mFilter.empty()) {
                std::vector<fs::path> paths;
                VisitWildCardPath(path / filter, [this,&paths](const fs::path &p) {
                    HasChanged(p);
                    paths.push_back(p);
                    return false;
                });
                // this means that the first watch won't call the callback function
                // so we have to manually call it here
                if (mCallback) {
                    mCallback(mPath / mFilter);
                } else {
                    mListCallback(paths);
                }
            }
        }
        
        void Watch() {
            // if there's no filter we just check for one item
            if (mFilter.empty() && HasChanged(mPath) && mCallback) {
                mCallback(mPath);
            }
            // otherwise we check the whole parent directory
            else if (!mFilter.empty()) {
                std::vector<fs::path> paths;
                VisitWildCardPath(mPath / mFilter, [this,&paths](const fs::path &p) {
                    bool pathHasChanged = HasChanged(p);
                    if (pathHasChanged && mCallback) {
                        mCallback(mPath / mFilter);
                        //#error TODO: still have to figure out an elegant way to do this without cinder
                        return true;
                    }
                    else if (pathHasChanged && mListCallback) {
                        paths.push_back(p);
                    }
                    return false;
                });
                if (paths.size() && mListCallback) {
                    mListCallback(paths);
                }
            }
        }
        
        bool HasChanged(const fs::path &path) {
            // get the last modification time
            auto time = fs::last_write_time(path);
            // add a new modification time to the map
            std::string key = path.string();
            if (mModificationTimes.find(key) == mModificationTimes.end()) {
                mModificationTimes[key] = time;
                return true;
            }
            // or compare with an older one
            auto &prev = mModificationTimes[key];
            if (prev < time) {
                prev = time;
                return true;
            }
            return false;
        };
        
    protected:
        fs::path                                            mPath;
        std::string                                         mFilter;
        std::function<void(const fs::path&)>                mCallback;
        std::function<void(const std::vector<fs::path>&)>   mListCallback;
        std::map<std::string, fs::file_time_type>           mModificationTimes;
    };
    
    std::mutex                      mMutex;
    std::atomic<bool>               mWatching;
    std::unique_ptr<std::thread>    mThread;
    std::map<std::string, Watcher>  mFileWatchers;
};

#endif

}

#endif /* !WATCHDOG_H */
