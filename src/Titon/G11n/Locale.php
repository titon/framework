<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\G11n\Bundle\LocaleBundle;
use Titon\G11n\Bundle\MessageBundle;
use Titon\Utility\Config;

/**
 * The Locale class manages all aspects of a locale code, like its region specific rules
 * and its translated messages.
 *
 * @package Titon\G11n
 */
class Locale {

    /**
     * Possible formats for locale keys.
     */
    const int FORMAT_1 = 1; // en-us (urls)
    const int FORMAT_2 = 2; // en-US
    const int FORMAT_3 = 3; // en_US (preferred)
    const int FORMAT_4 = 4; // enUS

    /**
     * Locale country code.
     *
     * @var string
     */
    protected string $code;

    /**
     * Locale resource bundle.
     *
     * @var \Titon\G11n\Bundle\LocaleBundle
     */
    protected LocaleBundle $localeBundle;

    /**
     * Message resource bundle.
     *
     * @var \Titon\G11n\Bundle\MessageBundle
     */
    protected MessageBundle $messageBundle;

    /**
     * Parent locale.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $parent;

    /**
     * Set locale code.
     *
     * @param string $code
     */
    public function __construct(string $code) {
        $this->code = $code;
        $this->localeBundle = new LocaleBundle();
        $this->messageBundle = new MessageBundle();

        // Add default resource paths
        if ($paths = Config::get('titon.paths.locales')) {
            $this->addResourcePaths('core', $paths);
        }
    }

    /**
     * Add a resource lookup path for locales and messages.
     *
     * @param string $domain
     * @param string $path
     * @return $this
     */
    public function addResourcePath(string $domain, string $path): this {
        $path = rtrim($path, '/');
        $code = $this->getCode();

        $this->getLocaleBundle()->addPath($domain, sprintf('%s/locales/%s', $path, $code));

        $this->getMessageBundle()->addPath($domain, sprintf('%s/messages/%s', $path, $code));

        return $this;
    }

    /**
     * Add multiple resource lookup paths.
     *
     * @param string $domain
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addResourcePaths(string $domain, PathList $paths): this {
        foreach ($paths as $path) {
            $this->addResourcePath($domain, $path);
        }

        return $this;
    }

    /**
     * Convert a locale key to 3 possible formats.
     *
     * @param string $key
     * @param int $format
     * @return string
     */
    public static function canonicalize(string $key, int $format = self::FORMAT_1): string {
        $parts = explode('-', str_replace('_', '-', strtolower($key)));
        $return = $parts[0];

        if (array_key_exists(1, $parts)) {
            switch ($format) {
                case self::FORMAT_1:
                    $return .= '-' . $parts[1];
                break;
                case self::FORMAT_2:
                    $return .= '-' . strtoupper($parts[1]);
                break;
                case self::FORMAT_3:
                    $return .= '_' . strtoupper($parts[1]);
                break;
                case self::FORMAT_4:
                    $return .= strtoupper($parts[1]);
                break;
            }
        }

        return $return;
    }

    /**
     * Takes an array of key-values and returns a correctly ordered and delimited locale ID.
     *
     * @uses Locale
     *
     * @param array $tags
     * @return string
     */
    public static function compose(array $tags) {
        return \Locale::composeLocale($tags);
    }

    /**
     * Parses a locale string and returns an array of key-value locale tags.
     *
     * @uses Locale
     *
     * @param string $locale
     * @return string
     */
    public static function decompose($locale) {
        return \Locale::parseLocale($locale);
    }

    /**
     * Instantiate the locale and message bundles using the resource paths.
     *
     * @uses Locale
     * @uses Titon\Utility\Config
     *
     * @return $this
     */
    public function initialize(): this {
        if ($data = $this->getLocaleBundle()->loadResource(null, 'locale')) {
            $data = \Locale::parseLocale($data['code']) + $data;

            $config = $this->allConfig();
            unset($config['code'], $config['initialize']);

            $this->addConfig($config + $data);
        }

        // Force parent config to merge
        $this->getParentLocale();

        return $this;
    }

    /**
     * Return the locale code.
     *
     * @return string
     */
    public function getCode(): string {
        return $this->code;
    }

    /**
     * Return the format patterns from the locale bundle.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return string|array
     */
    public function getFormatPatterns($key = null) {
        return Hash::get($this->_loadResource('formats'), $key);
    }

    /**
     * Return the inflection rules from the locale bundle.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return string|array
     */
    public function getInflectionRules($key = null) {
        return Hash::get($this->_loadResource('inflections'), $key);
    }

    /**
     * Return the validation rules from the locale bundle.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return string|array
     */
    public function getValidationRules($key = null) {
        return Hash::get($this->_loadResource('validations'), $key);
    }

    /**
     * Return the parent locale if it exists.
     *
     * @uses Titon\G11n\Locale
     *
     * @return \Titon\G11n\Locale
     */
    public function getParentLocale() {
        if ($this->_parent) {
            return $this->_parent;
        }

        if (!$this->hasConfig('parent')) {
            return null;
        }

        $parent = new Locale($this->getConfig('parent'));
        $parent->initialize();

        // Merge parent config
        $this->addConfig($this->allConfig() + $parent->allConfig());

        $this->_parent = $parent;

        return $parent;
    }

    /**
     * Return the locale bundle.
     *
     * @return \Titon\G11n\Bundle\LocaleBundle
     */
    public function getLocaleBundle(): LocaleBundle {
        return $this->localeBundle;
    }

    /**
     * Return the message bundle.
     *
     * @return \Titon\G11n\Bundle\MessageBundle
     */
    public function getMessageBundle(): MessageBundle {
        return $this->messageBundle;
    }

    /**
     * Load a resource from the locale bundle and merge with the parent if possible.
     *
     * @param string $resource
     * @return array
     */
    protected function _loadResource($resource) {
        return $this->cache([__METHOD__, $resource], function() use ($resource) {
            $data = $this->getLocaleBundle()->loadResource(null, $resource);

            if ($parent = $this->getParentLocale()) {
                $data = array_merge(
                    $parent->getLocaleBundle()->loadResource(null, $resource),
                    $data
                );
            }

            return $data;
        });
    }

}
