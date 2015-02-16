<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Cache\CacheCallback;
use Titon\Cache\HitItem;
use Titon\Cache\Item;
use Titon\Cache\ItemList;
use Titon\Cache\ItemMap;
use Titon\Cache\MissItem;
use Titon\Cache\StatsMap;
use Titon\Cache\Storage;
use Titon\Utility\Config;

/**
 * Primary class for all storage engines to extend. Provides functionality for the Storage interface.
 *
 * @package Titon\Cache\Storage
 */
abstract class AbstractStorage implements Storage {

    /**
     * List of cache items to be committed.
     *
     * @var \Titon\Cache\ItemList
     */
    protected ItemList $deferred = Vector {};

    /**
     * Unique cache key prefix.
     *
     * @var string
     */
    protected string $prefix = '';

    /**
     * Set the unique prefix during instantiation.
     *
     * @param string $prefix
     */
    public function __construct(string $prefix = '') {
        $this->setPrefix($prefix);
    }

    /**
     * {@inheritdoc}
     */
    public function clear(): bool {
        return $this->flush();
    }

    /**
     * {@inheritdoc}
     */
    public function commit(): bool {
        foreach ($this->getDeferred() as $item) {
            $this->save($item);
        }

        $this->getDeferred()->clear();

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function decrement(string $key, int $step = 1, int $initial = 0): int {
        $item = $this->getItem($key);

        $value = ((int) $item->get() ?: $initial) - $step;

        $item->set($value);

        $this->save($item);

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function deleteItem(string $key): this {
        $this->remove($key);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function deleteItems(array<string> $keys): this {
        foreach ($keys as $key) {
            $this->remove($key);
        }

        return $this;
    }

    /**
     * Return a list of items waiting to be cached.
     *
     * @return \Titon\Cache\ItemList
     */
    public function getDeferred(): ItemList {
        return $this->deferred;
    }

    /**
     * {@inheritdoc}
     */
    public function getItem(string $key): Item {
        try {
            return new HitItem($key, $this->get($key));
        } catch (MissingItemException $e) {
            return new MissItem($key);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function getItems(array<string> $keys = []): ItemMap {
        $map = Map {};

        foreach ($keys as $key) {
            $map[$key] = $this->getItem($key);
        }

        return $map;
    }

    /**
     * {@inheritdoc}
     */
    public function getPrefix(): string {
        return (string) Config::get('cache.prefix', '') . $this->prefix;
    }

    /**
     * {@inheritdoc}
     */
    public function increment(string $key, int $step = 1, int $initial = 0): int {
        $item = $this->getItem($key);

        $value = ((int) $item->get() ?: $initial) + $step;

        $item->set($value);

        $this->save($item);

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function save(Item $item): this {
        $timestamp = $item->getExpiration()?->getTimestamp() ?: 0;

        if ($timestamp <= time()) {
            return $this; // Already expired
        }

        $this->set($item->getKey(), $item->get(), $timestamp);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function saveDeferred(Item $item): this {
        $this->deferred[] = $item;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setPrefix(string $prefix): this {
        $this->prefix = $prefix;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): StatsMap {
        return Map {};
    }

    /**
     * {@inheritdoc}
     */
    public function store(string $key, CacheCallback $callback, mixed $expires = null): mixed {
        if ($this->has($key)) {
            return $this->getItem($key)->get();
        }

        $value = call_user_func($callback);

        $this->save(new Item($key, $value, $expires));

        return $value;
    }

}
