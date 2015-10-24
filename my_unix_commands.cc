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

class my_DIR {
  public:
    dirent* base;
    dirent* offset;
    ino_t fh;
    dirent* max_offset;
    my_DIR( ino_t x )
      : fh(x), max_offset((dirent*)ilist[fh].i_size)
    {}
};

int ok = 0;
int err = -1;

int my_lstat(const char* path, struct stat *statbuf){
  // ino_t fh = find_inode(path);
  //
}



// called at line #241 of bbfs.c
int my_symlink(const char *path, const char *link) {
return err;
}
// called at line #261 of bbfs.c
int my_rename( const char *path, const char *newpath ) {
return err;
}

119
120
121
122
123

// called at line #279 of bbfs.c
int my_link(const char *path, const char *newpath) {
return err;
}

124
125
126
127
128

// called at line #296 of bbfs.c
int my_chmod(const char *path, mode_t mode) {
return err;
}

129
130

133

// called at line #314 of bbfs.c
int my_chown(const char *path, uid_t uid, gid_t gid) {
return err;
}

134
135
136
137
138

// called at line #331 of bbfs.c
int my_truncate(const char *path, off_t newsize) {
return err;
}

139
140
141
142
143

71
73

111

132

class my_DIR { // This is my version of DIR
public:
dirent* base; // not a pointer, rather an off_t
dirent* offset;
ino_t fh;
dirent* max_offset; // we need to initialize this.
my_DIR( ino_t x )
: fh(x), max_offset((dirent*)ilist[fh].i_size)
{}
};

70
72

110

131

59
61

// called at line #220 of bbfs.c
int my_rmdir( const char *path ) {
return err;
}

109

115

inline // a simple utility for splitting strings at a find−pattern.
vector<string>
split(const string s, const string pat ) {
string c;
vector<string> v;
int i = 0;
for (;;) {
int t = s.find(pat,i);
int j = ( t == string::npos ) ? s.size() : t;
string c = s.substr(i,j−i);
v.push_back(c);
i = j+pat.size();
if ( t == string::npos ) return v;
}
}

58
60

// called at line #203 of bbfs.c
int my_unlink( const char *path ) {
return err;
}

114

42
44

108

112

41
43

// called at line #186 of bbfs.c
int my_mkdir( const char *path, mode_t mode ) {
return err;
}

104

using namespace std;

107

38

// called at line #168 of bbfs.c. See line #151.
int my_mknod( const char *path, mode_t mode, dev_t dev ) {
// returns error status only. To get the handle for the inode
// simply search to the dirent via path.
return err;
}

99

106

37

// called at line #125 of bbfs.c
int my_readlink( const char *path, char *link, size_t size ) {
return err;
}

94

</Users/thp/Documents/courses/CS179F/fs.h>
<sys/stat.h> // this has our official definition of stat
<dirent.h>
// this has our official definition of dirent
<errno.h>
<vector>
<string>
<cassert>
<sstream>
<map>

34
35

// called at line #95 of bbfs.c
// for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
int my_lstat( const char* path, struct stat *statbuf ) {
// ino_t fh = find_inode(path);
// collect attributes from ilist[fh] load into struct stat.
return err;
}

87

33

36

Page 2/6

82

93

// Mine
#include
#include
#include
#include
#include
#include
#include
#include
#include

31
32

my_stubs.cc

74

2
3

Oct 23, 15 14:52

// called at line #349 of bbfs.c
int my_utime(const char *path, struct utimbuf *ubuf) {
return err;
}

144

int ok = 0;
int err = −1;

Friday October 23, 2015

145
146

my_stubs.cc

// called at line #376 of bbfs.c. Returns file handle not fd
int open( const char *path, int flags ) {

1/3

Printed by Tom Payne
Oct 23, 15 14:52
148
149
150
151
152
153
154
155
156
157
158

my_stubs.cc

Page 3/6

// Write a function called "lookup" that takes a name and the handle
// of a directory and returns the handle part of the entry having that
// name.
// Assume that path is a full path. Split path. Starting with the
// handle of the root directory, look up each segment in the directory
// whose handle you currently have. F
// directory, lookup each segment.
// Use a linear search that consists of successive calls to readdir to
// find the corresponding directory entry, which has type struct dirent.
// Return its d_fileno, or −1 if error.
return err;

147

218

161
162
163

220
221
222

166
167
168

224
225
226
227
228

}

229

171
172
173

231

176
177
178

233
234
235

// called at line #439 of bbfs.c
int my_pwrite( int fh, char *buf, size_t size, off_t offset ) {
return err;
}

236

181
182
183

238
239
240

// called at line #463 of bbfs.c
int my_statvfs(const char *fpath, struct statvfs *statv) {
return err;
}

241

186
187
188

243
244
245

// called at line #530 of bbfs.c
int my_close( ino_t fh ) {
return err;
}

246

191
192
193

248
249
250

// called at line #553 of bbfs.c
int my_fdatasync( ino_t fh ) {
return err;
}

251

196
197
198

253
254
255

// called at line #556 of bbfs.c
int my_fsync( ino_t fh ) {
return err;
}

256

200

258
259

// called at line #575 of bbfs.c
int my_lsetxattr( const char *fpath, const char *name, const char *value, size_t
size, int flags ) {
return err;
}

201
202
203

206
207
208

215
216
217

263
264
265
267

// called at line #921 of bbfs.c
// for details see: http://manpages.ubuntu.com/manpages/hardy/man2/stat.2.html
int my_fstat( ino_t fh, struct stat* statbuf ) {
return err;
}

268
269
270
271
273

// called at line #1015 of bbfs.c
char* my_realpath( const char* path, char* resolved_path ) {
assert( resolved_path != NULL );
return 0;
}

274
275
276
278
279
280
281
283

// In the next three functions, we need to create a my_DIR* from a file
// handle, i.e., an (ino_t) fh. So, how to do that?

// Here are my helper functions ==================================
//
//
//
//

In the code below opendir is defined in terms of find_inode,
which is defined in terms of lookup,
which is defined in terms of opendir and readdir.
The header file takes care of that circularity.

typedef char block[4096];

284
285

block* blocks[15];

286

213
214

// called at line #887 of bbfs.c
int my_ftruncate( ino_t fh, off_t offset ) {
return err;
}

282

210
212

// called at line #856 of bbfs.c
int my_creat( const char *fpath, mode_t mode ) {
return err;
}

277

// called at line #634 of bbfs.c
int my_lremovexattr( const char *path, const char *name ) {
return err;
}

209
211

// called at line #826 of bbfs.c
int my_access( const char *fpath, int mask ) {
return err;
}

262

272

204
205

261

266

// called at line #592 of bbfs.c
int my_lgetxattr( const char *fpath, const char *name, char *value, size_t size,
int flags ) {
return err;
}
// called at line #613 of bbfs.c
int my_llistxattr( const char *path, char *list, size_t size ) {
return err;
}

// // called at line #742 of bbfs.c
// int my_closedir( my_DIR* dp ) {
//
return err;
// }

257

260

199

// // called at line #707 of bbfs.c
// int my_readdir( my_DIR* dp ) {
//
return err;
// }

252

194
195

// // called at line #659 of bbfs.c
// my_DIR* my_opendir( const char* fpath ) {
//
return err;
// }

247

189
190

#define my_DIR_PTR (uniptr_t)

242

184
185

I recommend that we simply maintain the counter as an
directory−entry index and increment it by one each time. Then
multiply it by the size of a directory entry to get the offset of
the directory entry that it refers to within the directory. To get
the address of that directory entry, we simply add the address of
the corresponding block of the directory.

237

179
180

//
//
//
//
//
//

232

174
175

// Note that at line #742 of bbfs.c, Professor Pfeiffer converts a
// file handle into a my_DIR via "(uintptr_t) fh." But, his file handles
// are indices of byte streams, while our are the addresses of inodes.

230

// called at line #411 of bbfs.c
int my_pread( int fh, char *buf, size_t size, off_t offset ) {
return err;
}

169
170

Page 4/6

223

164
165

my_stubs.cc

// counter each time you move to the next item.

219

159
160

Oct 23, 15 14:52

//
//
//
//

It doesn’t say this anywhere that I can find, but a my_DIR has to be
similar to an open file. However, rather than creating a stream of
bytes (chars) from the file, it creates a stream of directory
entries. The key difference is how much you increment the offset

Friday October 23, 2015

287
288
289
290

my_stubs.cc

my_DIR* fhopendir( ino_t fh ) {
// if ( fh is not the handle of a directory ) return not−a−directory error;
return new my_DIR( fh );
}

2/3

Printed by Tom Payne

my_stubs.cc

Oct 23, 15 14:52

Page 5/6

291

364

292
293

366
367

off_t tmp = (dirp−>offset)−>d_reclen;
dirp−>offset += tmp;
return ( dirp−>offset < dirp−>max_offset) ? dirp−>offset : 0 ;

295
296
297

368
369
370

}

371

299

372

300
301
302
303

373

int closedir( my_DIR* dirp ) {
delete dirp;
}

374

377
378

308
309
310
311
312
313
314
315
316
317
318
319
320
321
322
323
324
325

class my_dir { // C++ verion of DIR, i.e., an opened directory
directory d;
map<string,file>::iterator it; // readdir must post−increment it.
my_dir( directory d )
: d(d)
{
it = d.themap.begin();
}
};

376

305
307

Page 6/6

375

304
306

my_stubs.cc

};

365

dirent* readdir( my_DIR* dirp ) {

294

298

Oct 23, 15 14:52

ino_t lookup( string name, ino_t fh ) {
// finds and returns ino_t of file whose name is name in directory fh.
// This function will be used by open() and opendir().
my_DIR* dirp = fhopendir( fh ); // fhopendir checks if fh is handle of a dir.
if ( ! dirp ) return err;
// cannot_open_error
while ( dirent* dp = readdir(dirp) ) {
string s = dp−>d_name;
// converts C string to C++ string
if ( s == name ) {
closedir(dirp);
if ( dp−>d_type != DT_REG && dp−>d_type != DT_DIR ) {
return err;
// wrong−file−type error
// later we may add more types
} else {
return dp−>d_fileno;
}
}
}
closedir(dirp);
// close my_DIR asap, to reset internal data
return err; // name−not−found
}

379
380
381
382
383
384
385
386
387

template< typename T1, typename T2>
string pickle(map<T1,T2> m) {
typename map<T1,T2>::iterator it;
string s;
stringstream ss(s);
for ( it = m.begin(); it != m.end(); ++it ) {
// This requires some kind of separation/terminaton symbol at the end of eac
h.
ss << it.first;
ss << it.second;
}
}

388
389

326
327
328
329
330
331
332
333
334
335

ino_t find_inode( string fullpath ) {
// fullpath must starts at the root. // See how Pfeiffer does it.
ino_t fh = 2; // 2 is the i_no of the root directory.
vector<string> v = split( fullpath, "/" );
for ( int i = 1; i != v.size(); ++ i ) { // omit the initial null segment
fh = lookup( v[i], fh );
}
}

336
337
338
339
340

struct inode * find_inode( ino_t ) {
// the actual code for this will depend on how you implement this map
return 0;
}

341
342
343
344

my_DIR* opendir( char* fullpath ) {
return fhopendir( find_inode( fullpath ) );
}

345
346

// ===================== a crude c++ approach ======================

347
348
349
350
351

class file {
public:
inode ino;
};

352
353
354
355

class regfile : file {
string content;
};

356
357
358
359

class openfile : regfile {
stringstream(content);
};

360
361
362
363

class directory : file {
public:
map<string,file> themap;

Friday October 23, 2015

// a balanced binary tree.

my_stubs.cc

3/3


