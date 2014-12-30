# Cache #

Provides a data caching layer that supports popular storage engines like Memcache, APC, Redis,
and the local file system. A `Cache` instance can be used to manage and interact with `Storage` engines.

```hack
$cache = new Titon\Cache\Cache();
$cache->addStorage('memcache', new Titon\Cache\Storage\MemcacheStorage());
$cache->addStorage('fs', new Titon\Cache\Storage\FileSystemStorage());

$cache->set('foo', $data, '+1 hour', 'memcache');
$cache->get('bar', 'fs');
```

`Storage` engines can also be used externally and are primarily injected into other services for caching.

```hack
$apc = new Titon\Cache\Storage\ApcStorage();
$apc->remove('foo');
```

An `Item` class is returned when retrieving items from the cache. These items can be used to check for hits and misses.

```hack
$item = $storage->get('foo');

if ($item->isHit()) {
    // ...
}
```