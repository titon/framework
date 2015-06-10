# Global Functions #

The following global functions are available with the Intl package.

<div class="notice is-info">
    Using these functions will require the <code>Titon\Intl\Translator</code> class to be registered in a <code>Titon\Context\Depository</code> instance.
</div>

### msg(`string $key`[, `Titon\Intl\ParamList $params`]): `string` ###

Returns a message defined by key in dot notated format (includes the domain and catalog). An optional list of parameters can be passed for interpolation. [Learn more about translating strings](translating.md).

```hack
$message = msg('domain.catalog.key');
```

### __(`string $id`[, `string $catalog`[, `string $domain`[, `Titon\Intl\ParamList $params`]]]): `string` ###

A variant on `msg()` which allows for the catalog, domain, and key to be entered separately. If the catalog and domain are omitted, they will default to "default" and "common" respectively.

```hack
$message = __('key', 'catalog', 'domain');
```

### translator_context(): `Titon\Intl\Translator` ###

Returns the `Translator` instance found in the `Titon\Context\Depository` container. If no instance exists, it will attempt to make one.
