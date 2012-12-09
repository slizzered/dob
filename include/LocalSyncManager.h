#ifndef LocalSyncManager_H
#define LocalSyncManager_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dbg_print.h>

#include "SyncManager.h"


class LocalSyncManager : public SyncManager {

public:
  LocalSyncManager(string destFolder, string syncType);
  //virtual bool SyncSourceFolder(string sourceFolder);
  //virtual bool SyncFolder(string sourceFolder, string syncFolder, string folder);
  //virtual bool SyncFile(string sourceFolder, string syncFolder);
  //virtual bool RemoveFolder(string sourceFolder, string syncFolder, string folder);

 protected: 
  virtual bool CheckDestFolder();
  virtual bool MountDestFolder();

};


#endif /* LocalSyncManager_H */
