#include <Inotify.h>


Inotify::Inotify() :
  mError(0),
  mEventTimeout(0),
  mLastEventTime(0),
  mEventMask(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE){
  Initialize();
}

Inotify::Inotify(std::vector<std::string> ignoredFolders) :
  mError(0),
  mEventTimeout(0),
  mLastEventTime(0),
  mEventMask(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE),
  mIgnoredFolders(ignoredFolders){

  Initialize();
}

Inotify::Inotify(std::string ignoredFolder) :
  mError(0),
  mEventTimeout(0),
  mLastEventTime(0),
  mEventMask(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE){

  Initialize();
  mIgnoredFolders.push_back(ignoredFolder);
}

  Inotify::Inotify(std::string ignoredFolder, int eventTimeout) :
  mError(0),
  mEventTimeout(eventTimeout),
  mLastEventTime(0),
  mEventMask(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE){

  Initialize();
  mIgnoredFolders.push_back(ignoredFolder);
}

Inotify::~Inotify(){
  CleanUp();
}

bool Inotify::Initialize(){
  // Try to initialise inotify
  if(!mIsInitialized){
    mInotifyFd = inotify_init();
    if(mInotifyFd == -1){
      mError = errno;
      dbg_printc(LOG_ERR, "Inotify", "Initialize", "Couldn´t initialize inotify, Errno: %d", mError);
      return false;
    }
    mIsInitialized = true;
  }
  dbg_printc(LOG_DBG, "Inotify", "Initialize", "Inotify is initialized now, fd: %d", mInotifyFd);
  return true;

}

bool Inotify::CleanUp(){
  assert(mIsInitialized);
  if(!close(mInotifyFd)){
    mError = errno;
    dbg_printc(LOG_ERR,"Inotify","CleanUp", "Can´t close inotify fd(%d), Errno: %d", mInotifyFd, mError);
    return false;
  }
  return true;
}

bool Inotify::WatchFolderRecursively(std::string watchFolder){
  assert(mIsInitialized);
  // Important : Initialize should be called in advance
  DIR * directory;
  std::string nextFile = "";
  struct dirent * ent;
  struct stat64 my_stat;

  directory = opendir(watchFolder.c_str());
  if(!IsDir(watchFolder))
    return WatchFile(watchFolder);


  if(watchFolder[watchFolder.size()-1] != '/'){
    watchFolder.append("/");
  }

  ent = readdir(directory);
  // Watch each directory within this directory
  while(ent){
    if((0 != strcmp(ent->d_name, ".")) && (0 != strcmp(ent->d_name, ".."))){
      nextFile = watchFolder;
      nextFile.append(ent->d_name);
      
      // Check the File/Folder for acces
      if(lstat64(nextFile.c_str(), &my_stat) == -1){
	mError = errno;
	dbg_printc(LOG_ERR, "Inotify", "WatchFolderRecursively", "\nC Error on fstat %s, %d", nextFile.c_str(), mError);
	if (mError != EACCES){
	  mError = errno;
	  closedir(directory);
	  return false;

	}

      }
      // Watch a folder recursively
      else if(S_ISDIR(my_stat.st_mode ) || S_ISLNK( my_stat.st_mode )) {
	nextFile.append("/");
	if(!IsIgnored(nextFile)){
	  bool status = WatchFolderRecursively(nextFile);
	  if (!status){
	    closedir(directory);
	    return false;

	  }

	}

      }

    }
    ent = readdir(directory);
    mError = 0;
  }
  closedir(directory);
  // Finally add watch to parentfolder
  return WatchFile(watchFolder);
}


bool Inotify::WatchFile(std::string file){
  assert(mIsInitialized);
  mError = 0;
  int wd;
  dbg_printc(LOG_DBG, "Inotify","WatchFile","Add watch of file: %s", file.c_str());
  wd = inotify_add_watch(mInotifyFd, file.c_str(), mEventMask);
  if(wd == -1){
    mError = errno;
    dbg_printc(LOG_WARN, "Inotify", "WatchFile", "Failed to watch %s, but keep on scanning, Errno: %d", file.c_str(), mError);
    return true;

  }
  mFolderMap[wd] =  file;
  return true;

}

bool Inotify::RemoveWatch(int wd){
  int error = inotify_rm_watch(mInotifyFd, wd);
  if(errno <= 0){
    dbg_printc(LOG_WARN, "Inotify","RemoveWatch","Failed to remove watch of wd %d, errno: %d", wd, error);
    return false;
  }
  dbg_printc(LOG_DBG, "Inotify","WatchFile","Removed watch with wd: %d, file: %s", wd, WdToFilename(wd).c_str());
  mFolderMap.erase(wd);
  return true;
}

std::string Inotify::WdToFilename(int wd){
  assert(mIsInitialized);
  return mFolderMap[wd];

}

bool Inotify::IsDir(std::string folder){
  DIR* directory;
  directory = opendir(folder.c_str());
  if(!directory) {
    mError = errno;
    if(mError == ENOTDIR){

    }
    else {
      dbg_printc(LOG_WARN, "Inotify","IsDir", "Couldn´t not opendir %s, Errno: %d", folder.c_str(), mError);


    }
    closedir(directory);
    return false;

  }
  closedir(directory);
  return true;

}

FileSystemEvent<int>*  Inotify::GetNextEvent(){
  assert(mIsInitialized);
  inotify_event *event;
  FileSystemEvent<int> *fileSystemEvent;
  int length = 0;
  int i = 0;
  char buffer[EVENT_BUF_LEN];
  time_t currentEventTime = time(NULL);
  bool eventOccured = false;

  // Read Events from fd
  while(mEventQueue.empty() || OnTimeout(currentEventTime)){
    ClearEventQueue();
    length = 0;
      while(length <= 0 || OnTimeout(currentEventTime)){
	dbg_printc(LOG_DBG, "Inotify", "GetNextEvent", "Read from inotify fd(%d)", mInotifyFd );
	length = read(mInotifyFd, buffer, EVENT_BUF_LEN);
	currentEventTime = time(NULL);
	if(length == -1){
	  mError = errno;
	  if(mError != EINTR){
	    dbg_printc(LOG_ERR,"Inotify", "GetNextEvent", "Failed to read from inotify fd(%d), Errno %d", mInotifyFd, mError);
	    return NULL;

	  }
	  dbg_printc(LOG_WARN, "Inotify", "GetNextEvent", "Can´t read from Inotify fd(%d), Errno: EINTR, try to read again", mInotifyFd );

	}

      }

    i = 0;
    while(i < length){
      event = (struct inotify_event *) &buffer[i];
      fileSystemEvent = new FileSystemEvent<int>(event->wd, event->mask, event->name, WdToFilename(event->wd)); 
      if(!OnTimeout(currentEventTime) && fileSystemEvent->GetMask() != IN_IGNORED){
	currentEventTime = time(NULL);
	mEventQueue.push(fileSystemEvent);
	dbg_printc(LOG_DBG, "Inotify", "GetNextEvent",
		   "Event from fd(%d) was triggered %s %d %s queue.length: %d", 
		   mInotifyFd,
		   fileSystemEvent->GetFilename().c_str(), 
		   fileSystemEvent->GetMask(),
		   fileSystemEvent->GetMaskString().c_str(),
		   mEventQueue.size());

	if(CheckEvent(fileSystemEvent)){
	  eventOccured = true; 
	  break; 
	}

      }
      i += EVENT_SIZE + event->len;

    }
    if(eventOccured == true){
      break;
    }
  }
  mLastEventTime = currentEventTime;
  fileSystemEvent = mEventQueue.front();
  mEventQueue.pop();
  return fileSystemEvent;

}

int Inotify::GetLastError(){
  return mError;

}

bool Inotify::IsIgnored(std::string file){
  if(!mIgnoredFolders[0].compare(""))
    return false;
  for(int i = 0; i < mIgnoredFolders.size(); ++i){
    size_t pos = file.find(mIgnoredFolders[i]);
    if(pos!= string::npos){
      dbg_printc(LOG_DBG, "Inotify", "IsIgnored","File will be ignored: %s", file.c_str());
      return true;
    }
  }
  return false;
}

void Inotify::ClearEventQueue(){
  int eventCount;
  for(eventCount = 0; eventCount < mEventQueue.size(); eventCount++){
    mEventQueue.pop();
  }

}

bool Inotify::OnTimeout(time_t eventTime){
  return (mLastEventTime + mEventTimeout) > eventTime;

}

bool Inotify::CheckEvent(FileSystemEvent<int>* event){
  // Events seems to have no syncfolder,
  // this can happen if not the full event
  // was read from inotify fd
  if(!event->GetWatchFolder().compare("")){
    return false;
  }

  return true;

}
