<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Translator;

use Titon\Cache\Storage;
use Titon\Io\Reader;
use Titon\G11n\Translator;
use Titon\G11n\Exception\InvalidCatalogException;
use \MessageFormatter;
use \Locale;

/**
 * Abstract class that implements the message translation functionality for translators.
 *
 * @package Titon\G11n\Translator
 */
abstract class AbstractTranslator implements Translator {

    /**
     * File reader used for parsing.
     *
     * @var \Titon\Io\Reader
     */
    protected ?Reader $reader;

    /**
     * Storage engine used for caching.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $storage;

    /**
     * {@inheritdoc}
     */
    public function getReader(): ?Reader {
        return $this->reader;
    }

    /**
     * {@inheritdoc}
     */
    public function getStorage(): ?Storage {
        return $this->storage;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\G11n\Exception\InvalidCatalogException
     */
    <<__Memoize>>
    public function parseKey(string $key): MessageKey {
        $parts = explode('.', preg_replace('/[^-a-z0-9\.]+/i', '', $key));
        $count = count($parts);
        $domain = 'core';

        if ($count < 2) {
            throw new InvalidCatalogException(sprintf('No domain or catalog present for %s key', $key));

        } else if ($count === 2) {
            $catalog = $parts[0];
            $key = $parts[1];

        } else {
            $domain = array_shift($parts);
            $catalog = array_shift($parts);
            $key = implode('.', $parts);
        }

        return shape(
            'domain' => $domain,
            'catalog' => $catalog,
            'key' => $key
        );
    }

    /**
     * {@inheritdoc}
     */
    public function setReader(Reader $reader): this {
        $this->reader = $reader;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function translate(string $key, ParamList $params = Vector {}) {
        return (string) MessageFormatter::formatMessage(Locale::DEFAULT_LOCALE, $this->getMessage($key), $params);
    }

}
