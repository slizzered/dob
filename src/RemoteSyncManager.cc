#include "RemoteSyncManager.h"

RemoteSyncManager::RemoteSyncManager(std::string destFolder, std::string syncType, std::string destProtocol):
  SyncManager(destFolder, syncType),
  mDestProtocol(destProtocol){

}

/**
 *  @todo sourcefolder for "\" as last char
 *
 **/
bool RemoteSyncManager::syncSourceFolder(std::string sourceFolder){
  dbg_printc(LOG_DBG, "RemoteSyncManager","SyncSourceFolder", "Syncronise source and destination folder");
  std::string rsync_push_query = "rsync -vruLKpt --progress --inplace ";
  std::string rsync_pull_query = "rsync -vruLKpt --progress --inplace ";
  rsync_push_query
    .append(sourceFolder)
    .append(" ")
    .append(mDestFolder);
  rsync_pull_query
    .append(mDestFolder)      
    .append(" ")
    .append(sourceFolder);
  cerr << "\n";
  dbg_printc(LOG_DBG, "RemoteSyncManager", "SyncSourceFolder", "Rsync pull string: %s", rsync_pull_query.c_str());
  system(rsync_pull_query.c_str());
  dbg_printc(LOG_DBG, "RemoteSyncManager", "SyncSourceFolder", "Rsync push string: %s", rsync_push_query.c_str());
  system(rsync_push_query.c_str());

  return true;
}

bool RemoteSyncManager::syncFolder(std::string sourceFolder, std::string syncFolder, std::string folder){
  std::string rsync_query = "rsync -vruLKpt --progress --inplace ";  
  rsync_query
    .append(syncFolder)
    .append(folder)
    .append(" ")
    .append(mDestFolder)
    .append(syncFolder.substr(sourceFolder.length(), syncFolder.length()));

  dbg_printc(LOG_DBG,"RemoteSyncManager","SyncFolder","%s ", rsync_query.c_str());  
  system(rsync_query.c_str());

  return true;
  
}

bool RemoteSyncManager::syncFile(std::string sourceFolder, std::string syncFolder){
  std::string rsync_query = "rsync -vruLKpt --progress --inplace ";  
  rsync_query
    .append(syncFolder)
    //.append(folder)
    .append(" ")
    .append(mDestFolder)
    .append(syncFolder.substr(sourceFolder.length(), syncFolder.length()));

  dbg_printc(LOG_DBG,"RemoteSyncManager","SyncFile","%s ", rsync_query.c_str());  
  system(rsync_query.c_str());

  return true;

}

/**
 * @todo there should be a quicker alternative to remove files as rsync
 *
 **/
bool RemoteSyncManager::removeFolder(std::string sourceFolder, std::string syncFolder, std::string folder){
  
  std::string rm_query = "rsync -vruLKpt --delete --progress --inplace ";
  rm_query
    .append(syncFolder)
    .append(" ")
    .append(mDestFolder)
    .append(syncFolder.substr(sourceFolder.length(), syncFolder.length()));

  dbg_printc(LOG_DBG, "RemoteSyncManager","RemoveFolder","%s", rm_query.c_str());
  system(rm_query.c_str());

  return true;

}

/**
 * @todo fill with content
 *
 **/
bool RemoteSyncManager::checkDestFolder(){
  /*
  string mountpoint_query = "mountpoint -q ";
  mountpoint_query.append(mDestFolder);
  return !system(mountpoint_query.c_str());
  */
  return false;
}

/**
 * @todo fill with content
 *
 **/
bool RemoteSyncManager::mountDestFolder(){
  /*
  if(!mMountOptions.compare(""))
    return false;

  string mount_query;
  mount_query
    .append("sshfs -o ServerAliveInterval=15 ")
    .append(mMountOptions)
    .append(" ")
    .append(mDestFolder);
  cerr << mount_query << "\n";
  system(mount_query.c_str());

  return true;
  */
  return false;
}
