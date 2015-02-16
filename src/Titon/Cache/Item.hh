<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

use Titon\Cache\Exception\UnsupportedOperationException;
use Titon\Utility\Config;
use Titon\Utility\Time;
use \DateTime;

/**
 * The Item class represents an item with a key, value, and expiration date that is persisted in a cache.
 *
 * @package Titon\Cache
 */
class Item {

    /**
     * The timestamp of when the item expires.
     *
     * @var \DateTime
     */
    protected ?DateTime $expires;

    /**
     * The unique cache key.
     *
     * @var string
     */
    protected string $key = '';

    /**
     * The items value to be saved.
     *
     * @var mixed
     */
    protected mixed $value = '';

    /**
     * Set the key, value, and expiration.
     *
     * @param string $key
     * @param mixed $value
     * @param mixed $ttl
     */
    public function __construct(string $key, mixed $value = '', mixed $ttl = null) {
        $this->setKey($key);
        $this->set($value);
        $this->setExpiration($ttl);
    }

    /**
     * Confirms if the cache item exists in the cache.
     *
     * Note: This method MAY avoid retrieving the cached value for performance
     * reasons, which could result in a race condition between exists() and get().
     * To avoid that potential race condition use isHit() instead.
     *
     * @return boolean
     *  True if item exists in the cache, false otherwise.
     */
    public function exists(): bool {
        throw new UnsupportedOperationException('Please use a storage engine for checking an items existence.');
    }

    /**
     * Retrieves the value of the item from the cache associated with this objects key.
     *
     * The value returned must be identical to the value original stored by set().
     *
     * if isHit() returns false, this method MUST return null. Note that null
     * is a legitimate cached value, so the isHit() method SHOULD be used to
     * differentiate between "null value was found" and "no value was found."
     *
     * @return mixed
     *   The value corresponding to this cache item's key, or null if not found.
     */
    public function get(): mixed {
        return $this->value;
    }

    /**
     * Returns the expiration time of a not-yet-expired cache item.
     *
     * If this cache item is a Cache Miss, this method MAY return the time at
     * which the item expired or the current time if that is not available.
     *
     * @return \DateTime
     *   The timestamp at which this cache item will expire.
     */
    public function getExpiration(): ?DateTime {
        return $this->expires;
    }

    /**
     * Returns the key for the current cache item.
     *
     * The key is loaded by the Implementing Library, but should be available to
     * the higher level callers when needed.
     *
     * @return string
     *   The key string for this cache item.
     */
    public function getKey(): string {
        return $this->key;
    }

    /**
     * Confirms if the cache item lookup resulted in a cache hit.
     *
     * Note: This method MUST NOT have a race condition between calling isHit()
     * and calling get().
     *
     * @return boolean
     *   True if the request resulted in a cache hit.  False otherwise.
     */
    public function isHit(): bool {
        return false;
    }

    /**
     * Sets the value represented by this cache item.
     *
     * The $value argument may be any item that can be serialized by PHP,
     * although the method of serialization is left up to the Implementing
     * Library.
     *
     * Implementing Libraries MAY provide a default TTL if one is not specified.
     * If no TTL is specified and no default TTL has been set, the TTL MUST
     * be set to the maximum possible duration of the underlying storage
     * mechanism, or permanent if possible.
     *
     * @param mixed $value
     *   The serializable value to be stored.
     * @return $this
     */
    public function set(mixed $value): this {
        $this->value = $value;

        return $this;
    }

    /**
     * Sets the expiration for this cache item.
     *
     * @param mixed $ttl
     *   - If an integer is passed, it is interpreted as the number of seconds
     *     after which the item MUST be considered expired.
     *   - If a DateTime object is passed, it is interpreted as the point in
     *     time after which the item MUST be considered expired.
     *   - If null is passed, a default value MAY be used. If none is set,
     *     the value should be stored permanently or for as long as the
     *     implementation allows.
     *
     * @return $this
     */
    public function setExpiration(mixed $ttl = null): this {
        if (!$ttl) {
            $time = strtotime((string) Config::get('cache.expires', '+1 hour'));

        } else if (is_int($ttl)) {
            $time = time() + $ttl;

        } else {
            $time = Time::toUnix($ttl);
        }

        $this->expires = Time::factory($time);

        return $this;
    }

    /**
     * Set the cache key but inflect it before hand to be valid.
     *
     * @param string $key
     * @return $this
     */
    public function setKey(string $key): this {
        $this->key = $key;

        return $this;
    }

}
