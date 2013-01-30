/**
 * @file      FileSystemEvent.h
 * @author    Erik Zenker
 * @date      01.11.2012
 * @copyright Gnu Public License
 **/

#ifndef FileSystemEvent_H
#define FileSystemEvent_H

#include <string>
#include <unistd.h>
#include <stdint.h>


/**
 * @brief Container for events on filesystem.
 * @class FileSystemEvent
 *        FileSystemEvent.h
 *        "include/FileSystemEvent.h"
 *
 * The intention for FileSystemEvent was to
 * create some common interface for different
 * filesystem scan methods. For example you
 * could poll your filesystem every 10 seconds
 * to seek for changes or use inotify which detects 
 * events by it self.
 *
 **/
template <class T>
class FileSystemEvent{
 public:
  FileSystemEvent(T id, uint32_t mask, std::string filename, std::string watchFolder);
  ~FileSystemEvent();
  uint32_t getMask();
  std::string getMaskString();
  std::string getFilename();
  T getId();
  std::string getWatchFolder();
  std::string getFullPath();
  std::string getFolderPath();

 private:
  std::string maskToString(uint32_t events);

  // Member
  T mId;
  uint32_t mMask;
  std::string mFilename;
  std::string mWatchFolder;

};

template <class T>
FileSystemEvent<T>::FileSystemEvent(T id, uint32_t mask, std::string filename, std::string watchFolder) :
  mId(id),
  mMask(mask),
  mFilename(filename),
  mWatchFolder(watchFolder){

}

template <class T>
FileSystemEvent<T>::~FileSystemEvent(){

}

template <class T>
uint32_t FileSystemEvent<T>::getMask(){
  return mMask;

}

template <class T>
std::string FileSystemEvent<T>::getFilename(){
  return mFilename;

}

template <class T>
T FileSystemEvent<T>::getId(){
  return mId;

}

template <class T>
std::string FileSystemEvent<T>::getWatchFolder(){
  return mWatchFolder;

}

template <class T>
std::string FileSystemEvent<T>::getMaskString(){
  return maskToString(mMask);
}

template <class T>
std::string FileSystemEvent<T>::getFullPath(){
  return mWatchFolder.append(mFilename);
}

template <class T>
std::string FileSystemEvent<T>::getFolderPath(){
  return mWatchFolder;
}

template <class T>
std::string FileSystemEvent<T>::maskToString(uint32_t events){
    std::string eventString = "";

  if(IN_ACCESS & events)
    eventString.append("IN_ACCESS ");
  if(IN_ATTRIB & events)
    eventString.append("IN_ATTRIB ");
  if(IN_CLOSE_WRITE & events)
    eventString.append("IN_CLOSE_WRITE ");
  if(IN_CLOSE_NOWRITE & events)
    eventString.append("IN_CLOSE_NOWRITE ");
  if(IN_CREATE & events)
    eventString.append("IN_CREATE ");
  if(IN_DELETE & events)
    eventString.append("IN_DELETE ");
  if(IN_DELETE_SELF & events)
    eventString.append("IN_DELETE_SELF ");
  if(IN_MODIFY & events)
    eventString.append("IN_MODIFY ");
  if(IN_MOVE_SELF & events)
    eventString.append("IN_MOVE_SELF ");
  if(IN_MOVED_FROM & events)
    eventString.append("IN_MOVED_FROM ");
  if(IN_MOVED_TO & events)
    eventString.append("IN_MOVED_TO ");
  if(IN_OPEN & events)
    eventString.append("IN_OPEN ");
  if(IN_ISDIR & events)
    eventString.append("IN_ISDIR ");
  if(IN_UNMOUNT & events)
    eventString.append("IN_UNMOUNT ");
  if(IN_Q_OVERFLOW & events)
    eventString.append("IN_Q_OVERFLOW ");
  if(IN_CLOSE & events)
    eventString.append("IN_CLOSE ");
  if(IN_IGNORED & events)
    eventString.append("IN_IGNORED ");
  if(IN_ONESHOT & events)
    eventString.append("IN_ONESHOT ");

  return eventString;
}

#endif /* FileSystemEvent_H */
