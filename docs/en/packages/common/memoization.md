# Memoization #

Memoization is an optimization technique that caches the result of expensive processes for the duration of the request.
This type of caching is handled by the `Titon\Common\Cacheable` and `Titon\Common\StaticCacheable` traits.

```hack
class Foo {
    use Titon\Common\Cacheable;
}

// Static class
class Bar {
    use Titon\Common\StaticCacheable;
}
```

<div class="notice is-info">
    The API for <code>StaticCacheable</code> is the same as <code>Cacheable</code>, excluding toggle support.
</div>

## Caching Data ##

There are 2 ways to cache data in the class -- the first being the preferred way, using `cache()`. 
The `cache()` method handles the automatic checking, fetching, and caching of data through a callback.

```hack
public function doExpensiveProcess(): mixed {
    return $this->cache(__METHOD__, ($self) ==> {
        // Expensive calculation
    });
}
```

In the example above, if the key defined by `__METHOD__` exists, then the cached result is returned, 
else it is set using the value from the defined callback.

<div class="notice is-info">
    The class instance is passed as the 1st argument to the callback. This does not apply when using <code>StaticCacheable</code>.
</div>

The other way to cache data, is with the `setCache()` method. This method is useful when you want to manually handle the 
entire checking and fetching process using different steps. The following example is functionality equivalent to `cache()`.

```hack
public function doExpensiveProcess(): mixed {
    $key = $this->createCacheKey(__METHOD__);

    if ($this->hasCache($key)) {
        return $this->getCache($key);
    }

    // Expensive calculation

    return $this->setCache($key, $value);
}
```

## Retrieving Data ##

Besides the `cache()` method mentioned previously, the `getCache()` method is used for retrieving an item by a unique key. 
If the key does not exist, then `null` is returned.

```hack
$this->getCache('foo'); // Cached value or null
```

## Removing Cached Items ##

To remove a cached item, use the `removeCache()` method.

```hack
$this->removeCache('foo');
```

Or to remove all cached items, use `flushCache()`.

```hack
$this->flushCache();
```

## Toggling Caching ##

There may be times where caching needs to be disabled for a short duration. This can be achieved with the `toggleCache()` method. 
The method accepts a boolean, which either enables or disables caching.

```hack
$this->toggleCache(false); // Disable

// Do something that shouldn't be cached

$this->toggleCache(true); // Enable
```

To check the state of caching, use the `isCacheEnabled()` method.

## Creating Unique Keys ##

To create a unique cache key, one can use the `createCacheKey()` method. This method accepts scalar and traversable values. 
When a traversable is used, each item is separated with a `-` and optionally hashed.

```hack
$this->createCacheKey('foo'); // foo
$this->createCacheKey(['foo', 'bar']); // foo-bar
$this->createCacheKey(['foo', 'bar', ['nested', 'array']]); // foo-bar-d3e545e5b6dd7d1c9c7be76d5bb18241
```

<div class="notice is-info">
    This method is automatically called within all the other methods.
</div>