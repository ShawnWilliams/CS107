using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 
	int actor_number = 0;
	actor_number = *(int*)actorFile;
	cout << endl << endl;
	
	char* cur_actor_name;
	int delta_address;
	void* cur_actor_address;
	string ac = player;
	const char* actor_search = ac.c_str();

	//binary search for actor
	int low, middle, high, result;
	short* movie_num;
	int* movie_delta_address;
	char* cur_movie_name;
	film cur_film;
	//string temp;
	low = 1;
	high = actor_number;
	while(low <= high){
		middle = (low + high)/2;
		delta_address = *((int*)actorFile + middle);
		cur_actor_address = (char*)actorFile + delta_address;
		cur_actor_name = (char*)cur_actor_address;
		result = strcmp(actor_search, cur_actor_name);
		if(result == 0){ 
			if(ac.length()%2 == 0){
				movie_num = (short*)((char*)cur_actor_address + ac.length() + 2);
			}
			else{
				movie_num = (short*)((char*)cur_actor_address + ac.length() + 1);
			}
			//cout << "The number of his/her films is: " << *movie_num << endl;

			if((ac.length() + 1 + 2)%4 == 0 || (ac.length() + 2 + 2)%4 == 0){
				movie_delta_address = (int*)(movie_num + 1);
			}
			else{
				movie_delta_address = (int*)(movie_num + 1 + 1);
			}
			
			for(int counter = 0; counter < *movie_num; counter++){
				cur_movie_name = (char*)movieFile + (*movie_delta_address);
				string temp(cur_movie_name);
				cur_film.title = temp;
				cur_film.year = 1900 + *(cur_movie_name + cur_film.title.length() + 1);
				films.push_back(cur_film);
				movie_delta_address++;
			}
			return true; 
		}
		else if(result < 0){high = middle - 1;}
		else{low = middle + 1;}
	}
	return false; 
}

bool imdb::getCast(const film& movie, vector<string>& players) const { 
	int movie_number = *(int*)movieFile;
	cout << endl << endl;
	int delta_address;
	void* cur_movie_address;
	film cur_film;
	char* cur_movie_year;

	//binary search for movie
	int low, middle, high;
	short* actor_num;
	int* actor_delta_address;
	char* cur_actor_name;
	short* temp_pointer;
	low = 1;
	high = movie_number;
	while(low <= high){
		middle = (low + high)/2;
		delta_address = *((int*)movieFile + middle);
		cur_movie_address = (char*)movieFile + delta_address;
		string temp((char*)cur_movie_address);
		cur_film.title = temp;
		cur_movie_year = (char*)cur_movie_address + cur_film.title.length() + 1;
		cur_film.year = *cur_movie_year + 1900;
		//cout << "movie name: " << cur_film.title << "---movie year" << cur_film.year << endl;
		if(cur_film.operator==(movie)){
			if((cur_film.title.length() + 2)%2 == 0){
				actor_num = (short*)((char*)cur_movie_address + cur_film.title.length() + 2);
			}
			else{
				actor_num = (short*)((char*)cur_movie_address + cur_film.title.length() + 3);
			}
			//cout << "The number of actor is: " << *actor_num << endl;

			if((cur_film.title.length() + 2 + 2)%4 == 0 || (cur_film.title.length() + 2 + 1 + 2)%4 == 0){
				temp_pointer = actor_num + 1;
				actor_delta_address = (int*)temp_pointer;
			}
			else{
				temp_pointer = actor_num + 2;
				actor_delta_address = (int*)temp_pointer;
			}
			//cout << "players.size = " << players.size() << endl;

			for(int counter = 0; counter < *actor_num; counter++){
				cur_actor_name = (char*)actorFile + (*actor_delta_address);
				//cout << "actor name:" << cur_actor_name << endl;
				players.push_back(cur_actor_name);
				actor_delta_address++;
			}
			//cout << "players.size = " << players.size() << endl;
			//cout << "test1" << endl;
			return true; 
		}
		else if(cur_film.operator<(movie)){
			low = middle + 1;
		}
		else{
			high = middle - 1;
		}
	}
	return false; 
}



imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
