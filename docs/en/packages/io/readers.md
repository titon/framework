# Readers #

A `Titon\Io\Reader` is a specialized type of interface that handles the reading of specific file types, like PHP, YAML, and JSON, just to name a few. A reader is primarily used for reading configuration or resource files and returning a usable key-value map.

To begin, simply instantiate a new reader with a filesystem path, and then call `readResource()`.

```hack
$reader = new Titon\Io\Reader\PhpReader('/path/to/file.php');
$map = $reader->readResource();
```

As readers are used for reading files, a [writer](writers.md) can be used for writing files.

## Supported Types ###

The following types of readers are supported.

### Hack ###

The `Titon\Io\Reader\HackReader` can be used in reading Hack files. 

```hack
$reader = new Titon\Io\Reader\HackReader('/path/to/file.hh');
```

A Hack file must return a map to be valid.

```hack
<?hh
return Map {'key' => 'value'};
```

### PHP ###

The `Titon\Io\Reader\PhpReader` can be used in reading PHP files. 

```hack
$reader = new Titon\Io\Reader\PhpReader('/path/to/file.php');
```

A PHP file must return an array to be valid.

```php
<?php
return ['key' => 'value'];
```

### JSON ###

The `Titon\Io\Reader\JsonReader` can be used in reading JSON files. 

```hack
$reader = new Titon\Io\Reader\JsonReader('/path/to/file.json');
```

A JSON file must contain valid JSON.

```javascript
{"key":"value"}
```

### YAML ###

The `Titon\Io\Reader\YamlReader` can be used in reading YAML files. YAML by default is not enabled in HHVM, to enable it, set `hhvm.enable_zend_compat` to `true` and restart HHVM.

```hack
$reader = new Titon\Io\Reader\YamlReader('/path/to/file.yml');
```

A YAML file must contain valid YAML.

```yaml
key: "value"
```

### XML ###

The `Titon\Io\Reader\XmlReader` can be used in reading XML files. This reader makes use of the [XML class](../type/xml.md) and will return a specific map format.

```hack
$reader = new Titon\Io\Reader\XmlReader('/path/to/file.xml');
```

An XML file must contain valid XML.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<config>
    <key>value</key>
</config>
```

### INI ###

The `Titon\Io\Reader\IniReader` can be used in reading INI files. 

```hack
$reader = new Titon\Io\Reader\IniReader('/path/to/file.ini');
```

An INI file must contain valid INI settings. Sections are supported.

```ini
key = "value"
```

### PO ###

The `Titon\Io\Reader\PoReader` can be used in reading PO files. A PO (portable object) file contains localized messages for the gettext extension.

```hack
$reader = new Titon\Io\Reader\PoReader('/path/to/file.po');
```

An PO file must contain valid message declarations.

```
msgid "key"
msgstr "value"
```
