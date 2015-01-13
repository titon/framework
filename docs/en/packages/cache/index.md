# Cache Package #

The cache package provides a data caching layer that supports popular storage engines like Memcache, APC, Redis,
and the local file system, with features like prefixing and deferred saves.

### Requirements ###

* APC extension (for `ApcStorage`)
* Redis extension (for `RedisStorage`)
* Memcache extension (for `MemcacheStorage`)

### Installation ###

This package can be installed with Composer.

```shell
composer require titon/cache:*
```

### Documentation ###

* [Caching](usage.md)
* [Storage Engines](storages.md)
* [Type Aliases](types.md)