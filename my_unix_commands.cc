#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include </Users/thp/Documents/courses/CS179F/fs.h>
#include <sys/stat.h>  
#include <dirent.h>    
#include <errno.h>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <map>

using namespace std;

struct inode {
  off_t i_size;
};

struct inode ilist[300];

inline // a simple utility for splitting strings at a find-pattern
vector <string>
spilt( const string s, const string pat) {
  string c;
  vector<string> v;
  int i = 0;
  for(;;) {
    int t = s.find(pat,i);
    int j = (t == string::npos) ? s.size() : t;
    string c = s.substr(i,j-i);
    v.push_back(c);
    i = k+pat.size();
    if ( t == string::npos ) return v;
  }
}

