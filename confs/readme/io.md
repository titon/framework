# IO #

Provides classes that deal with input and output of data on the file system.
`File` and `Folder` classes can be used to read, write, alter permissions, rename,
copy, create, delete, move, traverse, and many more through an easy to use interface.

```hack
$file = new Titon\Io\File('/path/to/file.ext', true);
$file->write($data);

$folder = new Titon\Io\Folder('/path/to/folder');
$contents = $folder->read();
```

To increase portability and to reduce complexity of reading and writing data to files,
robust `Reader` and `Writer` classes are available for common resources types like XML, JSON, and INI.
All readers and writers extend the `File` class for full functionality.

```hack
$xml = new Titon\Io\Reader\XmlReader('/path/to/file.xml');
$data = $xml->read();

$ini = new Titon\Io\Writer\IniWriter('/path/to/file.ini');
$ini->write($data);
```

A `Bundle` can be used to manage resources and files from multiple locations.
When reading from a bundle, the contents of each matching file in every location will be merged into a single result.

```hack
$bundle = new Titon\Io\Bundle\ResourceBundle();
$bundle->addPaths('core', Vector {'/messages/', '/msgs/'});
$bundle->addReader(new Titon\Io\Reader\PhpReader());

// Load common resources from the core domain
$messages = $bundle->loadResource('core', 'common');
```