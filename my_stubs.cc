
// Note: this code is a work-in-progress.  This version: 2:00PM PST 11/14/15

// I intend that the prototypes and behaviors of these functions be
// identical to those of their glibc counterparts, with three minor
// exceptions:

//   *  open(...) returns the handle of a file rather than the
//      descriptor of an open file.

//   *  pread(...) and pwrite(...) take
//      the handle of a file rather than the descriptor of an open file
//      as their respective first arguments.

// Recall that openfiles are to files what stringstreams are to strings. 

// In our case, we have no need to create an open file from the file
// in question; that gets done at a higher level. 


// Josef Pfeiffer's bbfs.c includes:
//#include "params.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
//#include <fuse.h>  
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


#ifdef HAVE_SYS_XATTR_H
c#include <sys/xattr.h>
#endif

// Here we include
// C stuff
#include "my_stubs.H"
#include </usr/include/linux/fs.h>  // needed for compilation on vagrant
#include <sys/stat.h>  // this has our official definition of stat
#include <dirent.h>    // this has our official definition of dirent
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <err.h>
// C++ stuff
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <iostream>  
#include <errno.h>

using namespace std;

const int DELETED = INT_MAX;

// Prototypes for local functions.  There's no need to put these into
// my_stubs.H, since these functions are local to my_stubs.cc.
void show_stat( struct stat& root );
void initialize();                  
ino_t find_ino( string path );      


// Here is a convenient macro for debugging.  cdbg works like cerr but 
// prefixes the error message with its location: line number and function
// name.
#define cdbg cerr <<"\nLn "<<dec<<  __LINE__ << " of "  << __FUNCTION__ << ": "

map< ino_t,int > open_files;
string cwd;   // to hold name of current working directory but not used yet.

struct dirent_frame { // The official definition of dirent puts its users
  // into "undefined behavior"
  dirent the_dirent;
  char overflow[NAME_MAX]; // to catch overflow from one-char name field.
};


class File {
public:
  //inode metadata;                     // for use by all files
  struct stat metadata;                 // for use by all files
  // one or the other of the following (data and frame) should be empty.
  string data;                          // for use by regular files
  vector<dirent_frame> dentries;        // for use by directories
  // list<dirent_frame> dentries;        // it'd be cleaner to make this a list
};


class Ilist {
public:
  int count;
  map<ino_t,File> entry;
  int next() {
    static int count = 2;  // ino counter stats at 2.
    return count++;
  }
} ilist; 


void initialize() { // now called from main() but could be called from
                    // Ilist constructor (I think).
  my_mkdir("/", 0700);
  char tmp[PATH_MAX];
  getcwd(tmp,998);
  cwd = string(tmp);  
  // cwd now hold the path to the initial current working directory.
  // But, we don't yet use it.
}

void show_stat( struct stat& root ) {
  // Displays the entries in a stat in same formats that Pfeiffer uses.
  cerr << "SHOW\n";
  cerr << "st_dev     = " << dec << root.st_dev     << endl;  
  cerr << "st_ino     = "        << root.st_ino     << endl;  
  cerr << "st_mode    = " << oct << root.st_mode    << endl;  
  cerr << "st_nlink   = "        << root.st_nlink   << endl;  
  cerr << "st_uid     = " << dec << root.st_uid     << endl;  
  cerr << "st_gid     = " << dec << root.st_gid     << endl;  
  cerr << "st_rdev    = "        << root.st_rdev    << endl;  
  cerr << "st_size    = "        << root.st_size    << endl;  
  cerr << "st_blksize = "        << root.st_blksize << endl;  
  cerr << "st_blocks  = "        << root.st_blocks  << endl;  
  cerr << "st_atime   = " << hex << root.st_atime   << endl;  
  cerr << "st_mtime   = " << hex << root.st_mtime   << endl;  
  cerr << "st_ctime   = " << hex << root.st_ctime   << endl;  
}; 


inline  // a simple utility for splitting strings at a find-pattern.
vector<string>
split(const string s, const string pat ) {
  string c;
  vector<string> v;
  int i = 0;
  for (;;) {
    int t = s.find(pat,i);
    int j = ( t == string::npos ) ? s.size() : t;
    string c = s.substr(i,j-i);
    v.push_back(c);
    i = j+pat.size();
    if ( t == string::npos ) return v;
  }
}

inline  // joins vector of strings separating via a given pattern.
string
join( const vector<string> v, const string pat, int start=0, int end=-1 ) {
  if ( end < 0 ) end += v.size();
  //  assert ( start < v.size() );   // should throw an exception.
  if ( start >= v.size() ) return "";
  string s = v[start++];
  for ( int i = start; i <= end; ++i )  s += pat + v[i];
  return s;
}

const int ok = 0;
const int an_err = -1;
// Note that for pointer and ino_t return values, 0 indicates an error.


// lstat is called at line #95 of bbfs.c
int my_lstat( const char* path, struct stat *statbuf ) {
  //cdbg << "lstat has been called and is calling find_ino on " << path << endl;
  ino_t fh = find_ino(path);
  //cdbg << "fh = " << fh << endl;
  int retstat;
  if ( fh == 0 ) {
    retstat = an_err;
  } else {
    retstat = my_fstat(fh,statbuf);
  }  
  return retstat;
}  

// called at line #125 of bbfs.c
int my_readlink( const char *path, char *link, size_t size ) {
  return an_err;  
}  

// called at line #168 of bbfs.c.  See line #151.
int my_mknod( const char *path, mode_t mode, dev_t dev ) {
  //cdbg <<"mknod been called with " << path << " " << oct << mode 
  //       << " " << dev <<endl;
  // Returns error code only.  It should make a file and install it
  // into ilist and instrall a dentry for it in the parent directory.

  // To later get the handle for that inode use find_ino().  To get a
  // pointer to that file use find_file.  IMHO, we should use it to
  // creat both directories and regular files, but the man page leaves
  // out directories.  So, I've not used it, and it's not been tested.


  dev = 100;   // a device number that's unlikely to be used on vagrant

  // Now we create and configure this File's metadata (inode/struct).
  int the_ino = ilist.next();     // Get and record this directory's ino
  struct stat& md = ilist.entry[the_ino].metadata;   // Create this file
  mode_t old_umask = umask(0);  // sets umask to 0 and returns old value
  umask(old_umask);                       // restores umask to old value
  md.st_dev     = dev;                /* ID of device containing file */
  md.st_ino     = the_ino;                            /* inode number */
  //cdbg << "mode = " << oct << mode << " old_umask = " 
  //     << oct << old_umask << endl;
  md.st_mode    = ( mode & ~ old_umask);                /* protection */
  md.st_nlink   = 1;                          /* number of hard links */
  md.st_uid     = geteuid();                      /* user ID of owner */
  md.st_gid     = getegid();              /* group ID of owning group */
  md.st_rdev    = 0;                   /* device ID (if special file) */
  md.st_size    = 0;       /* size in bytes for reg-files and symlinks*/
  md.st_blksize = 4096;              /* blocksize for file system I/O */
  md.st_blocks  = 0;               /* number of 512B blocks allocated */
  md.st_atime   = time(0);                     /* time of last access */
  md.st_mtime   = time(0);               /* time of last modification */
  md.st_ctime   = time(0);              /* time of last status change */

  // The man page on STAT(2) is an excellent resource for configuring
  // a stat, e.g., http://man7.org/linux/man-pages/man2/stat.2.html


  // Now we push a dentry for this directory onto the parent directory.
  // except for root directory
  vector<string> v = split( path, "/" );  // split the path
  string the_name = v.back();
  v.pop_back();

  // Create, configure, and install a dentry for this new directory's
  // in its parent directory.  But, omit this the first time, since
  // the root directory has no parent.
  // cdbg << "the_ino is " << the_ino << endl;
  ino_t parent_ino = find_ino(join(v,"/"));  
  static bool first_time = true;  // static vars get initialized once
  if ( first_time ) {                    
    first_time = false;
  } else {    // Omit this on first invocation, i.e., when making root
    ino_t fh = find_ino(join(v,"/"));
    dirent_frame df;                    
    df.the_dirent.d_ino = the_ino; 
    strcpy(df.the_dirent.d_name, the_name.c_str());  
    //cdbg << "Inserting dentry for " << the_ino << " w name " \
           << the_name << " into directory " << join(v,"/");
    ilist.entry[fh].dentries.push_back(df);  // push df onto the back of parent
  }

  // Branch around this if not a directory
  if ( S_ISDIR(mode) ) {
    // Install dentries for . and .. within this new directory
    dirent_frame df;

    strcpy(df.the_dirent.d_name, ".");
    df.the_dirent.d_ino  = the_ino;  
    ilist.entry[the_ino].dentries.push_back(df);

    strcpy(df.the_dirent.d_name, "..");
    df.the_dirent.d_ino  = parent_ino;  // needs to be adjusted for root directory
    ilist.entry[the_ino].dentries.push_back(df);
  }
  else if ( S_ISREG(mode) ) {
    ilist.entry[the_ino].data = "[insert text here]"; // ???
  }
  return ok;

}  

//check the the permissions of a file
//by using bitwise operations to determine
//
bool check_permissions(ino_t fh, string operation)
{
   // cout << "fh: " << fh  << " ilist.count: " << dec << ilist.count << endl;
    if(fh < 2)// || ilist.count < fh)
    {
        cout << "Invalid fh.\n";
        return false;
    }
    
    //current user's gid and uid
    uid_t cur_uid = geteuid(); //Payne also uses this function when making files
    gid_t cur_gid = getegid();  
    //files gid and uid
    uid_t file_owneruid = ilist.entry[fh].metadata.st_uid;
    gid_t file_ownergid = ilist.entry[fh].metadata.st_gid;
    mode_t file_permissions = ilist.entry[fh].metadata.st_mode; 
    //cout << "file_owneruid: " << file_owneruid << endl;
    //cout << "cur_uid: " << cur_uid << endl;
    /* octal values for different flag permissions
     * will simply do a bitwise AND to determine if user has permissions
     * */
    mode_t uread_mode = 400;
    mode_t uwrite_mode = 200;
    mode_t uexec_mode = 100;
    mode_t gread_mode = 040;
    mode_t gwrite_mode = 020;
    mode_t gexec_mode = 010;
    mode_t oread_mode = 004;
    mode_t owrite_mode = 002;
    mode_t oexec_mode = 001;
    mode_t result = 0;
    
    //check if uid's gid's match
    bool is_owner = false;
    if(cur_uid == file_owneruid)
    {
      is_owner = true;   
    }
    else
    {
        is_owner = false;
    }
    
    bool is_group = false; 
    if(cur_gid == file_ownergid)
    {
        is_group =  true;
    }
    else
    {
        is_group = false;
    }
    
    if(operation == "read")
    {
        if(is_owner)
        {
          return file_permissions&uread_mode;
        }
        else if(is_group)
        {
          return file_permissions&gread_mode;
        }
        else
        {
         return file_permissions&oread_mode;   
        }
        return false;
    }
    else if(operation == "write")
    {
        if(is_owner)
        {
          return file_permissions&uwrite_mode;
        }
        else if(is_group)
        {
          return file_permissions&gwrite_mode;
        }
        else
        {
          return file_permissions&owrite_mode;   
        }
        
        return false;
    }
    else if(operation == "execute")
    {
        if(is_owner)
        {
          return file_permissions&uexec_mode;
        }
        else if(is_group)
        {
          return file_permissions&gexec_mode;
        }
        else
        {
          return file_permissions&oexec_mode;   
        }
      //won't need as we will not have executables
      return false;
    }
    return false;
}

// called at line #186 of bbfs.c
int my_mkdir( const char *path, mode_t mode ) {
  // returns an error code.
  //cdbg << "mkdir has been called with path \"" << path<< "\" and mode " \
  //     << oct <<  mode << endl;
  return my_mknod(path, (S_IFDIR | mode), 100 );
}  // my_mkdir

// called at line #203 of bbfs.cg
int my_unlink( const char *path ) {
  vector<string> v = split(string(path),"/");
  string fileName = v.back();
  string dirpath = join(v, "/");
  ino_t fh = find_ino(path);
  if(fh == 0)
  {
    cout << "File Not Found\n";
    return an_err;
  }
  if(open_files.find(fh) != open_files.end())
  {
    if(open_files.at(fh) > 0)
    {
      cout << "File In Use! Cannot Unlink!\n";
      return an_err;      
    }
  }
  else
  {
    if(check_permissions(fh,"write") == false)
    {
      uid_t cur_uid = geteuid(); 
      cout << "Current user: " << getpwuid(cur_uid)->pw_name  
           << " does not have write permissions to unlink this file.\n"; 
      return an_err;   
    }
        
    if(ilist.entry[fh].metadata.st_nlink > 0) //Decrease number of opened links to a file
    {
      (ilist.entry[fh].metadata.st_nlink)--;
    }
    if(ilist.entry[fh].metadata.st_nlink == 0) //If the number of links to a file is 0, delete the file
    {
      //ilist.entry[fh].metadata.st_ino = DELETED;
      map<ino_t, File>::iterator ilist_it;
      vector<dirent_frame>::iterator dentry_it;
      
      //This is a way to find the directory entry
      //Look through the map for all files and directories
      for(ilist_it = ilist.entry.begin(); ilist_it != ilist.entry.end(); ilist_it++)
      {
        //If the current element is a directory
        if(S_ISDIR(ilist_it->second.metadata.st_mode))
        {
          //Parse the directory
          for(dentry_it = ilist_it->second.dentries.begin(); dentry_it != ilist_it->second.dentries.end(); dentry_it++)
          {
            //If the inode number is the same as the file we want to delete
            //if(ilist.entry[fh].metadata.st_ino == )
            cout << "fh: " << fh << " d_ino: " << dentry_it->the_dirent.d_ino << "\n";
            if(fh == dentry_it->the_dirent.d_ino)
            {
              //Delete the file from the directory
              ilist_it->second.dentries.erase(dentry_it);
              break;
            }
          }
        }
      }
      //Final Step to Deleting a file
      ilist_it = ilist.entry.find(fh);
      ilist.entry.erase(ilist_it);
    }
  }
  return 0;
}  

// called at line #220 of bbfs.c
int my_rmdir( const char *path ) {
  // See http://linux.die.net/man/2/rmdir for a full list of all 13
  // possible errors for rmdir.

  ino_t fh = find_ino( path );
  File the_dir = ilist.entry[fh];
  if ( ! S_ISDIR(the_dir.metadata.st_mode) ) { 
    cdbg << "does not exist\n";
    errno = ENOTDIR;
    return an_err;
  }
  if ( ! the_dir.dentries.size() > 2 ) {  // for . and ..
    cdbg << "not empty\n";
    errno = ENOTEMPTY;
    return an_err;
  }
  vector<string> vs = split( path, "/" );
  vs.pop_back();
  string parent_path = join(vs,"/");
  parent_path = "/" + parent_path;  // FIX this hack
  // cdbg << "Parent path is " << parent_path << endl;
  ino_t parent = find_ino(parent_path);
  // cdbg << "Parent ino is " << parent << endl;
  vector<dirent_frame>& v = ilist.entry[parent].dentries;
  for(vector<dirent_frame>::iterator it = v.begin(); it != v.end(); ++it ) {
    // We erase him from his parent directory.
    if ( it->the_dirent.d_ino == fh ) {
      // cdbg << "erasing " << fh << " from " << parent << endl;
      v.erase(it);  
      break;  // Must stop iterating now!
    }
  }    
  if ( --the_dir.metadata.st_nlink == 0 ) {
    ilist.entry.erase(fh);  // get rid of this file
  }
  return ok;
}  

// called at line #241 of bbfs.c
int my_symlink(const char *path, const char *link) {
  return an_err;  
}  


int my_rename( const char *path, const char *newpath ) {

  int i = 0;
  vector<string> v = split( path, "/" );
  string old_file_name = v.back();
  v.pop_back();

  ino_t old_parent_ino = find_ino(join(v, "/"));
  ino_t old_file_ino = find_ino(path);
  if(old_file_ino == 0)
  {
     cout << "Failed to find file: \"" << path << "\"" << endl;
     return -1;
  }

  if(check_permissions(old_file_ino,"write") == false)
  {
    uid_t cur_uid = geteuid(); 
    cout << "Current user: " << getpwuid(cur_uid)->pw_name  
         << " does not have write permissions to rename this file.\n"; 
         return an_err;   
  }
  v = split( newpath, "/" );
  string new_file_name = v.back();
  v.pop_back();

  ino_t new_parent_ino = find_ino(join(v, "/"));
  ino_t new_file_ino = find_ino(path);
  for(vector<dirent_frame>::iterator it = ilist.entry[old_parent_ino].dentries.begin(); it !=  ilist.entry[old_parent_ino].dentries.end(); ++it){
    if(it->the_dirent.d_name == old_file_name)
    {
      //return 0;
      //dirent char d_name[]  
     //it->the_dirent.d_name = new_file_name.c_str();
     for(i = 0; i < new_file_name.size() ; i++)
     {
         it->the_dirent.d_name[i] = new_file_name[i];
     }
     //slight workaround
     //however if the filename is too long then this could very well
     //cause a segfault
     it->the_dirent.d_name[i] = '\0';
     return 0;
    }
    
  }

}

  //dirent_frame df;
  // Add a new dentry
  // df.the_dirent.d_ino = old_file_ino;
  // strcpy(df.the_dirent.d_name, old_file_name.c_str());
  // ilist.entry[old_parent_ino].dentries.push_back(df);
  
  //ilist.entry[old_parent_ino].dentries.


// called at line #279 of bbfs.c
int my_link(const char *path, const char *newpath) {
  vector<string> v = split(string(newpath),"/");
  string tail = v.back();
  string dirpath = join(v, "/");
  ino_t fh = find_ino(path);
  if ( ! S_ISDIR( ilist.entry[fh].metadata.st_mode ) ) {
    errno = EPERM; 
    return an_err; 
  }
  ino_t fh2 = find_ino(newpath);
  if ( ! S_ISDIR( ilist.entry[fh2].metadata.st_mode ) ) {
    errno = EPERM; 
    return an_err; 
  }
  ++ ilist.entry[fh].metadata.st_nlink;
  // check for overflow.
  dirent d;
  d.d_ino = fh;
  strcpy( d.d_name, tail.c_str() );
  return ok;  
}  

/* Change permissions of a file. The new file permissions of the file
   are specified in [mode], which is a bit mask created by ORing together
   zero or more of the following: S_ISUID(04000), S_ISGID(02000), ...
   Return value: On sucess, zero is returned. On error, -1 is returned,
   and [errno] is set appropriately.
   (Reference: http://linux.die.net/man/2/chmod)
*/
/* Each file and directory has 3 user based group permission groups:
 * owner
 * group
 * all users
 * 
 * */
// called at line #296 of bbfs.c
int my_chmod(const char *path, mode_t mode) {
  ino_t fh = find_ino(path);
  //not sure if it is allowed to chan
  
  if ( fh > 2 )
  {
  ilist.entry[fh].metadata.st_mode =  mode;
  cout << "Permissions changed on: \"" << path << "\"" << endl;
  return 0;
  }
  cout << "Failed to find file: \"" << path << "\"" << endl;
  return an_err;  
}  

// called at line #314 of bbfs.c
/*chown changes the user and/or group ownership of each given file. 
 * If only an owner (a user name or numeric user ID) is given, 
 * that user is made the owner of each given file, and the files' 
 * group is not changed. 
 * If the owner is followed by a colon and a group name 
 * (or numeric group ID), with no spaces between them, 
 * the group ownership of the files is changed as well. 
 * If a colon but no group name follows the user name, that
 *  user is made the owner of the files and the group of the 
 * files is changed to that user's login group. If the colon
 *  and group are given, but the owner is omitted, 
 * only the group of the files is changed; in this case, 
 * chown performs the same function as chgrp.
 *  If only a colon is given, or if the entire operand
 *  is empty, neither the owner nor the group is changed. 
*/
int my_chown(const char *path, uid_t uid, gid_t gid) {
        
     //uid_t     st_uid;
     /* TODO: Check file permissions */
     // md.st_gid     = getegid();   
    // Payne uses these get the username / group name
    //getpwuid(st.st_uid)->pw_name,     // password name
    //getgrgid(st.st_gid)->gr_name,     // group name
    uid_t file_owneruid = 0;// ilist.entry[fh].metadata.st_uid;
    uid_t cur_uid = geteuid(); //Payne also uses this function when making files
    ino_t fh = find_ino(path);;
    if ( fh == 0 ) 
    {
        cout << "Filepath: \"" << path << "\" not found.\n";
        return an_err;
    } 
    else 
    {
        //for now just comparing uids
        uid_t file_owneruid = ilist.entry[fh].metadata.st_uid;
        //group_id to be used if user also specifies group
        gid_t file_ownergid = ilist.entry[fh].metadata.st_gid;
        //TODO: check permissions then change if allowed
         ilist.entry[fh].metadata.st_uid = uid;
         cout << "changed uid to: " << uid << endl;
         //cout << "File Owner changed to: " <<  getpwuid(ilist.entry[fh].metadata.st_uid)->pw_name << endl;
          //don't change gid if gid is -1
         if(gid != 0)
         {
            ilist.entry[fh].metadata.st_gid = gid;
            cout << "changed gid to: " << gid << endl;
            //cout << "File Group changed to: " <<  getgrgid(ilist.entry[fh].metadata.st_gid)->gr_name << endl;
         }
        return 0;
    }  
  return an_err;  
}  

// called at line #331 of bbfs.c
/*The truncate() and ftruncate() functions cause the regular 
 * file named by path or referenced by fd to be truncated to 
 * a size of precisely length bytes.
 * If the file previously was larger than this size, 
 * the extra data is lost. If the file previously was shorter, 
 * it is extended, and the extended part reads as null bytes ('\0').
 * The file offset is not changed.
 * If the size changed, then the st_ctime and st_mtime fields 
 * (respectively, time of last status change and time of 
 * last modification; see stat(2)) for the file are updated, 
 * and the set-user-ID and set-group-ID permission bits may be cleared.
 * With ftruncate(), the file must be open for writing; 
 * with truncate(), the file must be writable.
 * */
int my_truncate(const char *path, off_t newsize) {
    ino_t fh = find_ino(path);
    //ilist.entry[fh].data.size() < offset
    /* TODO: check file permissions to see if can write to file 
     * */
    if ( fh == 0 ) 
    {
        cout << "Filepath: \"" << path << "\" not found.\n";
        return an_err;
    } 
    else 
    {
     //exploit the fact that we have strings here
     //when the string is increased
     //the function sets the added characters to null
        ilist.entry[fh].data.resize(newsize);   
        return 0;
    }  
  return an_err;  
}  

// called at line #349 of bbfs.c
int my_utime(const char *path, struct utimbuf *ubuf) {
  return an_err;  
}  

// called at line #376 of bbfs.c.  Returns file handle not a file descriptor
int my_open( const char *path, int flags ) {
  // Write a function called "lookup" that takes a full path and the
  // handle of a directory and returns the handle of the inode having
  // that name.  To do so, split full path.  Then starting with the
  // handle of the root directory, look up each segment in the
  // directory whose handle you currently have.  Use a linear search
  // that consists of successive calls to readdir() to find the
  // corresponding directory entry, which has type struct dirent.
  // Return its d_fileno, unless there's an error and then return -1.

  //static filecount = 0;

  ino_t fh = find_ino(path);
  
  if(ilist.entry[fh].metadata.st_ino == DELETED)
  {
    cout << "File: \"" << path << "\" failed to open.\n";
    return -1;    
  }
  else if ( fh > 2 ) {
    cout << "file opened, fh = " << fh << endl;
      //search open_file map for fh
     //returns map::end() if nothing found
    if(open_files.find(fh) != open_files.end())
    {
        cout << "before open_files.at(fh): " << open_files.at(fh) << endl;
        open_files.at(fh) = open_files.at(fh) + 1;
        cout << " after openfiles.at(fh): " << open_files.at(fh) << endl;
    }
    else
    {
        //if file not opened insert file into open_file list
        open_files.insert(std::pair<ino_t,int >(fh,1));      
        cout << " new  openfiles.at(fh): " << open_files.at(fh) << endl;
    } 
    return fh;
  } 
  else if ( flags & O_CREAT ) {
    mode_t mode = 0644;
    my_creat( path, mode );
    fh = find_ino(path);
    cout << "file created & opened, fh = " << fh << endl;
    //insert file into open file map
    open_files.insert(std::pair<ino_t,int >(fh,1));
    cout << " new  openfiles.at(fh): " << open_files.at(fh) << endl;
    
    return fh;
  }
  else
  {
    cout << "File: \"" << path << "\" failed to open.\n";
    return -1;
  }
}

// called at line #411 of bbfs.c  Note that our firt arg is an fh not an fd
int my_pread( int fh, char *buf, size_t size, off_t offset ) {
    //cout << "ilist.entry[fh].data.size() = " << ilist.entry[fh].data.size() << endl;
    
    if(offset < 0 || ilist.entry[fh].data.size() < offset)
    {
        return an_err;
    }   
    if(offset == ilist.entry[fh].data.size() - 1)
    {
        return 0;
    }
    
    if(check_permissions(fh,"read"))
    {
        //buf = new char[size + 1];
        int i;  
        for(i = 0; i < size; i++)
        {
            if(ilist.entry[fh].data[offset + i] == '\0')
            {
              cout << "data[offset+i]: " << ilist.entry[fh].data[offset + i] << endl;
                break;
            }
            buf[i] = ilist.entry[fh].data[offset + i];
                    //cout << buf[i] << " ";
        }
        //buf[i + 1] = '\0';
        return i;
    }
    else
    {
        uid_t cur_uid = geteuid(); 
        cout << "Current user: " << getpwuid(cur_uid)->pw_name  
             << " does not have permission to read from this file.\n";
        return 0;
    }
}  

// called at line #439 of bbfs.c  Note that our firt arg is an fh not an fd
int my_pwrite( int fh, const char *buf, size_t size, off_t offset ) 
{
  
  if(offset < 0 || ilist.entry[fh].data.size() < offset){return an_err;}
  if(offset == ilist.entry[fh].data.size() -1){return 0;}
  
  if(check_permissions(fh,"write"))
  {
    int i = 0;
    for(i = 0; i < strlen(buf); i++)
    {
        if(ilist.entry[fh].data[offset+i] == '\0')
        {
        cout << "End of fh#" << fh << "reached, write complete" << endl;
        }
    
        ilist.entry[fh].data[i+offset] = buf[i];
    }
    return i;
  }
  else
  {
    uid_t cur_uid = geteuid();  
    cout << "Current user: " << getpwuid(cur_uid)->pw_name  
         << " does not have permission to write to this file.\n";
    return 0;
  } 
}  

// called at line #463 of bbfs.c
int my_statvfs(const char *fpath, struct statvfs *statv) {
  return an_err;  
}  

// called at line #530 of bbfs.c
int my_close( int fh ) {
    
  if ( fh > 2 )
  {
    cout << "Closing file, fh = " << fh << endl;
    //search open_file map for fh
    //returns map::end() if nothing found
    std::map<ino_t,int>::iterator search_result;
    search_result = open_files.find(fh);
    if(search_result  != open_files.end())
    {
        cout << "before close .at(fh): " << open_files.at(fh) << endl;
        open_files.at(fh) = open_files.at(fh) -1;
        cout << "after close .at(fh): " << open_files.at(fh) << endl;
    }
    else if(search_result == open_files.end())
    {
     cout << "File with fh: \"" << fh << "\" not found.\n";
     return an_err;
    }
    //if no more files open
    if(open_files.at(fh) <= 0)
    {
        //remove file from open_files map
        cout << "Removing file with fh: \"" << fh << "\" from open_files.\n";
        open_files.erase(search_result);
    }
    return 0;
  }
  return an_err;
}  

// called at line #553 of bbfs.c
int my_fdatasync( ino_t fh ) {
  return an_err;  
}  

// called at line #556 of bbfs.c
int my_fsync( ino_t fh ) {
  return an_err;  
}  

// called at line #575 of bbfs.c
int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t size, int flags ) {
  return an_err;  
}  

// called at line #592 of bbfs.c
int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size, int flags ) {
  return an_err;  
}  

// called at line #613 of bbfs.c
int my_llistxattr( const char *path, char *list, size_t size ) {
  return an_err;  
}  

// called at line #634 of bbfs.c
int my_lremovexattr( const char *path, const char *name ) {
  return an_err;  
}  


// called at line #826 of bbfs.c
//uses the process UID and GID to determine if user has access
//returns 0 on success. (All permissions granted)
//-1 otherwise
int my_access( const char *fpath, int mask ) {
    
     //uid_t     st_uid;
     /* TODO: Check file permissions 
     *  in order to determine if okay for other
     *  users with same GroupID allowed to alter file
     *  For now though, only concerned with UIDs as
     *  no way of really 'logging in' with a different GID
     *  Guess you could technically use chown to do this. 
     * Will implement later.
     * 
     *  NEED TO USE MASK SOMEHOW 
     * */
    
    // Payne uses these get the username / group name
    //getpwuid(st.st_uid)->pw_name,     // password name
    //getgrgid(st.st_gid)->gr_name,     // group name
    uid_t file_owneruid = 0;// ilist.entry[fh].metadata.st_uid;
    uid_t cur_uid = geteuid(); //Payne also uses this function when making files
    ino_t fh = find_ino(fpath);;
    if ( fh == 0 ) 
    {
        cout << "Filepath: \"" << fpath << "\" not found.\n";
        return an_err;
    } 
    else 
    {
        //for now just comparing uids
        uid_t file_owneruid = ilist.entry[fh].metadata.st_uid;
        if(file_owneruid == cur_uid)
        {
            cout << "Current user: " << getpwuid(cur_uid)->pw_name  
                 << " has access to " << fpath << endl;
            return 0;
        }
        else
        {
         cout << "Current user: " << getpwuid(cur_uid)->pw_name  
              << " does not have permissions for " << fpath << endl;
         cout << "User: " << getpwuid(file_owneruid)->pw_name << " has permissions." << endl;
          return an_err;  
        } 
    }  
    return an_err;  
}  

// called at line #856 of bbfs.c
int my_creat( const char *fpath, mode_t mode ) {
  // we can create a file by using the right flags to open
  // ADD: if file is already exists, check flags for append
  ino_t fh = find_ino(fpath);
  if(fh == 0)
  {
    cout << "file created: " << fpath << endl;
    return my_mknod(fpath, (S_IFREG | mode), 100 );
  }
  else
  {
      //ADD CHECK FOR FLAGS FOR APPEND  ABOVE THIS
      cout << "File \"" << fpath << "\" already exists." << endl;
      return 0;
  }
}  

// called at line #887 of bbfs.c
//int my_truncate(const char *path, off_t newsize)
//ftruncate checks if the file is open before attempting to truncate
//found in the manual of ftruncate
int my_ftruncate( ino_t fh, off_t offset ) {
    //ilist is a global
    //can access anywhere
    //ino_t fh = find_ino(path);
    
    //ilist.entry[fh].data.size() < offset
    /* TODO: check file permissions to see if can write to file 
     * */
    if ( fh < 2 || fh > ilist.count) 
    {
        cout << "Fh: \"" << fh << "\" cannot be truncated.\n";
        return an_err;
    }
    else if( open_files.find(fh) == open_files.end())
    {
        //check open files for file if not open then cannot truncate
        cout << "Fh: \"" << fh << "\" is not open. Cannot be truncated\n";
        return an_err;
    }
    else 
    {
     //exploit the fact that we have strings here
     //when the string is increased
     //the function sets the added characters to null
        ilist.entry[fh].data.resize(offset);   
        return 0;
    }  
  return an_err;  
}  

// called at line #921 of bbfs.c
int my_fstat( ino_t fh, struct stat* statbuf ) {

  //cdbg << "my_fstat has been called on " << fh << " " << long(statbuf) << endl;
  if ( ilist.entry.count(fh) == 0 ) {
    errno = ENOENT;
    return an_err;
  }  
  *statbuf = ilist.entry[fh].metadata;
  // cdbg << "and here's *statbuf for " << endl;
  // show_stat(*statbuf);
  return ok;
}  


// Here are my helper functions ==================================

// In the code below  opendir is defined in terms of find_ino,
// which is defined in terms of lookup,
// which is defined in terms of opendir and readdir.
// The header file takes care of that circularity.

// It doesn't say so anywhere that I can find, but a MY_DIR has to be
// similar to an open file.  However, rather than creating a stream of
// bytes (chars) from the file, it creates a stream of directory
// entries.  The key difference is how much you increment the offset
// counter each time you move to the next item.

// Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
// file handle into a DIR* via "(uintptr_t) fh."  But, his file handles
// are indices of byte streams, while our are the addresses of inodes.


MY_DIR* fopendir( ino_t fh ) {  // not exported
  if ( ! S_ISDIR( ilist.entry[fh].metadata.st_mode ) ) {
    return 0;  // null pointer indicates err
  }
  MY_DIR * tmp = new MY_DIR;
  // Initialize both members of tmp*
  tmp->fh    = fh;
  tmp->index = 0;
  return tmp;
}


// called at lines #707 and #726 of bbfs.c
dirent* my_readdir( MY_DIR* dirp ) {  
  vector<dirent_frame> v = ilist.entry[dirp->fh].dentries;
  int tmp = dirp->index++;  // post increment dirp*'s index.
  return tmp == v.size() ? 0 : & v[tmp].the_dirent;
}


// called at line #742 of bbfs.
int my_closedir( MY_DIR* dirp ) {
  delete dirp;  
}


ino_t lookup( string name, ino_t fh ) {
  // Searches for and returns ino of file of a given name within the 
  // directory given by fh.  This function will be used by find_ino().

  MY_DIR* dirp = fopendir( fh ); 
  // fopendir() will return 0 if fh isn't the handle of a directory.
  if ( ! dirp ) return 0;                   // And so will lookup().
  while ( dirent* dp = my_readdir(dirp) ) {  
    //cdbg << dp->d_name << " =? " <<name<< " then return " << dp->d_ino << endl;
    if ( string(dp->d_name) == name ) { // comparision of C++ strings
      my_closedir(dirp);
      return dp->d_ino;  
    } 
  }
  my_closedir(dirp);  // close MY_DIR asap, to reset internal data
  return 0;  // name-not-found
}  


ino_t find_ino( string path ) {

  //cdbg << "find_ino() has been called with path \"" << path << "\"\n"; 
  vector<string> v = split( path, "/" );  // The members of v are "segments."
  // Here are the exact details concerning path resolution:
  // http://manpages.ubuntu.com/manpages/lucid/en/man7/path_resolution.7.html

  // The plan: if the initial segment is not null prepend the full
  // pathname for the current working directory to path and start
  // over.  Process out segments of the form"." or ".." (along with
  // its predecessor).  Delete any other null segments.  Among other
  // things, we want the path name "/" to designate the root
  // directory.

  if ( v[0] != "" ) path = cwd + "/" + path;  // cwd == "" for now
  ino_t fh = 2;    // I've read that 2 is the ino of a filesystem's 
                   // root directory.  
  for ( vector<string>::iterator it = v.begin() ; it != v.end(); it++) {
    if ( (*it) == "" ) {
       continue;                  // ignore null segments.
    } else if ( ilist.entry.count(fh) == 0 ) {  // no such directory
      cdbg << "no such entry as " << *it << " in " << path << endl;
      errno = ENOENT;
      fh = 0;
      break;
    } else if ( S_ISDIR(ilist.entry[fh].metadata.st_mode) ) {
      //cdbg << "lookup(" << it << "," << fh << ") yields ...";
      fh = lookup( *it, fh );
      // cerr << "yields ... this value " << fh << endl;
    } else {
      cdbg << "In " << path <<  ", " << *it << " is not a directory \n";
      // errno = ENOTDIR;
      fh = 0;
      break;   
    }
    //cdbg << v[i] << " " << fh << end;
  } 
  // cdbg << "and is returning " << fh << endl;
  return fh;
} 


File* find_file( ino_t ino ) { // could improve readability of code
  return & ilist.entry[ino];
}


File* find_file( string s ) {  // could improve readability of code
  return find_file(find_ino(s));
}


// called at line #659 of bbfs.c
MY_DIR * my_opendir( const char path[PATH_MAX] ) {
  return fopendir( find_ino( path ) ); 
  // Note that fopendir checks whether its input is the handle of a
  // directory.
}


int describe_file( string pathname );

int ls(string path) {
  // diagnostic tool to see what's in a directory
  // cdbg << "ls has been called on \"" << path << "\" and ilist has "   
  // << ilist.entry.size() << " entries\n";
  ino_t fh = find_ino(path.c_str());
  if ( fh == 0 ) return an_err;
  File f = ilist.entry[fh];
  if ( ! S_ISDIR(f.metadata.st_mode) ) {
    errno = ENOTDIR;
    cdbg << "ilist entry " << fh << " isn't a directory: mode = " 
         << oct << f.metadata.st_mode << endl;
    //show_stat( f.metadata );
    describe_file(path);
    return ok;
  }
  vector<struct dirent_frame> v = f.dentries;
  //cdbg << "The directory (ino" << fh << ") has " << v.size() << " entries:\n";
  for ( vector<struct dirent_frame>::iterator it = v.begin(); it != v.end(); it++ ) {
    struct dirent d = (*it).the_dirent;
    //cout << d.d_ino << " " << d.d_name << endl;
    describe_file( path+"/"+d.d_name );
  }
}

// ===========================================


template< typename T1, typename T2>   // An idea
string pickle(map<T1,T2> m) 
{
  typename map<T1,T2>::iterator it;
  string s;
  stringstream ss(s);
  for (it = m.begin(); it != m.end(); it++) 
  {
    // This requires some kind of separation/terminaton symbol at the end of each.
    ss << it.first;  
    ss << it.second;
  }  
}  

  
// MY_DIR is typedef'd in my_stubs.H.  It's essentially an iterator for 
// directories.

// Per this GNU document (http://www.delorie.com/gnu/docs/dirent/dirent.5.html): 
// "The dirent structure is defined below.
// struct dirent {
//   long           d_ino;
//   off_t          d_off;
//   unsigned short d_reclen;
//   char d_name[1];
// };
// The field d_ino is a number which is unique for each file in the
// file system. The field d_off represents an offset of that directory
// entry in the actual file system directory. The field d_name is the
// beginning of the character array giving the name of the directory
// entry. This name is null terminated and may have at most NAME_MAX
// characters in addition to the null terminator. This results in file
// system independent directory entries being variable-length
// entities. The value of d_reclen is the record length of this
// entry. This length is defined to be the number of bytes between the
// beginning of the current entry and the next one, adjusted so that
// the next entry will start on a long boundary."

// But, per the relevant GNU manual pages on dirent,
// (http://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html),
// it may have many additional fields and even d_reclen is not
// required.  So far as I can tell, we don't need d_off and d_reclen
// but may need to maintain them for compatibility with FUSE, etc.

// Per a creator of FUSE (Millos Szeredit) regarding the offset field of dirents:
// http at fuse.996288.n3.nabble.com/issues-in-readdir-and-fuse-fill-dir-t-filler-td10397.html:

// "For directories it's completely irrelevant.  It can go forward,
// backward or jump around randomly.  Ancient unix systems allowed
// directories to be read as regular files (i.e. cat would work) and the
// offset was really an offset into this file.

// But now filesystems are free to implement directories in any way and
// the "offset" is not necessarily an offset any more, just a opaque
// cookie, unique for each directory entry within the directory,
// indicating the position within the directory stream.

// Thanks,
// Miklos "

// IMHO, this validates our use of maps for directories (thp).

// INODES: There is a good definition of struct inode at
// http://www.makelinux.net/books/lkd2/ch12lev1sec6.  But
// we are not using them.  Rather we are using struct stat
// for the metadata of a file.


// Here are details on lstat() and struct stat from
// http://linux.die.net/man/2/stat:

// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// These functions return information about a file. No permissions are
// required on the file itself, but-in the case of stat() and lstat()
// - execute (search) permission is required on all of the directories
// in path that lead to the file.

// stat() stats the file pointed to by path and fills in buf.

// lstat() is identical to stat(), except that if path is a symbolic
// link, then the link itself is stat-ed, not the file that it refers
// to.

// fstat() is identical to stat(), except that the file to be stat-ed
// is specified by the file descriptor fd.

// All of these system calls return a stat structure, which contains
// the following fields:

//     struct stat {
//         dev_t     st_dev;     /* ID of device containing file */
//         ino_t     st_ino;     /* inode number */
//         mode_t    st_mode;    /* protection */
//         nlink_t   st_nlink;   /* number of hard links */
//         uid_t     st_uid;     /* user ID of owner */
//         gid_t     st_gid;     /* group ID of owner */
//         dev_t     st_rdev;    /* device ID (if special file) */
//         off_t     st_size;    /* total size, in bytes */
//         blksize_t st_blksize; /* blocksize for file system I/O */
//         blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
//         time_t    st_atime;   /* time of last access */
//         time_t    st_mtime;   /* time of last modification */
//         time_t    st_ctime;   /* time of last status change */
//     };

// The st_dev field describes the device on which this file
// resides. (The major(3) and minor(3) macros may be useful to
// decompose the device ID in this field.)

// The st_rdev field describes the device that this file (inode) represents.

// The st_size field gives the size of the file (if it is a regular
// file or a symbolic link) in bytes. The size of a symbolic link is
// the length of the pathname it contains, without a terminating null
// byte.

// The st_blocks field indicates the number of blocks allocated to the
// file, 512-byte units. (This may be smaller than st_size/512 when
// the file has holes.)

// The st_blksize field gives the "preferred" blocksize for efficient
// file system I/O. (Writing to a file in smaller chunks may cause an
// inefficient read-modify-rewrite.)

// Not all of the Linux file systems implement all of the time
// fields. Some file system types allow mounting in such a way that
// file and/or directory accesses do not cause an update of the
// st_atime field. (See noatime, nodiratime, and relatime in mount(8),
// and related information in mount(2).) In addition, st_atime is not
// updated if a file is opened with the O_NOATIME; see open(2).

// The field st_atime is changed by file accesses, for example, by
// execve(2), mknod(2), pipe(2), utime(2) and read(2) (of more than
// zero bytes). Other routines, like mmap(2), may or may not update
// st_atime.

// The field st_mtime is changed by file modifications, for example,
// by mknod(2), truncate(2), utime(2) and write(2) (of more than zero
// bytes). Moreover, st_mtime of a directory is changed by the
// creation or deletion of files in that directory. The st_mtime field
// is not changed for changes in owner, group, hard link count, or
// mode.

// The field st_ctime is changed by writing or by setting inode
// information (i.e., owner, group, link count, mode, etc.).

// The following POSIX macros are defined to check the file type using
// the st_mode field:

//     S_ISREG(m)

//     is it a regular file?

//     S_ISDIR(m)

//     directory?

//     S_ISCHR(m)

//     character device?

//     S_ISBLK(m)

//     block device?

//     S_ISFIFO(m)

//     FIFO (named pipe)?

//     S_ISLNK(m)

//     symbolic link? (Not in POSIX.1-1996.)

//     S_ISSOCK(m)

//     socket? (Not in POSIX.1-1996.) 
// The following flags are defined for the st_mode field:
// The set-group-ID bit (S_ISGID) has several special uses. For a
// directory it indicates that BSD semantics is to be used for that
// directory: files created there inherit their group ID from the
// directory, not from the effective group ID of the creating process,
// and directories created there will also get the S_ISGID bit
// set. For a file that does not have the group execution bit
// (S_IXGRP) set, the set-group-ID bit indicates mandatory file/record
// locking.

// The sticky bit (S_ISVTX) on a directory means that a file in that
// directory can be renamed or deleted only by the owner of the file,
// by the owner of the directory, and by a privileged process.
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



// The following gives information on configuring the root directory:
// "The root directory has a '.' and a '..' entry in it, and the inode
// number for each is the same. Traditionally, the inode number is 2;
// it still is on MacOS X, Linux, Solaris. So, when you do 'cd /; cd
// ..', you end up at the same location.

// In the 1980s, there was a system called Newcastle Connection that
// treated networked computers as being above the root of your local
// computer. Thus, on such a machine, you would type:

// cd /../othermachine/path/to/interesting/place

// to change directory to a remote file system.

// (You can find the paper via a Google search of 'Newcastle
// Connection' - the URL is intractable.)"
// See http://superuser.com/questions/186184/the-parent-of-the-root-directory

     // struct stat {
     //     unsigned int  st_dev;      /* ID of device containing file */
     //     unsigned int  st_ino;      /* inode number */
     //     mode_t        st_mode;     /* type and protection of file */
     //     unsigned int  st_nlink;    /* number of hard links */
     //     unsigned int  st_uid;      /* user ID of owner */
     //     unsigned int  st_gid;      /* group ID of owner */
     //     unsigned int  st_rdev;     /* device type (if inode device) */
     //     unsigned long st_size;     /* total size, in bytes */
     //     unsigned long st_blksize;  /* blocksize for filesystem I/O */
     //     unsigned long st_blocks;   /* number of 512B blocks allocated */
     //     time_t        st_atime;    /* time of last access */
     //     time_t        st_mtime;    /* time of last modification */
     //     time_t        st_ctime;    /* time of last change */
     // };
// Note that this shows the real types of all inode components except
// mode_t and time_t.
// for moredetails see: 
//   http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html

// Under development: Function to format ls lines in format of "ls -d"
int describe_file( string pathname ) {
  //string stream ccout;
  struct stat st;

  //struct stat st;       // "struct stat" because stat() is defined
  if ( my_lstat( pathname.c_str(), &st ) != 0 ) {
    errno = ENOENT;
    cdbg << "Cannot stat file " << pathname
         << ": " << strerror(errno) << endl;
    return -1;
  }

  cout << st.st_ino << ": "
       << ( (S_ISDIR(st.st_mode) != 0) ? 'd' : '-' )
       << ( (st.st_mode & S_IRUSR) ? 'r' : '-' )
       << ( (st.st_mode & S_IWUSR) ? 'w' : '-' )
       << ( (st.st_mode & S_IXUSR) ? 'x' : '-' )
       << ( (st.st_mode & S_IRGRP) ? 'r' : '-' )
       << ( (st.st_mode & S_IWGRP) ? 'w' : '-' )
       << ( (st.st_mode & S_IXGRP) ? 'x' : '-' )
       << ( (st.st_mode & S_IROTH) ? 'r' : '-' )
       << ( (st.st_mode & S_IWOTH) ? 'w' : '-' )
       << ( (st.st_mode & S_IXOTH) ? 'x' : '-' )
  ;

  char date[64];
  strftime( date, 15, "%b %d %H:%M  ", localtime( &st.st_mtime ) );

  printf( 
    "%2i %7s %7s %8ld %8s ",          // format string
    st.st_nlink,                      // number of links
    getpwuid(st.st_uid)->pw_name,     // password name
    getgrgid(st.st_gid)->gr_name,     // group name
    st.st_size,                       // size of file
    date                              // time of last modification
  );
  vector<string> v = split(pathname, "/");
  cout << v.back() << endl;  
}



// man readdir(3), lstat, opendir, closedir.

// Format of a directory entry: 
// http://www.delorie.com/gnu/docs/glibc/libc_270.html

// Testing the type of a file: 
// http://www.delorie.com/gnu/docs/glibc/libc_286.html
// (Test the st_mode field returned by stat on a given file.)

// To get Linux to use ascii ordering "export LANG=us.ascii"

// handy macro for iterating through an stl container
#define each(I) \
  for( auto it=(I).begin(); it!=(I).end(); ++it )


int visit( string root ) { // recursive visitor function, implements lslr

  // OPEN root
  MY_DIR* dirp;                                          // open DIR
  //if ( ! ( dirp = my_opendir( root.c_str() ) ) ) {
  if ( ! ( dirp = my_opendir( root.c_str() ) ) ) {
    cerr << "Cannot open directory " << root << ".\n";
    return -1;
  }

  // CREATE TWO LISTS OF FILE NAMES: file and hardSubdirectory
  list<string> file;          // names of each file in this dirctory
  list<string> hardSubdirectory;    // each hard-linked subdirectory
  list<string>::iterator it;
  while ( dirent* dp = my_readdir(dirp) ) {
    string s = dp->d_name;        // converts C string to C++ string
    if ( s == "." || s == ".." ) continue;             // skip these
    s = root + ( root[root.size() - 1] == '/'? "" : "/") + s;  // prepend the current path
    file.push_back( s ); 
    struct stat st;
    cdbg << "d_ino = " << dp->d_ino << endl;
    int error = my_fstat(dp->d_ino, &st);
    //show_stat(st);
   
    cdbg << "Considering " 
         << s << dp->d_ino << " of mode " 
         << oct << st.st_mode << " for hard directory.\n";
    if ( S_ISDIR(st.st_mode) ) {
    //if ( ( dp->d_type & DT_DIR ) && !(dp->d_type & DT_LNK) ) {
      hardSubdirectory.push_back( s );
    }
  }
  my_closedir(dirp);         // close DIR asap, to reset internal data
  cdbg << "hardSubdirectory has " << hardSubdirectory.size() << " entries.\n";
  for(  it = hardSubdirectory.begin(); it != hardSubdirectory.end(); it++) {cdbg << *it << endl;}

  // EMIT root's HEADER, INCLUDING ITS TOTAL SIZE
  cout << root << ":" << endl;
  cout << "total ";
  int size = 0;
  for(it = file.begin(); it != file.end(); it++ ) {
    string filename = *it;
    struct stat st;
    if ( my_lstat( filename.c_str(), &st ) == 0 ) size += st.st_blocks;
  }
  cout << size/2 << endl;    // kilobytes-per-block correcton factor

  // lstat() AND REPORT ON EACH FILE WHOSE NAME IS IN root
  file.sort();  
  for(it = file.begin(); it != file.end(); it++ ) {
    string filename = *it;    

    struct stat st;       // "struct stat" because stat() is defined
    if ( my_lstat( filename.c_str(), &st ) != 0 ) {
      cerr << "Cannot stat file " << filename 
           << ": " << strerror(errno) << endl;
      return -1;
    }

    cout << st.st_ino << ": "     // added for CS179F
         << ( (S_ISDIR(st.st_mode) != 0) ? 'd' : '-' )
         << ( (st.st_mode & S_IRUSR) ? 'r' : '-' )
         << ( (st.st_mode & S_IWUSR) ? 'w' : '-' )
         << ( (st.st_mode & S_IXUSR) ? 'x' : '-' )
         << ( (st.st_mode & S_IRGRP) ? 'r' : '-' )
         << ( (st.st_mode & S_IWGRP) ? 'w' : '-' )
         << ( (st.st_mode & S_IXGRP) ? 'x' : '-' )
         << ( (st.st_mode & S_IROTH) ? 'r' : '-' )
         << ( (st.st_mode & S_IWOTH) ? 'w' : '-' )
         << ( (st.st_mode & S_IXOTH) ? 'x' : '-' )
    ;

    char date[64];
    strftime( date, 15, "%b %d %H:%M  ", localtime( &st.st_mtime ) );

    printf( 
      "%2i %7s %7s %8ld %8s ",          // format string
      st.st_nlink,                      // number of links
      getpwuid(st.st_uid)->pw_name,     // password name
      getgrgid(st.st_gid)->gr_name,     // group name
      st.st_size,                       // size of file
      date                              // time of last modification
    );

    cout << *it << endl;

  }

  // RECURSE THROUGH root's HARD-LINKED SUBDIRECTORIES AND RETURN
  for(it = hardSubdirectory.begin(); it != hardSubdirectory.end(); it++ ) {
    cout << endl;
    visit( *it );   
  }

  return 0;                                        // return success

}


// int main( int argc, char* argv[] ) { 
//   return visit( argc > 1 ? argv[1] : "." ); 
// }




int main(int argc, char* argv[] ) {
  int cwd = 2;  // ino of current working directory;
  // The place for testing of functions.
  // cdbg << "Now we call initialize()" << endl;
  initialize();
  stringstream record;
  ifstream myin;
  
  if ( argc ){myin.open( argv[1] );}
  
  // Idiom for infinite loop
  for(;;) 
  { 
    string op, file;
    // if ( myin.eof() ) exit(0);
    cout << "Which op and file? " << endl;
    (myin.good() ? myin : cin) >> op >> file;
    if ( op != "exit" ){record << op << " " << file << endl;}
    if (op == "?" || op == "help") //List available operations
    {
          //~ cout << "Operations: \n"; 
          //~ cout << "\t
          //~ cout << "\t
          //~ cout << "\t
          //~ cout << "\t
          //~ cout << "\t 
    }
    else if (op == "play"  ) // accepts input from file instead of keyboard
    {
        ifstream in;
        string playFile;
        cout << "Enter the name of the file to play: ";
        cin >> playFile;
        
   
        
        
    } 
    else if (op == "save"  ) // saves dialog to specified file
    {
    } 
    else if (op == "mkdir" ) 
    { // prompts for protection mode
      cout << "Specify file permissions in octal: ";
      mode_t mode; 
      // cin >> oct >> mode;
      (myin.good()? myin : cin) >> oct >> mode;
      record << oct << mode << endl;
      my_mkdir(file.c_str(), mode );
    } 
    else if (op == "rmdir"  ) // shows file's metadata
    { 
      my_rmdir(file.c_str() );
    } 
    else if (op == "show"  ) // shows file's metadata
    { 
      show_stat( ilist.entry[ int(find_ino(file)) ].metadata );
    } 
    else if (op == "ls"  ) // lists the specified directory.
    {  
      ls(file);
    } 
    else if (op == "lstat"  ) // lists the specified directory.
    {  
      struct stat a_stat;
      my_lstat(file.c_str(), &a_stat);
      show_stat(a_stat);
    } 
    else if (op == "exit"  ) 
    { // quits 
      // save dialog so far to specified file.
      ofstream myfile;
      myfile.open(file.c_str());
      myfile << record.str();
      myfile.close();
      //check if buf has anything
        // if it does delete it
      return 0;
    } 
    else if (op == "break"  ) // executes the rest of main()
    { 
      break;
    } 
    else if (op == "lslr"  ) // executes visit()
    {
      visit(file);
    }
    else if (op == "write")
    {
      int fh = my_open(file.c_str(), O_WRONLY);
      
      size_t num_bytes = 0;
      off_t offset = 0;
      string my_str;

      cout << "Enter string to write: ";
        cin.ignore();
        getline(cin, my_str);
        const char * buf = my_str.c_str();
        num_bytes = strlen(buf);     
       
        cout << "Enter offset: ";
        (myin.good()? myin : cin) >> dec >> offset;
        
        int status = my_pwrite(fh, buf, num_bytes, offset);
        cout << endl << "Number of bytes written: " << status << endl;
      
    }
    else if (op == "read") 
    {
        int fh = my_open(file.c_str(), O_RDONLY);
        size_t num_bytes = 0;
        off_t offset = 0;
        //int num_bytes;
        //int offset;

        cout << "Enter number of bytes to read: ";
            //getline(cin, num_bytes);
        //cin >> num_bytes;
        (myin.good()? myin : cin) >> dec >>  num_bytes;
        cout << "Enter offset: " ;
        (myin.good()? myin : cin) >> dec >> offset;
        //cin >> offset;
        //getline(cin, offset);
        char * buf;
        //remember to delete at some point
        buf = new char[num_bytes]();
        cout << "num_bytes: " << num_bytes << "offset: " << offset << endl; 
        int status = my_pread(fh, buf, num_bytes, offset);
        cout << endl << "Number of bytes read " << status << endl;
        cout << buf << endl;
    }
    else if (op == "creat" || op == "create") 
    {
        cout << "Specify file permissions in octal: ";
        mode_t mode; 
        (myin.good()? myin : cin) >> oct >> mode;
        record << oct << mode << endl;
        my_creat( file.c_str(), mode );
    } 
    else if (op == "open")
    {
        // O_RDONLY for testing, should be changed depending on users permissions
        my_open( file.c_str(), O_RDONLY); 
    }
    else if(op == "close")
    {
      int fh = 0;
      cout << "Enter fh to close: ";
      cin >> fh;
      int close_result  = my_close(fh);
      if (close_result == 0)
      {
          cout << "Successfully closed handle.";
      }
      else if (close_result == an_err)
      {
          cout << "Error: Unable to close file handle." << endl;
      }
    }
    else if(op == "chmod")
    {
        cout << "Specify new file permissions in octal: ";
        mode_t mode; 
        (myin.good()? myin : cin) >> oct >> mode;
        record << oct << mode << endl;
        my_chmod(file.c_str(), mode);
    }
    else if (op == "access")
    {
        cout << "Specify mask:" ;
        mode_t mode; 
        (myin.good()? myin : cin) >> oct >> mode;
        record << oct << mode << endl;
        my_access(file.c_str(), mode);
    }
    else if (op == "chown")
    {
        uid_t new_uid = 0;
        gid_t new_gid = 0;
        cout << "Enter new file owner uid: ";
        cin >> dec >> new_uid;
        cout << "Enter new file group id (0 if no change):";
        cin >> dec >> new_gid;
        //const char path, uid_t uid, gid_t gid  
        my_chown(file.c_str(), new_uid, new_gid);
    }
    else if (op == "rename")
    {
       string new_fname;
       cout << "Enter new file name: " ;
       cin >> new_fname;
       my_rename(file.c_str(), new_fname.c_str());
    }
    else if (op == "unlink")
    {
      //~ string fileName;
      //~ cout << "Enter file name to unlink: ";
      //~ cin >> fileName;
      //Success
      if(my_unlink(file.c_str()) == 0)
      {
        cout << "Unlink successful!\n";
      }
      else
      {
        cout << "Unlink unsucessful!\n";
      }
    }    
    else if (op == "truncate")
    {
         off_t new_file_size;
         //int my_truncate(const char *path, off_t newsize)
         cout << "Enter new file size: " ;
         cin >> dec >> new_file_size;
         my_truncate(file.c_str(), new_file_size);
    }
    else if (op == "ftruncate")
    {
        //int my_ftruncate( ino_t fh, off_t offset ) {
         off_t new_file_size;
         ino_t fh;
         cout << "Enter fh of file: " ;
         cin >> dec >> fh;
         cout << "Enter new file size: " ;
         cin >> dec >> new_file_size;
         my_ftruncate(fh, new_file_size);
    }
    else 
    {
      cout << "Correct usage is: op pathname,\n"; 
      cout << "where \"op\" is one of the following:\n";
      cout << "help, play, save, mkdir, show, break, lslr, exit.\n";
      cout << "For example, type \"exit now\" to exit.\n";
    }
  }  
  // Continuation of main(), which is reacable via the "break" op.
  show_stat( ilist.entry[2].metadata );  // all looks good here.
  cdbg << "Now we call lstat on \"/\" and &mystat" << endl;
  struct stat mystat;
  my_lstat("/",&mystat);
  cdbg << "Now we call ls on \"/\"" << endl;
  ls("/"); 
  cdbg << "Now we call mkdir on \"/junk\" and 0700" << endl;
  my_mkdir("/junk", 0700);
  cdbg << "Now we call ls on \"/\"" << endl;
  ls("/");

  cdbg << "now we call ls on \"/junk\"" << endl;
  //  cout << endl;
  ls("/junk");
  
  cdbg << "Now we call mkdir on \"/junk/stuff\" and 0700" << endl;
  my_mkdir("/junk/stuff", 700);
  cdbg << "now we call ls on \"/junk\"" << endl;
  ls("/junk");
  cdbg << "now we call ls on \"/junk/stuff\"" << endl;
  ls("/junk/stuff");

  cout << endl;
  describe_file( "/junk" );
  cout << endl;
}

