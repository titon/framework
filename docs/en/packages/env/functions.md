# Global Functions #

The following global functions are available with the Environment package.

### env(`string $key`): `string` ###

Returns a secure variable defined by key. [Learn more about secure variables](environments.md#secure-variables).

```hack
$key = env('API_KEY');
```

<div class="notice is-info">
    Using this function requires the `Titon\Environment\Environment` class to be loaded in registry.
</div>
