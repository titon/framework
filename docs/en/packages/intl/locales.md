# Locales #

## Associated Information ##

### Metadata ###

### Format Patterns ###

### Inflection Rules ###

### Validation Patterns ###

## Helper Methods ##

The static `canonicalize()` method will accept a locale code and convert it to 1 of 4 specific formats. The following formats are supported:

* `FORMAT_1` - Will convert to dashed lower format (`en-us`). Used in routes and URLs.
* `FORMAT_2` - Will convert to dashed, lower language, upper region format (`en-US`). Used by PHP and the OS.
* `FORMAT_3` - Will convert to underscore, lower language, upper region format (`en_US`). Used internally by Titon. (default)
* `FORMAT_4` - Will convert to spaceless, lower language, upper region format (`enUS`).

```hack
$code = Locale::canonicalize('en-us'); // en_US
```

Furthermore, the static `decompose()` method will accept a locale code and return a map of tagged locale elements. This method is based around PHP's `Locale::parseLocale()`.

```hack
$tags = Locale::decompose('en_US`); // Map { language: 'en', region: 'US' }
```

Lastly, the inverse, `compose()`, which accepts a map of tagged locale elements and returns a combined locale code. This method is based around PHP's `Locale::composeLocale()`.

```hack
$code = Locale::compose(Map { 
    'language' => 'en', 
    'region' => 'US' 
}); // en_US
```

## Supported Locales ##

For a list of supported locales, please visit the [locales resource folder found in the official repository](https://github.com/titon/framework/tree/master/src/Titon/Intl/locales). Every locale listed has an associated set of metadata, but may not have complete format patterns, inflection rules, or validation patterns. If you want to help support a specific locale, please read the contributing section below.

### Contributing ###

Even though our official locale list is thorough, it is nowhere near complete. We are constantly accepting contributions for all locales as the majority have nothing more than metadata defined. If you're looking to help, a good place to start is basing all your locale configuration files on the [English (en) locale](https://github.com/titon/framework/tree/g11n-package/src/Titon/Intl/locales/en). Each of the configuration files should be straight forward as they contain helpful comments, but regardless, here are a few tips to keep in mind.

#### Metadata ####

All metadata associated with a locale must be defined in the `locale.hh` file. Every locale currently has this file defined as it's required for loading but any fixes or changes (like timezone) are welcome.

#### Format Patterns ####

All format patterns must be defined in the `formats.hh` file. Some notes about the structure of this file:

* Each hash (#) character in a pattern will be replaced by the character at the same position in the source string (e.g. the 5th # will be replaced by the 5th character).
* Number and currency maps must have strings for both the keys and values.
* Date and time patterns must use locale aware patterns as defined by [strftime()](http://php.net/manual/en/function.strftime.php).
* The index of the phone map represents the length of the phone number while the value is the pattern to format it with.

#### Inflection Rules ####

All inflection rules must be defined in the `inflections.hh` file. Some notes about the structure of this file:

* Ordinal suffixes, plural forms, and singular forms use regex patterns for detection. Be sure to have the appropriate line start (`^`) and line end (`$`) meta-characters. 
* A catch all regex pattern should always be last.
* Uninflected words must use a `Set`, not a `Vector` or `Map`, but a `Set`.
* Transliteration characters will replace accented characters with non-accented characters when used during slugging and normalization.

#### Validation Patterns ####

All validation patterns must be defined in the `validations.hh` file. This file is simply a mapping of regex patterns, pretty simple.
