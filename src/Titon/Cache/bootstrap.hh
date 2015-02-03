<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the cache package.
 */

namespace Titon\Cache {
    type CacheCallback = (function(): mixed);
    type ItemList = Vector<Item>;
    type ItemMap = Map<string, Item>;
    type StatsMap = Map<string, mixed>;
    type StorageMap = Map<string, Storage>;
}
