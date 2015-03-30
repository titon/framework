# Secure Variables #

There are 2 possible dotenv files. The first being the base `.env` file, which is loaded first for all environments, and the second being the environment specific `.env.{key}` file (`.env.local`, `.env.prod`, etc), which is loaded based on the `APP_ENV` variable. Any variables defined in the later file will overwrite the former, unless explicitly set as immutable.

To make use of these variables, they will need to be loaded during the [detection process](detection.md).

## Configuration Formatting ##

The `.env` files use an INI configuration like structure with a few additional features, like type casting, inline comments, lists, and maps. All keys must be in alpha-numeric uppercase format (underscores allowed).

To start, all primitive types are supported.

```ini
STRING = foo
STRING_WITH_QUOTES = "bar baz"
INTEGER = 123
FLOAT = 456.7
BOOL_TRUE = true
BOOL_FALSE = false
```

As well as lists and maps.

```ini
LIST[] = 123
LIST[] = 456
LIST[] = 789
MAP[foo] = bar
MAP[baz] = qux
```

And finally, 3 types of inline comments.

```ini
; This is a comment
# As well as this
// And also this one
```

## Variable Interpolation ##

Variables may be nested within each other using curly brace syntax. Only strings are supported.

```ini
FOO = foo
BAR = bar
STRING = "{FOO}-{BAR}" # foo-bar
```

## Retrieving A Variable ##

Once loaded, we can access the variables using `getVariable()` and `getVariables()` on the `Titon\Environment\Detection` instance, or through the global `env()` [function](functions.md).

```hack
$env->getVariable('STRING'); // foo

env('FLOAT'); // 456.7
```

<div class="notice is-info">
    Secure variables will not be found in <code>$_ENV</code> or <code>$_SERVER</code>, as super globals are immutable.
</div>

## Immutable Variables ##

When loading `.env` files, the `.env.{key}` file has the ability to overwrite variables found in the base file. In some cases, we do not want to overwrite a variable, like `APP_ENV` for instance. To circumvent this, a variable can be marked as immutable.

```hack
Titon\Environment\Loader::lock('VAR_NAME');
```

<div class="notice is-info">
    The <code>APP_ENV</code> variable is immutable by default.
</div>
