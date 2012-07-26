
#ifndef EventManager_H
#define EventManager_H

#include <vector>
#include <iostream>
#include <string>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>
#include <stdlib.h>
#include "RemoteSyncManager.h"

using namespace std;

/*
 * @brief Handels events from FileSystemScanner
 * @class EventManager
 *        EventManager.h 
 *        "include/EventManager.h"
 *
 * The EvenManager takes different events (inotify_event)
 * from an FileSystemScanner and handels them (HandleEvent).
 *
 *  struct inotify_event {
 *    int      wd;        Watch descriptor 
 *    uint32_t mask;      Mask of events 
 *    uint32_t cookie;    Unique cookie associating related
 *                        events (for rename(2)) 
 *    uint32_t len;       Size of name field 
 *    char     name[];    Optional null-terminated name 
 *  };
 * 
 */
class EventManager{
private:
  vector<inotify_event*> mEventList;
  SyncManager* mpSyncManager;

  void HandleEvent(inotify_event* pEvent, string sourceFolder);
public:
  EventManager(SyncManager * pSyncManager);
  void PushBackEvent(inotify_event* pNewEvent, string sourceFolder);
};

#endif /* EventManager_H */
