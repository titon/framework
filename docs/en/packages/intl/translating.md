# Translating #

## Configuring Paths ##

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

To verify whether or not the `Translator` is enabled for translating (locales have been defined), the method `isEnabled()` can be used.

```hack
$translator->isEnabled(); // true
```

### Fallback Locale ###

The fallback `Locale` is used in situations where a locale code could not be detected from the client. The first `Locale` added is automatically set as the fallback but the fallback can easily be overridden using `setFallback()`, which requires a locale code.

```hack
$translator->setFallback('en');
```

## Detecting Client Locale ##

### Manual Detection ###

### Cascading Order ###

## Translating Messages ##

### Custom Parameters ###

### Pluralization ###
