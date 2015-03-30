# Environment Detection #

It's good practice to have unique sets of configuration for different types of environments. The `Titon\Environment\Detector` class provides a mechanism for loading special `.env` (known as dotenv) configuration files, which in turn contain a set of secure variables.

[Secure variables](variables.md) (also known as environment variables) represent sensitive configuration, like API keys, encryption keys, and database passwords, all of which *should not* be part of an application's source code, nor should it ever be committed into a VCS.

To begin, instantiate a new `Detector` instance while providing a directory path to where the `.env` files will be found.

```hack
$env = new Titon\Environment\Detector('/path/to/config/');
```

Then we call `initialize()` to trigger the lookup and detection process. This process will attempt to find and parse all relative `.env` files and determine the current environment based on the `APP_ENV` variable (will fallback to "prod" if not found).

```hack
$env->initialize();
```

Once detected, we can use the `is()` method, or the global `is_env()` [function](functions.md) to determine the current environment. This method will compare against the value of `APP_ENV`.

```hack
$env->is('prod');

is_env('local');
```
