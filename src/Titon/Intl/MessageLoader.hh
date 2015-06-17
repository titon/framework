<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Cache\Item;
use Titon\Cache\Storage;
use Titon\Intl\Exception\UndetectedLocaleException;
use Titon\Io\Reader;
use Titon\Io\ReaderList;
use \MessageFormatter;

/**
 * The `MessageLoader` handles the loading and translating of messages found within catalogs.
 *
 * @package Titon\Intl
 */
class MessageLoader {

    const string DEFAULT_DOMAIN = 'common';
    const string DEFAULT_CATALOG = 'default';

    /**
     * Loaded catalogs.
     *
     * @var \Titon\Intl\CatalogMap
     */
    protected CatalogMap $catalogs = Map {};

    /**
     * File readers used for parsing.
     *
     * @var \Titon\Io\ReaderList
     */
    protected ReaderList $readers = Vector {};

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
     * @param \Titon\Io\ReaderList $readers
     * @param \Titon\Cache\Storage $storage
     */
    public function __construct(ReaderList $readers, ?Storage $storage = null) {
        $this->readers = $readers;
        $this->storage = $storage;
    }

    /**
     * Add a file reader to use for resource parsing.
     *
     * @param \Titon\Io\Reader $reader
     * @return \Titon\Intl\Translator
     */
    public function addReader(Reader $reader): this {
        $this->readers[] = $reader;

        return $this;
    }

    /**
     * Add multiple file readers.
     *
     * @param \Titon\Io\ReaderList $readers
     * @return \Titon\Intl\Translator
     */
    public function addReaders(ReaderList $readers): this {
        $this->readers->addAll($readers);

        return $this;
    }

    /**
     * Return the file readers.
     *
     * @return \Titon\Io\ReaderList
     */
    public function getReaders(): ReaderList {
        return $this->readers;
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
        invariant($this->translator !== null, 'Translator must be defined.');

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
        $code = $translator->current()?->getCode();
        $cacheKey = sprintf('intl.catalog.%s.%s.%s', $domain, $catalog, $code);

        if ($code === null) {
            throw new UndetectedLocaleException('No locale has been detected. Cannot load messages.');
        }

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
                $bundle->addReaders($this->getReaders());

                $bundleMessages = $bundle->loadResource($domain, $catalog);

                // Merge with the previous messages
                if ($bundleMessages instanceof Map && $messages instanceof Map) {
                    $messages->setAll($bundleMessages);
                }
            }

            // Cache the messages
            if ($storage && $messages) {
                $storage->save(new Item($cacheKey, $messages, $ttl)); // TODO customize TTL
            }
        }

        invariant($messages instanceof Map, 'Message strings must be a map.');

        return $this->catalogs[$cacheKey] = new Catalog($catalog, $domain, $messages);
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
        $key = Catalog::parseKey($key);

        return (string) MessageFormatter::formatMessage(
            (string) $this->getTranslator()->current()?->getCode(),
            $this->loadCatalog($key['domain'], $key['catalog'])->getMessage($key['id']),
            $params->toArray());
    }

}
