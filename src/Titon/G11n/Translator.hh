<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Io\Reader;
use Titon\Cache\Storage;

/**
 * Interface for g11n string translators.
 *
 * @package Titon\G11n
 */
interface Translator {

    /**
     * Locate the key within the catalog. If the catalog has not been loaded,
     * load it and cache the collection of strings.
     *
     * @param string $key
     * @return string
     */
    public function getMessage(string $key): string;

    /**
     * Return the file reader.
     *
     * @return \Titon\Io\Reader
     */
    public function getReader(): Reader;

    /**
     * Return the cache storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): Storage;

    /**
     * Parse out the domain, catalog and key for string lookup.
     *
     * @param string $key
     * @return string
     */
    public function parseKey(string $key): string;

    /**
     * Set the file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return \Titon\G11n\Translator
     */
    public function setReader(Reader $reader): this;

    /**
     * Set the storage engine to use for catalog caching.
     *
     * @param \Titon\Cache\Storage $storage
     * @return \Titon\G11n\Translator
     */
    public function setStorage(Storage $storage): this;

    /**
     * Process the located string with dynamic parameters if necessary.
     *
     * @param string $key
     * @param array $params
     * @return string
     */
    public function translate($key, array $params = []): string;

}
