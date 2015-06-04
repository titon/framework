<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Cache\Item;
use Titon\Cache\Storage;
use Titon\Io\Reader;
use \MessageFormatter;

/**
 * The `MessageLoader` handles the loading and translating of messages found within catalogs.
 *
 * @package Titon\Intl
 */
class MessageLoader {

    /**
     * Loaded catalogs.
     *
     * @var \Titon\Intl\CatalogMap
     */
    protected CatalogMap $catalogs = Map {};

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
     * @var \Titon\Intl\Translator
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
     * @return \Titon\Intl\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

    /**
     * Load and generates a catalog of messages based on the defined domain and catalog.
     * The method will attempt to loop through the list of cascading locales,
     * loading and merging messages from each catalog resource file.
     *
     * @param string $domain
     * @param string $catalog
     * @param mixed $ttl
     * @return \Titon\Intl\Catalog
     */
    public function loadCatalog(string $domain, string $catalog, mixed $ttl = null): Catalog {
        $translator = $this->getTranslator();
        $cacheKey = sprintf('intl.catalog.%s.%s.%s', $domain, $catalog, $translator->current()->getCode());

        // Return the catalog if it has been loaded
        if ($this->catalogs->contains($cacheKey)) {
            return $this->catalogs[$cacheKey];
        }

        $storage = $this->getStorage();
        $messages = Map {};

        // Check within the cache first
        if ($storage && $storage->has($cacheKey)) {
            $item = $storage->getItem($cacheKey);

            if ($item->isHit()) {
                $messages = $item->get();
            }
        }

        // Cycle through each locale and load messages from each catalog
        if (!$messages) {
            $locales = $translator->cascade()->toVector();
            $locales->reverse(); // Reverse order so that parents are loaded first

            foreach ($locales as $locale) {
                $bundle = $translator->getLocale($locale)->getMessageBundle();
                $bundle->addReader($this->getReader());

                $bundleMessages = $bundle->loadResource($domain, $catalog);

                // Merge with the previous messages
                if ($bundleMessages instanceof Map) {
                    $messages->setAll($bundleMessages);
                }
            }

            // Cache the messages
            if ($storage && $messages) {
                $storage->save(new Item($cacheKey, $messages, $ttl));
            }
        }

        invariant($messages instanceof Map, 'Message strings must be a map.');

        return $this->catalogs[$cacheKey] = new Catalog($catalog, $domain, $messages);
    }

    /**
     * Set the file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return \Titon\Intl\Translator
     */
    public function setReader(Reader $reader): this {
        $this->reader = $reader;

        return $this;
    }

    /**
     * Set the storage engine to use for catalog caching.
     *
     * @param \Titon\Cache\Storage $storage
     * @return \Titon\Intl\Translator
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

    /**
     * Set the translator instance.
     *
     * @param \Titon\Intl\Translator $translator
     * @return $this
     */
    public function setTranslator(Translator $translator): this {
        $this->translator = $translator;

        return $this;
    }

    /**
     * Load a message from the defined domain catalog and interpolate parameters using the
     * built-in `MessageFormatter`.
     *
     * @param string $key
     * @param \Titon\Intl\ParamList $params
     * @return string
     */
    public function translate(string $key, ParamList $params = Vector {}): string {
        list($domain, $catalog, $id) = array_values(Catalog::parseKey($key));

        return (string) MessageFormatter::formatMessage(
            $this->getTranslator()->current()->getCode(),
            $this->loadCatalog($domain, $catalog)->getMessage($id),
            $params->toArray());
    }

}
