# Translating #

@TODO

## Configuring Paths ##

@TODO

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

To fetch a localized message from the `Translator` and subsequently the `MessageLoader`, use the `translate()` method. This method requires a unique message key, composed of a domain, catalog, and ID in dot notated format. A domain is a [lookup group for paths](#configuring-paths) that also contain catalog files. A catalog is a file within a domain that contains a mapping of messages. An ID is the unique key to locate a message in a catalog.

```hack
$message = $translator->translate('domain.catalog.id');

// catalog.hh
return Map {
    'id' => 'This is a message',
    'sub.nested.id' => 'This is also a message' // Keys can also contain periods
};
```

Message formatting makes use of PHP's built-in [MessageFormatter](http://php.net/manual/en/class.messageformatter.php).

### Custom Parameters ###

A vector of parameters can be passed as the 2nd argument to `translate()`, which will interpolate into the message based on the index number.

```hack
$message = $translator->translate('domain.catalog.hello', Vector {'Titon'}); // Hello Titon

// catalog.hh
return Map {
    'hello' => 'Hello {0}'
};
```

### Plural & Variadic Parameters ###

For cases where different wording must be used based on the length of a parameter, the built-in ICU formatting rules can be used. For more information on these formatting rules, check out the official [ICU documentation](http://userguide.icu-project.org/formatparse).

```hack
$message = $translator->translate('domain.catalog.choice', Vector {0}); // 0 results
$message = $translator->translate('domain.catalog.choice', Vector {1}); // 1 result
$message = $translator->translate('domain.catalog.choice', Vector {9}); // 9 results

// catalog.hh
return Map {
    'choice' => '{0,choice,0#0 results|1#1 result|1<{0,number} results}'
};
```
