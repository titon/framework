# Global Functions #

The following global functions are available with the Environment package.

<div class="notice is-info">
    Using these functions will require the <code>Titon\Environment\Detector</code> class to be registered in a <code>Titon\Context\Depository</code> instance.
</div>

### env(`string $key`): `string` ###

Returns a variable value defined by key. [Learn more about environment variables](variables.md).

```hack
$key = env('API_KEY');
```

### is_env(`string $key`): `bool` ###

Verify which environment is detected by matching against the `APP_ENV` variable. [Learn more about environment detection.](detection.md)

```hack
is_env('local'); // true
```
