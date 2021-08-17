using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
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
/*
This function gets string of player and searchs it in the actorFile data of actors and return 
the begin index Byte of player in the actorFile array.
*/
int get(const string&player,int *saw){
  int start=1;
  int end=*saw;
  while(start<end){
    int mid=(start+end)/2;
    int  cur=*(saw+mid);
    char* poss=(char*)saw+cur;
    const int compare=strcmp(player.c_str(),poss);
    if(!compare){
      return *(saw+mid);
    }else if(compare<0){
      end=mid;
    }else {
      start=mid+1;
    }
  }
  return 0;
}
/*Fills the vector of films with the movies where this actor starred and return true if this 
actor exists in database and false otherwise.
*/
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  int getStringIndex=get(player,(int*)actorFile);
  if(getStringIndex==0){
    return false;
  }
  char* playerName=(char*)actorFile+getStringIndex;
  int size=strlen(playerName)+1;
  size+=size%2;
  char* currentPointer=(char*)playerName+size;
  const short vectorSize=*((short*)(currentPointer));
  currentPointer+=sizeof(short);
  if((size+sizeof(short))%4){
    currentPointer+=2;
  }
  for(int i=0;i<vectorSize;i++,currentPointer+=sizeof(int)){
    const char* filmToAdd=(char*)movieFile+*(int*)currentPointer;
    const char yearToAdd=*((char*)filmToAdd+strlen(filmToAdd)+1);
    int dasa=1900+yearToAdd;
    films.push_back({filmToAdd,dasa});
  }
  return true;
  }

/*
Finds film with binary search, gets movie,which is film struct and int pointer of movieFile
and returns its begining index in the movieFile.
*/
  int getFilmIndex(const film& movie,int* saw){
    int start=0;
    int end=*saw;
    while(start<end){
      int mid=(start+end)/2;
      int cur=*(saw+mid);
      const char* curPointer=(char*)saw+cur;
      film possible;
      possible.title=curPointer;
      const char year=*((char*)curPointer+strlen(curPointer)+1);
      possible.year=1900+year;
      if(possible==movie){
        return cur;
      }else if(possible<movie){
        start=mid+1;
      }else if(movie<possible){
        end=mid;
      }
    }
    return 0;
  }
/*
Fills the vector of players with the actors of movies and if movie doesn't exist returns false
true otherwise.
*/
bool imdb::getCast(const film& movie, vector<string>& players) const { 
  int* saw=(int*)movieFile;
  int currentFilmIndex=getFilmIndex(movie,saw);
  if(currentFilmIndex==0){
    return false;
  }
  char* film_name=(char*)movieFile+currentFilmIndex;
  char* curPointer=(char*)film_name+strlen(film_name)+1+sizeof(char);
  curPointer+=(curPointer-film_name)%2;
  short numberOfPlayers=*((short*)(curPointer));
  curPointer+=sizeof(short);
  if((curPointer-film_name)%4){
    curPointer+=2;
  }
  for(int i=0;i<numberOfPlayers;i++,curPointer+=sizeof(int)){
    players.push_back((string)((char*)actorFile+*(int*)curPointer));
  }
  return true; 
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
