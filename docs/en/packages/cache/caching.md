# Caching #

Titon provides a unified API for caching through various storage engines. A storage engine is a type of cache pool that provides specialized functionality for popular cache engines, like Memcache. This functionality may include namespacing (pools), prefixing, and unique configurations. Jump to the storage engine documentation for a [complete list of supported storage engines](storages.md).

A `Titon\Cache\Storage` engine can be used by itself, managed through a `Titon\Cache\Cache` instance, or injected into external classes that want to make us of caching. 

The `Cache` class can be used for managing multiple `Storage` objects. It pipes methods to the respective storage engine mapped by a key. This class is useful for managing storage engines in a single location, but is not required.

```hack
$storage = new Titon\Cache\Storage\MemoryStorage();

$cache = new Titon\Cache\Cache();
$cache->addStorage('memory', $storage);
```

This example will continue to be used in following examples.

## Items ##

Whether your saving data, or retrieving it, the `Titon\Cache\Item` class is used. This class manages the cache key, the value, an expiration date, and any hit or miss checks.

When creating an item, a key is required, with the value and expiration date being optional. These values can also be set through their respective setters, `set()`, `setKey()`, and `setExpiration()`.

```hack
$item = new Titon\Cache\Item('key', 'value', '+5 minutes');
```

The type of value passed as the expiration date determines how the date is used. The following patterns are supported.

* If `string` is used, it will be passed through `strtotime()`. 
* If `int` is used, then it will be treated as a TTL (time to live) and be added to `time()`.
* If `DateTime` is used, then the time will be used as the literal expiration.
* If no value or an empty value is passed, then the expiration time will be +1 hour.

To access values on the item, use the `get()`, `getKey()`, and `getExpiration()` methods.

```hack
$item->get(); // mixed
$item->getExpiration() // DateTime
```

To check if an item was found in the cache (a hit) or was not found (a miss), use the `isHit()` method.

```hack
if ($item->isHit()) {
    // ...
}
```

## Saving Items ##

The `save()` method on the storage engine can be used for saving an item, which requires a `Titon\Cache\Item` instance.

```hack
$storage->save(new Item('key', 'value', '+5 minutes'));
```

If using the `Cache` class, the `set()` method can be used which will automatically setup an `Item` instance, and pass it to the storage engine defined by key.

```hack
$cache->set('key', 'value', '+5 minutes', 'memory');
```

### Bulk & Deferred Saving ###

The `saveDeferred()` method can be used for deferring items to be saved at a later time. This is very helpful in bulk saving items that are built from loops or expensive processes.

```hack
foreach ($items as $item) {
    // Some expensive process
    $storage->saveDeferred(new Item($item->key, $item->value, $item->expires));
}
```

Once the deferred queue is ready to be saved, simply `commit()` them.

```hack
$storage->commit();
```

The `Cache` class does not support deferred items.

### Callback Saving ###

The `store()` method handles the automatic checking, fetching, and caching of data through a callback. If the cache key exists, then the cached value is returned as an `Item`, else it is set using the value from the defined callback.

```hack
$storage->store('foo', () ==> {
    // Expensive calculation
}, '+1 hour');
```

## Retrieving Items ##

The `getItem()` method on the storage engine can be used to retrieve a cache item defined by key. This method will return an `Item` instance, which should be used to check for a cache hit or miss.

```hack
$item = $storage->getItem('foo');

if ($item->isHit()) {
    // ...
}
```

To retrieve multiple items at once, use the `getItems()` method. This will return a map of `Item`s, with the map key being the item key.

```hack
$items = $storage->getItems(['foo', 'bar']);
```

<div class="notice is-info">
    Depending on the type of result, either a <code>Titon\Cache\HitItem</code> or <code>Titon\Cache\MissItem</code> will be returned.
</div>

If using the `Cache` class, the `get()` method can be used for retrieving. This will also return an `Item` instance.

```hack
$cache->get('foo', 'memory');
```

## Deleting Items ##

The `deleteItem()` method on the storage engine can be used for deleting an item defined by key.

```hack
$storage->deleteItem('foo');
```

To delete multiple items at once, use the `deleteItems()` method.

```hack
$storage->deleteItems(['foo', 'bar']);
```

If using the `Cache` class, the `remove()` method can be used for deleting.

```hack
$cache->remove('foo', 'memory');
```

### Flushing The Pool ###

To completely empty the cache and delete all items, use `flush()`. This method does not take into account namespaces or prefixes and will delete everything, so use cautiously.

```hack
$storage->flush();
```

If using the `Cache` class, the `flush()` method can be used. If no key is passed to the method, it will flush all storage engines currently defined in the class.

```hack
$cache->flush('memory');
$cache->flush();
```

## Incrementing & Decrementing ##

The `increment()` method will increase a number by a stepped value.

```hack
$storage->increment('foo', 5); // by 5
```

While the `decrement()` method will decrease a number.

```hack
$storage->decrement('foo'); // by 1
```

The same methods are available on the `Cache` class.

## Key Prefixing ##

Every storage engine supports prefixing of cache keys to avoid collision. The prefix can either be set through the constructor or using the `setPrefix()` method.

```hack
$storage = new MemoryStorage('prefix-'); // Or
$storage->setPrefix('prefix-');
```

Global prefixing can also be set through the configuration layer. This is extremely useful in setting application specific cache namespaces.

```hack
Titon\Utility\Config::set('cache.prefix', 'global-');
```

## Miscellaneous Usage ##

To check if an item exists in the cache, use `has()`. This method is available on both the storage engine and `Cache`.

```hack
$storage->has('foo');
```

To view statistics related to the current storage engine, like memory usage, and hit and miss counts, use `stats()`.

```hack
$storage->stats();
```
