# Writers #

A `Titon\Io\Writer` is a specialized type of interface that handles the writing of specific file types, like PHP, XML, and JSON, just to name a few. A writer is primarily used in writing key-value maps to configuration or resource files.

To begin, simply instantiate a new writer with a filesystem path to write to.

```hack
$writer = new Titon\Io\Writer\PhpWriter('/path/to/file.php');
```

We can then pass a key-value map to `writeResource()`, which will convert the map into a properly structured format depending on the type of writer.

```hack
$writer->writeResource(Map {'key' => 'value'});
```

As writers are used for writing files, a [reader](readers.md) can be used for reading files.

## Supported Types ###

The following types of writers are supported.

### PHP ###

The `Titon\Io\Writer\PhpWriter` can be used in writing PHP files by converting a map to an array.

```hack
$writer = new Titon\Io\Writer\PhpWriter('/path/to/file.php');
```

### JSON ###

The `Titon\Io\Writer\JsonWriter` can be used in writing JSON files by converting a map to valid JSON.

```hack
$writer = new Titon\Io\Writer\JsonWriter('/path/to/file.json');
```

### YAML ###

The `Titon\Io\Writer\YamlWriter` can be used in writing YAML files by converting a map to valid YAML.

```hack
$writer = new Titon\Io\Writer\YamlWriter('/path/to/file.yaml');
```

YAML by default is not enabled in HHVM, to enable it, set `hhvm.enable_zend_compat` to `true` and restart HHVM.

### XML ###

The `Titon\Io\Writer\XmlWriter` can be used in writing XML files by converting a map to valid XML.

```hack
$writer = new Titon\Io\Writer\XmlWriter('/path/to/file.xml');
```

This writer makes use of the [XML class](../type/xml.md) and will require a special map format.

### INI ###

The `Titon\Io\Writer\IniWriter` can be used in writing INI files by converting a map to valid INI. 

```hack
$writer = new Titon\Io\Writer\IniWriter('/path/to/file.ini');
```

INI sections are supported through nested maps.

### PO ###

The `Titon\Io\Writer\PoWriter` can be used in writing PO files by converting a map to valid message strings. A PO (portable object) file contains localized messages for the gettext extension.

```hack
$writer = new Titon\Io\Writer\PoWriter('/path/to/file.po');
```

The key in the map is the message identifier, while the value is the message. 

```hack
$writer->writeResource(Map {'key' => 'value'});

// msgid "key"
// msgstr "value"
```

Comments are supported through the `_comments` key, which accept a key-value map.

```hack
$map = Map {
    '_comments' => Map {'Language' => 'en'},
    'key' => 'value'
};
```

Plurals can also be defined by passing a vector of strings.

```hack
$map = Map {
    'item' => Vector {'item', 'items'}
};

// msgid "item"
// msgid_plural "item"
// msgstr[0] "item"
// msgstr[1] "items"
```
