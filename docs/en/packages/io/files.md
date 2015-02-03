# Files & Folders #

Files and folders are the staple of any filesystem and are represented by the `Titon\Io\File` and `Titon\Io\Folder` 
classes. The IO classes provide read, write, delete, and management functionality for the local filesystem.

```hack
$file = new Titon\Io\File('/path/to/file');
$folder = new Titon\Io\Folder('/path/to/folder');
```

## Usage ##

Once an object is created, you can check if the file exists at the given path using `exists()`.

```hack
$folder->exists(); // false
```

### Creating & Deleting ###

Passing a boolean `true` as the 2nd argument to the constructor will automatically create the file if it does not exist. 
A permission mode can also be set as the 3rd argument.

```hack
$file = new File('/path/to/file', true);
$folder = new Folder('/path/to/folder', true, 0777);
```

If not using the constructor, use `create()`, which will return a boolean `true` on successful creation.  
This method will only create the file if it doesn't already exist. As well, an optional permission mode can be 
passed (defaults to `0755`).

```hack
$file->create();
$folder->create(0777);
```

To delete the file, use `delete()`, which will return a boolean `true` on successful deletion. 

```hack
$file->delete();
$folder->delete();
```

<div class="notice is-warning">
    <b>Beware!</b> The <code>Folder</code> is atomic and will attempt to recursively delete all children 
    before deleting itself.
</div>

### Copying, Moving & Renaming ###

Copying files can be tricky as you must validate the source and the destination. When copying with `copy()`, 
a destination path must be defined, and the type of operation to use. The following operations are supported:

* `OVERWRITE` - Will overwrite the destination file if one exists (file and folder)
* `MERGE` - Will merge folders together if they exist at the same location (folder only)
* `SKIP` - Will not overwrite the destination file if it exists (file and folder)

An optional permission mode can be set as the 3rd argument for the newly copied file.

```hack
$file->copy('/to/new/file', File::OVERWRITE);
$folder->copy('/to/new/folder', Folder::MERGE, 0755);
```

Moving works in the same way as copying, but is much simpler. Simply use `move()` and pass the path to move to 
and a boolean on whether to overwrite the destination file if one exists (defaults `true`).

```hack
$file->move('/to/new/path');
$folder->move('/to/new/path', false);
```

Moving will only move a file, it will not rename a file. To rename a file in place, use `rename()`. 
This method requires a new name and a boolean on whether to overwrite the destination file (defaults `true`).

```hack
$file->rename('new-file-name.' . $file->ext());
$folder->rename('new-folder-name', false);
```

<div class="notice is-warning">
    Renaming does not handle file extensions. This should be handled manually.
</div>

### Informational ###

To access a file or folders name use `basename()`, or a file name without an extension use `name()`, 
or the extension itself use `ext()` (files only).

```hack
$file->basename(); // image.jpg
$file->name(); // image
$file->ext(); // jpg
```

To access the group, owner, or parent, use `group()`, `owner()`, and `parent()` respectively. The `parent()` 
method will return a new `Folder` instance for the folder that the file/folder belongs in. If there is no parent 
(at the root), `null` will be returned.

```hack
$file->group();
$file->owner();
$file->parent(); // Titon\File\Folder
```

To get the size of a file, or the number of children within a folder, use `size()`.

```hack
$file->size();
$folder->size();
```

### Timestamps ###

To access timestamps, like access, modified, and changed, use `accessTime()`, `modifyTime()`, and `changeTime()`.

```hack
$file->accessTime();
$file->modifyTime();
$file->changeTime();
```

### Permissions ###

To modify the group, owner, and mode, use `chgrp()`, `chown()`, and `chmod()` (requires an octal value).

```hack
$file->chgrp(8);
$file->chown(3);
$file->chmod(0777);
```

To modify permissions on all files within a folder, set the recursive 2nd argument to `true`.

```hack
$folder->chmod(0777, true);
```

The `permissions()` method will return the literal read-write-execute octal value.

```hack
$file->permissions(); // 0777
```

And to check if a file is `executable()`, `readable()`, and `writable()`.

```hack
$file->executable(); // true
```

### Paths ###

The `path()` method will return the current path while `dir()` will return the parent folders path.

```hack
$file->path();
$file->dir();
```

To check if a path is relative or absolute, use `isRelative()` or `isAbsolute()`.

```hack
$file->isRelative(); // true
```

## Files ##

`Titon\Io\File`s provide read, write, and lock functionality alongside the utility methods `mimeType()`, 
which attempts to guess the files type, and `md5()`, which returns an MD5 hash of the file.

```hack
$file->mimeType(); // text/html
$file->md5();
```

### Reading ###

The `read()` method will attempt to read the contents of the current file. An optional limit can be passed to return all 
bytes up until that point. 

```hack
$content = $file->read();
$content = $file->read(1234);
```

This method will lock the file until the contents have been read.

### Writing ###

The `write()` method will write content to the current file. The default write [mode](http://php.net/manual/en/function.fopen.php) 
is `w`, which can be changed using the 2nd argument.

```hack
$file->write($content);
$file->write($content, 'w+');
```

The previous method will truncate the file before writing to it. To append to the file instead, use `append()`.

```hack
$file->append('foo');
$file->append('bar');
```

The reverse can also be used, `prepend()`, which will write content to the beginning of the file.

```hack
$file->prepend('foo');
$file->prepend('bar');
```

When appending or prepending, the file resource is not automatically closed and will require manual closing.

```hack
$file->close();
```

These methods will also lock the file until the content has been written.

### Locking ###

We've mentioned locking previously, as it automatically occurs during reads and writes, but if you need to implement 
locking manually, the `lock()` and `unlock()` methods are available.

```hack
$file->lock();
// Do something
$file->unlock(); // or $file->close()
```

## Folders ##

`Titon\Io\Folder`s provide management of files and folders on the filesystem.

### Reading ###

To gather the child nodes within a folder, the `files()` method will return a list of `File`s,
and `folders()` will return a list of `Folder`s. Lists can be sorted by passing `true` as the 1st argument, 
and gathering via recursion is also possible by passing `true` as the 2nd argument.

```hack
$folder->files(); // Vector<File>
$folder->folders(); // Vector<Folder>
```

To gather a list of both files and folders, use `read()`, which accepts the same arguments.

```hack
$folder->read(); // Vector<Node>
```

### Finding ###

The `find()` method will attempt to find (glob) files or folders that match a specific pattern. 
An optional filter can be passed as the 2nd argument to return either files or folders.

```hack
$folder->find('foo.*'); // Vector<Node>
$folder->find('*.html', Folder::FILES); // Vector<File>
```

The available filters are:

* `ALL` - Returns both files and folders
* `FILES` - Returns only files
* `FOLDERS` - Returns only folders

### Cleaning ###

To delete all files within the folder but not the folder itself, use `flush()`.
