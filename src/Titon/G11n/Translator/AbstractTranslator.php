<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Translator;

use Titon\Cache\Traits\StorageAware;
use Titon\Common\Base;
use Titon\Common\Traits\Cacheable;
use Titon\Cache\Storage;
use Titon\Io\Reader;
use Titon\G11n\Translator;
use Titon\G11n\Exception\InvalidCatalogException;
use \MessageFormatter;
use \Locale;

/**
 * Abstract class that implements the string translation functionality for Translators.
 *
 * @package Titon\G11n\Translator
 */
abstract class AbstractTranslator extends Base implements Translator {
    use Cacheable, StorageAware;

    /**
     * File reader used for parsing.
     *
     * @var \Titon\Io\Reader
     */
    protected $_reader;

    /**
     * {@inheritdoc}
     */
    public function getReader() {
        return $this->_reader;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\G11n\Exception\InvalidCatalogException
     */
    final public function parseKey($key) {
        return $this->cache([__METHOD__, $key], function() use ($key) {
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

            return [$domain, $catalog, $key];
        });
    }

    /**
     * {@inheritdoc}
     */
    public function setReader(Reader $reader) {
        $this->_reader = $reader;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function translate($key, array $params = []) {
        return MessageFormatter::formatMessage(Locale::DEFAULT_LOCALE, $this->getMessage($key), $params);
    }

}
