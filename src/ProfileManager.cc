#include <ProfileManager.h>
#include <iostream>

ProfileManager::ProfileManager(std::vector<Profile>* pProfiles) : 
  mpProfiles(pProfiles){
  
}

ProfileManager::~ProfileManager(){
  free(mpProfiles);
}

bool ProfileManager::startProfile(std::string profileName){
  Profile * pProfile = getProfileByName(profileName);
  if(pProfile){
    pProfile->startProfile();
    return true;
  }
    
  return false;
}

bool ProfileManager::stopProfile(std::string profileName){
  Profile * pProfile = getProfileByName(profileName);
  if(pProfile){
    pProfile->stopProfile();
    return true;
  }
    
  return false;
}

bool ProfileManager::restartProfile(std::string profileName){
  dbg_printc(LOG_DBG, "ProfileManager", "restartProfile","Restart Profile : %s", profileName.c_str());
  Profile * pProfile = getProfileByName(profileName);
  if(pProfile){
    pProfile->stopProfile();
    pProfile->startProfile();
    return true;
  }
    
  return false;

}

Profile* ProfileManager::getProfileByName(std::string profileName){
  for(unsigned i = 0; i < mpProfiles->size(); ++i){
    if(!mpProfiles->at(i).getName().compare(profileName)){
      return &(mpProfiles->at(i));
    }
  }
  return NULL;
}

bool ProfileManager::startProfiles(){
  std::vector<Profile>::iterator profileIter;
  bool profilesAreStarted = true;
  for(profileIter = mpProfiles->begin(); profileIter < mpProfiles->end(); profileIter++){
    profilesAreStarted &= profileIter->startProfile();
    
  }

  return profilesAreStarted;
}
