<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Common\Base;
use Titon\Common\Config;
use Titon\Common\Traits\Cacheable;
use Titon\G11n\Bundle\LocaleBundle;
use Titon\G11n\Bundle\MessageBundle;
use Titon\Utility\Hash;

/**
 * The Locale class manages all aspects of a locale code, it's region specific rules
 * and even translated messages.
 *
 * @package Titon\G11n
 */
class Locale extends Base {
    use Cacheable;

    /**
     * Locale country code.
     *
     * @var string
     */
    protected $_code;

    /**
     * Locale resource bundle.
     *
     * @var \Titon\G11n\Bundle\LocaleBundle
     */
    protected $_localeBundle;

    /**
     * Message resource bundle.
     *
     * @var \Titon\G11n\Bundle\MessageBundle
     */
    protected $_messageBundle;

    /**
     * Parent locale.
     *
     * @var \Titon\G11n\Locale
     */
    protected $_parent;

    /**
     * Set code and config.
     *
     * @param string $code
     * @param array $config
     */
    public function __construct($code, array $config = []) {
        parent::__construct(['initialize' => false] + $config);

        $this->_code = $code;
        $this->_localeBundle = new LocaleBundle();
        $this->_messageBundle = new MessageBundle();

        // Add default resource paths
        if ($paths = Config::get('titon.path.resources')) {
            $this->addResourcePaths('core', $paths);
        }
    }

    /**
     * Add resource path lookups for locales and messages.
     *
     * @param string $domain
     * @param string $path
     * @return $this
     */
    public function addResourcePath($domain, $path) {
        $code = $this->getCode();

        $this->getLocaleBundle()->addPath($domain, sprintf('%s/locales/%s', $path, $code));

        $this->getMessageBundle()->addPaths($domain, [
            sprintf('%s/messages/%s', $path, $code),
            sprintf('%s/messages/%s/LC_MESSAGES', $path, $code) // gettext
        ]);

        return $this;
    }

    /**
     * Add multiple resource path lookups.
     *
     * @param string $domain
     * @param array $paths
     * @return $this
     */
    public function addResourcePaths($domain, array $paths) {
        foreach ($paths as $path) {
            $this->addResourcePath($domain, $path);
        }

        return $this;
    }

    /**
     * Instantiate the locale and message bundles using the resource paths.
     *
     * @uses Locale
     * @uses Titon\Common\Config
     *
     * @return $this
     */
    public function initialize() {
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
    public function getCode() {
        return $this->_code;
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
    public function getLocaleBundle() {
        return $this->_localeBundle;
    }

    /**
     * Return the message bundle.
     *
     * @return \Titon\G11n\Bundle\MessageBundle
     */
    public function getMessageBundle() {
        return $this->_messageBundle;
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
