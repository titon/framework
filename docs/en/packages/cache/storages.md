# Storage Engines #

The following storage engines are supported by default.

### APC ###

The `Titon\Cache\Storage\ApcStorage` engine integrates with the built-in [APC](http://php.net/manual/en/book.apc.php) API. 
This storage has no configuration options as all instances go through the same internal API.

```hack
$apc = new Titon\Cache\Storage\ApcStorage();
```

### File System ###

The `Titon\Cache\Storage\FileSystemStorage` engine integrates with the local file system for caching. 
A directory to cache files to must be passed to the constructor.

```hack
$fs = new Titon\Cache\Storage\FileSystemStorage('/path/to/cache/folder/');
```

<div class="notice is-info">
    This storage engine requires the <a href="../io/index.md">IO package</a>.
</div>

### Memcache ###

The `Titon\Cache\Storage\MemcacheStorage` engine integrates with the built-in [Memcached](http://php.net/manual/en/book.memcached.php) API. 
A `Memcached` instance must be passed to the constructor, which allows for full customization.

```hack
$memcache = new Titon\Cache\Storage\MemcacheStorage(new Memcached());
```

### Memory ###

The `Titon\Cache\Storage\MemoryStorage` engine provides in-memory caching for the duration of the request. 
Data passed to this storage engine will not persist.

```hack
$memory = new Titon\Cache\Storage\MemoryStorage();
```

### Redis ###

The `Titon\Cache\Storage\RedisStorage` engine integrates with the built-in Redis API. 
A `Redis` instance must be passed to the constructor, which allows for full customization.

```hack
$redis = new Titon\Cache\Storage\RedisStorage(new Redis());
```

## Creating An Engine ##

When creating a storage engine, the custom class must extend the `Titon\Cache\Storage\AbstractStorage` class, 
or implement the `Titon\Cache\Storage` interface. We suggest using the abstract class as it defines a handful of 
default functionality.

When extending the abstract class, the following methods will need to be defined. 
These methods should interact directly with a cache layer.

```hack
class ExampleStorage extends Titon\Cache\Storage\AbstractStorage {
    public function flush(): bool;
    public function get(string $key): mixed;
    public function has(string $key): bool;
    public function remove(string $key): bool;
    public function set(string $key, mixed $value, int $expires): bool;
    public function stats(): Titon\Cache\StatsMap;
}
```

<div class="notice is-warning">
    The <code>get()</code> method must throw a <code>Titon\Cache\Exception\MissingItemException</code> 
    when an item does not exist in the cache, or when an error occurs.
</div>