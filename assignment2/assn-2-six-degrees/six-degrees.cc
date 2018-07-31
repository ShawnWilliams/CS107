#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
#include <stdlib.h>
#include <string.h>
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } 
    else {
      // replace the following line by a call to your generateShortestPath routine... 
      //cout << endl << "No path between those two people could be found." << endl << endl;
      
      //Initialization
      path front_path(source);
      path last_path(source);
      list<path> partialPaths;
      set<string> previouslySeenActors;
      set<film> previouslySeenFilms;

      path front_path2(target);
      path last_path2(target);
      list<path> partialPaths2;
      set<string> previouslySeenActors2;
      set<film> previouslySeenFilms2;

      vector<film> cur_actor_film;
      vector<string> cur_costar; 
      string intermediary;
      bool bSignal = false;
      bool bSourcefirstfind = false;//if source first find intermediary, set bFirstfind true; else set false.  

      partialPaths.push_front(front_path);//add first path to partialPaths List
      partialPaths2.push_front(front_path2);
      string sLast_actor = source;
      string sLast_actor2 = target;
      previouslySeenActors.insert(source);
      previouslySeenActors2.insert(target);

    
      int iLoop_counter = 1;
      int iLoop_counter2 = 1;
      int cur_layer_num = 0;
      while(partialPaths.size() > 0 && partialPaths2.size() > 0 && partialPaths.front().getLength() + partialPaths2.front().getLength()<= 7){
        cur_layer_num = 0;
        for(int counter = 0; counter < iLoop_counter; counter++){
          front_path = partialPaths.front();
          partialPaths.pop_front();
          cur_actor_film.clear();//using for temp vector<film> variable
          db.getCredits(sLast_actor, cur_actor_film);
          for(vector<film>::iterator iter0 = cur_actor_film.begin(); iter0 != cur_actor_film.end(); iter0++){
            if(previouslySeenFilms.find(*iter0) == previouslySeenFilms.end()){//if current movie haven't been saught before
              previouslySeenFilms.insert(*iter0);//add this movie to previouslySeenFilms
              cur_costar.clear();//using for temp vector<string> variable
              db.getCast(*iter0, cur_costar);//look up movie's actor
              for(vector<string>::iterator iter2 = cur_costar.begin(); iter2 != cur_costar.end(); iter2++){
                if(previouslySeenActors.find(*iter2) == previouslySeenActors.end()){//if current actor haven't been saught before
                  previouslySeenActors.insert(*iter2);
                  cur_layer_num++;
                  last_path.addConnection(*iter0, *iter2);//add new connection
                  if(previouslySeenActors2.find(*iter2) != previouslySeenActors2.end()){
                    partialPaths.push_back(last_path);//add new path to previouslySeenFilms
                    //cout << "source find intermediary!" << endl;
                    intermediary = *iter2;
                    bSignal = true;
                    bSourcefirstfind = true;
                    break;
                  }
                  else{
                    partialPaths.push_back(last_path);//add new path to previouslySeenFilms
                  }
                  last_path = front_path;//copy the front path
                }
              }
            }//end if
            if(bSignal){break;}
          }//end for
          sLast_actor = front_path.getLastPlayer();
          if(bSignal){break;}
        }//end for
        iLoop_counter = cur_layer_num;
        //cout << "cur_layer_num = " << cur_layer_num << endl;
        cur_layer_num = 0;

        if(bSignal){break;}//break while

        for(int counter = 0; counter < iLoop_counter2; counter++){
          front_path2 = partialPaths2.front();
          partialPaths2.pop_front();
          cur_actor_film.clear();//using for temp vector<film> variable
          db.getCredits(sLast_actor2, cur_actor_film);
          for(vector<film>::iterator iter0 = cur_actor_film.begin(); iter0 != cur_actor_film.end(); iter0++){
            if(previouslySeenFilms2.find(*iter0) == previouslySeenFilms2.end()){//if current movie have't been saught before
                
              previouslySeenFilms2.insert(*iter0);//add this movie to previouslySeenFilms
              cur_costar.clear();//using for temp vector<string> variable
              db.getCast(*iter0, cur_costar);//look up movie's actor

              for(vector<string>::iterator iter2 = cur_costar.begin(); iter2 != cur_costar.end(); iter2++){
              
                if(previouslySeenActors2.find(*iter2) == previouslySeenActors2.end()){//if current actor have't been saught before
                  previouslySeenActors2.insert(*iter2);
                  cur_layer_num++;
                  last_path2.addConnection(*iter0, *iter2);//add new connection
                  if(previouslySeenActors.find(*iter2) != previouslySeenActors.end()){
                    partialPaths2.push_back(last_path2);//add new path to previouslySeenFilms
                    //cout << "target find intermediary!" << endl;
                    intermediary = *iter2;
                    bSignal = true;
                    break;
                  }
                  else{
                    partialPaths2.push_back(last_path2);//add new path to previouslySeenFilms
                  }
                  last_path2 = front_path2;//copy the front path
                }
              }//end for
            }//end if
            if(bSignal){break;}
          }//end for
          sLast_actor2 = front_path2.getLastPlayer();
          if(bSignal){break;}//break for
        }//end for
        iLoop_counter2 = cur_layer_num;
        //cout << "cur_layer_num2 = " << cur_layer_num << endl;
        cur_layer_num = 0;
        if(bSignal){break;}//break while
      }//end while
    
      path front_path3(intermediary);
      path last_path3(intermediary);
      list<path> partialPaths3;
      set<string> previouslySeenActors3;
      set<film> previouslySeenFilms3;

      string secondfound;//search target 
      string sLast_actor3 = intermediary;
      bSignal = false;

      partialPaths3.push_front(front_path3);
      previouslySeenActors3.insert(intermediary);
      if(bSourcefirstfind){secondfound = target;}
      else{secondfound = source;}//set target
      while(partialPaths3.size() > 0 && partialPaths3.front().getLength() <= 5){
        front_path3 = partialPaths3.front();
        partialPaths3.pop_front();

        cur_actor_film.clear();//using for temp vector<film> variable
        db.getCredits(sLast_actor3, cur_actor_film);
        for(vector<film>::iterator iter0 = cur_actor_film.begin(); iter0 != cur_actor_film.end(); iter0++){
          if(previouslySeenFilms3.find(*iter0) == previouslySeenFilms3.end()){//if current movie have't been saught before
              
              previouslySeenFilms3.insert(*iter0);//add this movie to previouslySeenFilms
              cur_costar.clear();//using for temp vector<string> variable
              db.getCast(*iter0, cur_costar);//look up movie's actor

              for(vector<string>::iterator iter2 = cur_costar.begin(); iter2 != cur_costar.end(); iter2++){
                if(previouslySeenActors3.find(*iter2) == previouslySeenActors3.end()){//if current actor have't been saught before
                  //cout << "test0" << endl;
                  previouslySeenActors3.insert(*iter2);
                  last_path3.addConnection(*iter0, *iter2);//add new connection
                  //cout << "last_path3.getLength = " << last_path3.getLength() << endl;
                  if(secondfound == *iter2){
                    partialPaths3.push_back(last_path3);//add new path to previouslySeenFilms
                    //cout << last_path3;
                    bSignal = true;
                    break;
                  }
                  else{
                    partialPaths3.push_back(last_path3);//add new path to previouslySeenFilms
                  }

                  last_path3 = front_path3;//copy the front path
                }
              }//end for
          }
          sLast_actor3 = front_path3.getLastPlayer();
          if(bSignal){break;}
        }//end for
        if(bSignal){break;}
      }//end while
      //cout << "intermediary " << intermediary << endl;
      
      if(bSourcefirstfind){
        cout << last_path << last_path3 << endl;
      }
      else{
        //cout << "11111111111" << endl;
        last_path3.reverse();
        last_path2.reverse();
        cout << last_path3 << last_path2 << endl;
      }
      
    }//end else
  }
  
  cout << "Thanks for playing!" << endl;
  return 0;
}

