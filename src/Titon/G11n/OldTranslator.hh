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
 * Interface for message translators.
 *
 * @package Titon\G11n
 */
interface Translator {

    /**
     * Locate the key within the catalog. If the catalog has not been loaded,
     * load it and cache the collection of messages.
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
    public function getReader(): ?Reader;

    /**
     * Return the cache storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage;

    /**
     * Parse out the domain, catalog and key for a message lookup.
     *
     * @param string $key
     * @return \Titon\G11n\MessageKey
     */
    public function parseKey(string $key): MessageKey;

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
     * Process the located message with dynamic parameters if necessary.
     *
     * @param string $key
     * @param \Titon\G11n\ParamList $params
     * @return string
     */
    public function translate(string $key, ParamList $params = Vector {}): string;

}
