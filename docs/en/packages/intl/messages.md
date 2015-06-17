# Messages #

Message keys and message strings, formally known as simply messages, are the backbone of any internationalized application. They provide a set of translated strings to be used for localization. Messages are defined in a catalog, which is a file that maps [message keys](#message-keys) (the ID portion only) to message strings. The catalog can be in any format supported by the [IO package readers](../io/readers.md). An example catalog file looks like the following:

```hack
// default.hh
<?hh
return Map {
    'hello' => 'Hello!',
    'yes' => 'Yes',
    'no' => 'No'
};
```

Furthermore, a catalog file is represented by a `Titon\Intl\Catalog` class, which is loaded from the `Titon\Intl\MessageLoader::loadCatalog()` method.

## Message Keys ##

A message key is a form of UUID (universally unique identifier) in which the key points to the same single instance of a message string. A message key is composed of a domain, catalog, and ID in dot notated format, like so:

```
<domain>.<catalog>.<id>

// Examples
forum.topic.locked
blog.article.excerpt
user.dashboard.welcome.message
```

<div class="notice is-info">
    Dashes, underscores, and uppercase characters are also welcome within each key part. IDs can also have periods.
</div>

### Parsing Keys ###

The `Titon\Intl\Catalog::parseKey()` method can be used for parsing a message key into its 3 distinct parts. This method will return a shape with the keys "domain", "catalog", and "id".

```hack
$key = Catalog::parseKey('user.dashboard.welcome.message'); // shape('domain' => 'user', 'catalog' => 'dashboard', 'id' => 'welcome.message')
```

### Shared & Shorthand Keys ###

Messages provided internally by Titon and messages that should be shared throughout the entire application use the "common" domain. This common domain provides special functionality that allows for shorthand keys to be used when [translating](translating.md#translating-messages) and [parsing](#catalogs). The following statements are equivalent:

```hack
$translator->translate('common.validation.date');
$translator->translate('validation.date'); // common.validation.date

Catalog::parseKey('common.validation.time');
Catalog::parseKey('validation.time'); // common.validation.time
```

Furthermore, this approach can be simplified even more, by also omitting the catalog, which defaults to "default". Take the following for example:

```hack
$translator->translate('yes'); // common.default.yes

Catalog::parseKey('hello'); // common.default.hello
```

These values are available as `DEFAULT_DOMAIN` and `DEFAULT_CATALOG` constants on the `Titon\Intl\MessageLoader` class.

<div class="notice is-warning">
    This approach only works for messages keys whose ID contains no periods.
</div>

## File Structure ##

Domains and catalogs follow the file structure specification defined in the [IO package bundle layer](../io/bundles.md). However, an important requirement is that messages should be located in a folder named after its respective locale. Folder names should follow the [3rd canonicalize format](locales.md#helper-methods). An example is as follows:

```
domain/messages/
├── en_US/
|   └── catalog.hh
└── fr/
    └── catalog.hh
```
