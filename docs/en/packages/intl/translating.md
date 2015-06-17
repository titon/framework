# Translating #

The `Titon\Intl\Translator` is the sole class that handles translation and message lookups -- it's the only class that should be interacted with directly. To begin, instantiate a new `Translator` instance while passing a `Titon\Intl\MessageLoader` as the only argument. The `MessageLoader` requires a list of `Titon\Io\Reader` instances, which dictate the types of files that messages will be written in. The internal messages provided by Titon are written in Hack and will require a `Titon\Io\Reader\HackReader`.

```hack
use Titon\Intl\MessageLoader;
use Titon\Intl\Translator;
use Titon\Io\Reader\HackReader;
use Titon\Io\Reader\PhpReader;

$translator = new Translator(new MessageLoader(Vector {
    new HackReader(),   // Internal messages are written in Hack
    new PhpReader()     // Application messages are written in PHP
}));
```

Message lookups can be pretty resource intensive and time consuming as the process has to constantly traverse the filesystem and merge datasets. To mitigate this issue, the message catalogs can be cached. To enable caching, pass a `Titon\Cache\Storage` instance as the 2nd argument to `MessageLoader`. This will require the [Cache package](../cache/index.md). 

```hack
use Titon\Cache\Storage\MemoryStorage;

$translator = new Translator(new MessageLoader($readers, new MemoryStorage()));
```

## Configuring Paths ##

Before messages can be translated, resource paths must be defined for lookups. These paths dictate the locations on the filesystem where [message catalogs](messages.md) and [locale configurations](locales.md) exist. Resource paths can be defined using `addResourcePaths()`, which requires a domain name and a set of paths.

```hack
$translator
    ->addResourcePaths('blog', Set {'/path/to/blog/resources'})
    ->addResourcePaths('forum', Set {
        '/path/to/forum/resources',
        '/another/path/from/vendor/forum/resources'
    });
```

<div class="notice is-info">
    A resource folder requires a "locales" and or "messages" folder to exist. These folders will contain the lookup files.
</div>

To define an individual locale or message path, the `addLocalePaths()` or `addMessagePaths()` methods can be used respectively.

```hack
$translator
    ->addLocalePaths(Set {'/path/to/resources/locales'}) // Doesn't require a domain
    ->addMessagePaths('user', Set {'/path/to/user/resources/messages'});
```

<div class="notice is-info">
    Internal messages provided by Titon use the "common" domain.
</div>

## Adding Locales ##

To support localized messages for a specific language, a `Titon\Intl\Locale` can be added to the `Translator`. A `Locale` should be added for each language the application wants to support. [Jump to the locales documentation for more information on supported locales](locales.md).

```hack
$translator
    ->addLocale(new Locale('en_US')) // English
    ->addLocale(new Locale('es_ES')) // Spanish (Spain)
    ->addLocale(new Locale('es_MX')) // Spanish (Mexico)
    ->addLocale(new Locale('ru')); // Russian
```

When a region/territory based locale code is used (the value after `_` in the locale code), the parent locale will automatically be setup as well. In the example above, the "en" and "es" parent locales have also been added.

To verify whether or not the `Translator` is enabled for translating (locales have been added), the method `isEnabled()` can be used.

```hack
$translator->isEnabled(); // true
```

### Fallback Locale ###

The fallback `Locale` is used in situations where a locale code could not be detected from the client. The first `Locale` added is automatically set as the fallback but the fallback can easily be overridden using `setFallback()`, which requires a locale code.

```hack
$translator->setFallback('en');
```

## Detecting Client Locale ##

To detect the language based on the client, the `detect()` method can be used (this method must be called last). The method will first check for the "locale" cookie, which should simply contain a locale code. The no cookie is found, the method will then detect based on the HTTP "Accept-Language" header. If detection has failed at this point, the [fallback locale](#fallback-locale) will be used.

```hack
$translator->detect();
```

<div class="notice is-info">
    The <code>Titon\Intl\Translator</code> does not set cookies, it simply checks for the existence of the "locale" cookie. This cookie must be manually set in the application.
</div>

The detected `Locale` can be accessed using `current()`. If no locale was detected, or `detect()` wasn't called, this method will return `null`.

```hack
$translator->current();
```

### Manual Detection ###

To manually set a locale and skip the detection process defined previously, use the `localize()` method. This method simply requires a locale code.

```hack
$translator->localize('es_MX'); // Use Spanish
```

### Cascading Order ###

Once detected, a cascading order of locale codes can be accessed using `cascade()`, which defines the order in which messages are looked up. 

For example, if the locale was set to "es_MX" and the fallback is set to "en_US", the following order will be returned.

```hack
$translator->cascade(); // Set {'es_MX', 'es', 'en_US', 'en'}
```

## Translating Messages ##

To fetch a localized message from the `Translator` and subsequently the `MessageLoader`, use the `translate()` method. This method requires a [unique message key](messages.md#message-keys).

```hack
$message = $translator->translate('forum.topic.locked');

// forum/resources/messages/topic.hh
return Map {
    'locked' => 'This topic is locked.',
    'locked.mod' => 'This topic is locked for everyone but moderators.' // Keys can also contain periods
};
```

Message formatting makes use of PHP's built-in [MessageFormatter](http://php.net/manual/en/class.messageformatter.php).

### Custom Parameters ###

A vector of parameters can be passed as the 2nd argument to `translate()`, which will interpolate into the message based on the index number.

```hack
$message = $translator->translate('user.dashboard.hello', Vector {'Titon'}); // Hello Titon

// user/resources/messages/dashboard.hh
return Map {
    'hello' => 'Hello {0}'
};
```

### Plural & Variadic Parameters ###

For cases where different wording must be used based on the length of a parameter, the built-in ICU formatting rules can be used. For more information on these formatting rules, check out the official [ICU documentation](http://userguide.icu-project.org/formatparse).

```hack
$message = $translator->translate('blog.search.results', Vector {0}); // 0 results
$message = $translator->translate('blog.search.results', Vector {1}); // 1 result
$message = $translator->translate('blog.search.results', Vector {9}); // 9 results

// blog/resources/messages/search.hh
return Map {
    'results' => '{0,choice,0#0 results|1#1 result|1<{0,number} results}'
};
```

## Resolving Routes ##

The `Translator` can also be tied into the [Route package](../route/index.md) to provide automatic route resolving. This process will check the URL being matched for the existence of a locale, and that the locale is supported. If an error occurs, the URL will be redirected to a new locale or the fallback locale. To make use of this, simply set `Titon\Intl\RouteResolver` as a listener to `Titon\Route\Router` while passing the `Translator`.

```hack
use Titon\Intl\RouteResolver;

$router->on(new RouteResolver($translator));
```
