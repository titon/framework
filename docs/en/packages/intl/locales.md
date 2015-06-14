# Locales #

A `Titon\Intl\Locale` represents a set of parameters and metadata for a language, a country, and an array of other variances and preferences. When used in the context of an application, it defines a languages that will provide translated messages. 

A `Locale` and an associated code can be setup through `Titon\Intl\Translator::addLocale()`. [More information on translating can be found here](translating.md). When a region/territory based locale code is used (the value after _ in the locale code), the parent locale will automatically be setup as well.

```hack
$translator->addLocale(new Locale('en_US')); // Will also add "en"
```

The parent `Locale` can be accessed using `getParentLocale()`. If no parent exists, it will return `null`.

```hack
$parent = $locale->getParentLocale();
```

## Supported Locales ##

For a list of supported locales, please visit the [locales resource folder found in the official repository](https://github.com/titon/framework/tree/master/src/Titon/Intl/locales). Each locale provided by Titon has a unique set of configuration which defines the rules for outputting and formatting data (found below). However, some of the configurations may be incomplete, [and could benefit from contributions](#contributing).

### Metadata ###

Metadata associated with a `Locale` can be accessed using `getMetadata()`. This will return a bag with the following methods:

* `getCode()` - The code used during creation.
* `getISO2Code()` - The official [ISO 3166-1 alpha-2 country code](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2).
* `getISO3Codes()` - A list of official [ISO 3166-1 alpha-3 country codes](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-3).
* `getTimezone()` - The timezone most associated with the locale.
* `getParentCode()` - The parent locale code, if applicable.
* `getPluralRule()` - The rule in which pluralization forms are determined.
* `getTitle()` - The name of the locale.

```hack
$locale->getMetadata()->getISO3Codes(); // Vector {'eng'}
```

### Format Patterns ###

Patterns for formatting data can be accessed using `getFormatPatterns()`. This will return a bag with the following methods:

* `getCurrency()` - A mapping of currency related formats. Defines the dollar and cents signs, negative usage, title form, and more.
* `getDate()` - A format for the display of dates.
* `getDatetime()` - A format for the display of dates with times.
* `getNumber()` - A mapping of number related formats. Defines decimal and comma characters, decimal places, and more.
* `getPhone()` - A mapping of for the display of phone numbers based on number length.
* `getSSN()` - A format for the display of SSNs (social security number) or IDs.
* `getTime()` - A format for the display of times.

```hack
$locale->getFormatPatterns()->getDate(); // %m/%d/%Y
```

### Inflection Rules ###

Rules for inflecting words, like plural and singular forms, can be accessed using `getInflectionRules()`. This will return a bag with the following methods:

* `getIrregularWords()` - A mapping of singular to plural forms, for words that don't follow normal rules.
* `getOrdinalSuffixes()` - A mapping of regex patterns to numerical ordinal suffixes.
* `getPluralPatterns()` - A mapping of regex patterns to pluralization rules.
* `getSingularPatterns()` - A mapping of regex patterns to singularization rules.
* `getTransliterations()` - A mapping of accented characters to non-accented equivalent characters.
* `getUninflectedWords()` - A list of words that should not be inflected (words without plural or singular forms for example).

```hack
$locale->getInflectionRules()->getUninflectedWords(); // Set {'rice', ...}
```

### Validation Patterns ###

Patterns for validating data can be accessed using `getValidationPatterns()`. This will return a bag with the following methods:

* `getCurrency()` - A regex pattern for detecting currency.
* `getPhone()` - A regex pattern for detecting phone numbers.
* `getPostalCode()` - A regex pattern for detecting postal/zip codes.
* `getSSN()` - A regex pattern for detecting SSNs (social security number) or IDs.

```hack
$locale->getValidationPatterns()->getSSN(); // /^[0-9]{3}-[0-9]{2}-[0-9]{4}$/
```

## Helper Methods ##

The static `canonicalize()` method will accept a locale code and convert it to 1 of 4 specific formats. The following formats are supported:

* `FORMAT_1` - Will convert to dashed lower format (`en-us`). Used in routes and URLs.
* `FORMAT_2` - Will convert to dashed, lower language, upper region format (`en-US`). Used by PHP and the OS.
* `FORMAT_3` - Will convert to underscored, lower language, upper region format (`en_US`). Used internally by Titon. (default)
* `FORMAT_4` - Will convert to spaceless, lower language, upper region format (`enUS`).

```hack
$code = Locale::canonicalize('en-us', Locale::FORMAT_3); // en_US
```

Furthermore, the static `decompose()` method will accept a locale code and return a map of tagged locale elements. This method is based around PHP's `Locale::parseLocale()`.

```hack
$tags = Locale::decompose('en_US'); // Map { language: 'en', region: 'US' }
```

Lastly, the inverse, `compose()`, which accepts a map of tagged locale elements and returns a combined locale code. This method is based around PHP's `Locale::composeLocale()`.

```hack
$code = Locale::compose(Map { 
    'language' => 'en', 
    'region' => 'US' 
}); // en_US
```

## Contributing ##

Even though our official locale list is thorough, it is nowhere near complete. We are constantly accepting contributions for all locales as the majority have nothing more than metadata defined. If you're looking to help, a good place to start is basing all your locale configuration files on the [English (en) locale](https://github.com/titon/framework/tree/master/src/Titon/Intl/locales/en). The configuration files should be straight forward as they contain helpful comments, but regardless, here are a few tips to keep in mind.

### Metadata ###

All metadata associated with a locale must be defined in the `locale.hh` file. Every locale currently has this file defined as it's required for loading but any fixes or changes (like timezone) are welcome.

### Format Patterns ###

All format patterns must be defined in the `formats.hh` file. Some notes about the structure of this file:

* Each hash character (#) in a pattern will be replaced by the character at the same position in the source string (e.g. the 5th # will be replaced by the 5th character).
* Number and currency maps must have strings for both the keys and values.
* Date and time patterns must use locale aware tokens as defined by [strftime()](http://php.net/manual/en/function.strftime.php).
* The index of the phone map represents the length of the phone number while the value is the pattern to format it with.

### Inflection Rules ###

All inflection rules must be defined in the `inflections.hh` file. Some notes about the structure of this file:

* Ordinal suffixes, plural forms, and singular forms use regex patterns for detection. Be sure to have the appropriate line start (`^`) and line end (`$`) meta-characters. 
* A catch all regex pattern should always be last.
* Uninflected words must use a `Set`, not a `Vector` or `Map`, but a `Set`.
* Transliteration characters will replace accented characters with non-accented characters when used during slugging and normalization.

### Validation Patterns ###

All validation patterns must be defined in the `validations.hh` file. This file is simply a mapping of regex patterns, pretty simple.
