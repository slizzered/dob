/*
 * ROADMAP
 *
 * * Copy mechanism (Rsync) --> done
 * * Mounting of server data --> done with /script/mount_server but needs more work
 * * Create InotifyFileSystemScanner alternative because is not threadsafe
 * * Replace libinotify with own implementation
 * * Write Wrapper for Inotify events for more general use
 * * Commandline installer
 * * Scanning of server data
 * * Ask in case of big data
 * * Graphical user interface
 * * Versioning of backup data (Git)
 *
 */

// Include libraries
#include <iostream>
#include <string>
#include <stdlib.h>
#include <gtkmm.h>
#include <signal.h>

// Include classes
#include <InotifyFileSystemScanner.h>
#include <RemoteSyncManager.h>
#include <LocalSyncManager.h>
#include <OptimizedEventManager.h>
#include <ConfigFileParser.h>
#include <CommandLineParser.h>
#include <Tray.h>

using namespace std;

int gui_test(int argc, char *argv[], vector<Profile> *pProfiles);

int main(int argc, char *argv[]){
  // Variable Definitions
  void* no_arg = NULL;
  vector<Profile>* pProfiles;
  string scanFolder;
  string destFolder;
  string configFileName;
  ConfigFileParser configFileParser;
  CommandLineParser commandLineParser;
  SyncManager * pSyncManager;
  EventManager * pEventManager;
  FileSystemScanner * pFileSystemScanner;

  // Parse commandline and Configfile
  if(!commandLineParser.parseCommandLine(argc, argv)){
    cout << "\nC No parameters found";
    cout << "\nC Usage: ./odb --config=CONFIGFILE";
    cout << "\n";
    return 0;
  }
  configFileName = commandLineParser.getConfigFileName();
  configFileParser.ParseConfigFile(configFileName);
  pProfiles = configFileParser.GetProfiles();

  // Start to run profiles
  vector<Profile>::iterator profileIter;
  for(profileIter = pProfiles->begin(); profileIter < pProfiles->end(); profileIter++){
    if(profileIter->IsValid()){
      scanFolder = profileIter->GetSyncFolder();
      destFolder = profileIter->GetDestFolder();
  
      // Setup synccomponents
      pSyncManager  = new LocalSyncManager(destFolder);
      pEventManager = new OptimizedEventManager(pSyncManager);
      pFileSystemScanner = new InotifyFileSystemScanner(scanFolder, pEventManager);
      profileIter->SetSyncManager(pSyncManager);
      profileIter->SetEventManager(pEventManager);
      profileIter->SetFileSystemScanner(pFileSystemScanner);
    }

  }
  
  if(pProfiles->size() > 1){
    cerr << "\nC Can´ t handle more then one profile, please comment all except of one";
    cerr << "\n";
    return 0;
  }

  // Start experimental gui
  return gui_test(argc, argv, pProfiles);

}
/**
 * @todo gui can only handle one profile should be able to handle more!
 **/
int gui_test(int argc, char *argv[], vector<Profile>* pProfiles){
  /*
 Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,"org.gtkmm.examples.base");
  Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("gui/tray.glade");
  Gtk::Window * pMainWindow = 0;
  builder->get_widget("mainWindow", pMainWindow);
  app->run(*pMainWindow);
  */

  Gtk::Main kit(argc, argv);
  Tray *tray = new Tray(pProfiles);
  
  vector<Profile>::iterator profileIter;
  for(profileIter = pProfiles->begin(); profileIter < pProfiles->end(); profileIter++){
      profileIter->GetEventManager()->SignalEvent().connect(sigc::mem_fun(*tray,
              &Tray::OnEventManagerSignal) );

  }

  Gtk::Main::run(*tray);
  return 0;

  /*
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,"org.gtkmm.examples.base");
  Gtk::Window window;
  Glib::RefPtr<Gtk::StatusIcon> m_refStatusIcon = Gtk::StatusIcon::create(Gtk::Stock::HOME);
  return app->run(window);
  */

}
