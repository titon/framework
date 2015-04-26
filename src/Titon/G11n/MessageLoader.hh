<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Cache\Storage;
use Titon\Io\Reader;
use \Locale as SystemLocale;
use \MessageFormatter;

/**
 * TODO
 *
 * @package Titon\G11n
 */
class MessageLoader {

    /**
     * Loaded catalogs.
     *
     * @var \Titon\G11n\CatalogMap
     */
    protected CatalogMap $catalogs;

    /**
     * File reader used for parsing.
     *
     * @var \Titon\Io\Reader
     */
    protected Reader $reader;

    /**
     * Storage engine used for caching.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $storage;

    /**
     * Translator instance.
     *
     * @var \Titon\G11n\Translator
     */
    protected ?Translator $translator;

    /**
     * Store the reader and optional storage.
     *
     * @param \Titon\Io\Reader $reader
     * @param \Titon\Cache\Storage $storage
     */
    public function __construct(Reader $reader, ?Storage $storage = null) {
        $this->reader = $reader;
        $this->storage = $storage;
    }

    /**
     * Return the file reader.
     *
     * @return \Titon\Io\Reader
     */
    public function getReader(): Reader {
        return $this->reader;
    }

    /**
     * Return the cache storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->storage;
    }

    /**
     * Return the translator.
     *
     * @return \Titon\G11n\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

    /**
     * TODO
     *
     * @param string $domain
     * @param string $catalog
     * @return \Titon\G11n\Catalog
     */
    public function loadCatalog(string $domain, string $catalog): Catalog {
        $cacheKey = sprintf('g11n.%s.%s', $domain, $catalog);

        // Return the catalog if it has been loaded
        if ($this->catalogs->contains($cacheKey)) {
            return $this->catalogs[$cacheKey];
        }

        $storage = $this->getStorage();

        // Check within the cache first
        if ($storage && $storage->has($cacheKey)) {
            $messages = $storage->get($cacheKey);

        // Cycle through each locale and load messages from each catalog
        } else {
            $translator = $this->getTranslator();
            $messages = Map {};

            foreach ($translator->cascade() as $locale) {
                $bundle = $translator->getLocale($locale)->getResourceBundle();
                $bundle->addReader($this->getReader());

                $bundleMessages = $bundle->loadResource($domain, $catalog);

                // Merge with the previous messages
                if ($bundleMessages instanceof Map) {
                    $messages->setAll($bundleMessages);
                }
            }

            // Cache the messages
            if ($messages && $storage) {
                $storage->set($cacheKey, $messages);
            }
        }

        invariant($messages instanceof Map, 'Message strings must be a map.');

        return $this->catalogs[$cacheKey] = new Catalog($catalog, $domain, $messages);
    }

    /**
     * Set the file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return \Titon\G11n\Translator
     */
    public function setReader(Reader $reader): this {
        $this->reader = $reader;

        return $this;
    }

    /**
     * Set the storage engine to use for catalog caching.
     *
     * @param \Titon\Cache\Storage $storage
     * @return \Titon\G11n\Translator
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

    /**
     * Set the translator instance.
     *
     * @param \Titon\G11n\Translator $translator
     * @return $this
     */
    public function setTranslator(Translator $translator): this {
        $this->translator = $translator;

        return $this;
    }

    /**
     * TODO
     *
     * @param string $key
     * @param \Titon\G11n\ParamList $params
     * @return string
     */
    public function translate(string $key, ParamList $params = Vector {}): string {
        list($domain, $catalog, $id) = Catalog::parseKey($key);

        return (string) MessageFormatter::formatMessage(
            SystemLocale::DEFAULT_LOCALE,
            $this->loadCatalog($domain, $catalog)->getMessage($id),
            $params);
    }

}
