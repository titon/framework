<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Common\Bag;
use Titon\Intl\Bag\FormatBag;
use Titon\Intl\Bag\InflectionBag;
use Titon\Intl\Bag\MetadataBag;
use Titon\Intl\Bag\ValidationBag;
use Titon\Io\PathList;
use Titon\Utility\Col;
use \Locale as SystemLocale;

/**
 * The `Locale` class manages all aspects of a locale code, like its region specific rules and translation messages.
 *
 * @package Titon\Intl
 */
class Locale {

    /**
     * Possible formats for locale codes.
     */
    const int FORMAT_1 = 1; // en-us (urls)
    const int FORMAT_2 = 2; // en-US
    const int FORMAT_3 = 3; // en_US (preferred)
    const int FORMAT_4 = 4; // enUS

    /**
     * Locale code.
     *
     * @var string
     */
    protected string $code;

    /**
     * Format patterns bag.
     *
     * @var \Titon\Intl\Bag\FormatBag
     */
    protected ?FormatBag $formatBag;

    /**
     * Inflection rules bag.
     *
     * @var \Titon\Intl\Bag\InflectionBag
     */
    protected ?InflectionBag $inflectionBag;

    /**
     * Locale bundle.
     *
     * @var \Titon\Intl\LocaleBundle
     */
    protected LocaleBundle $localeBundle;

    /**
     * Message bundle.
     *
     * @var \Titon\Intl\MessageBundle
     */
    protected MessageBundle $messageBundle;

    /**
     * Metadata bag.
     *
     * @var \Titon\Intl\Bag\MetadataBag
     */
    protected ?MetadataBag $metaBag;

    /**
     * Parent locale.
     *
     * @var \Titon\Intl\Locale
     */
    protected ?Locale $parent;

    /**
     * Validation patterns bag.
     *
     * @var \Titon\Intl\Bag\ValidationBag
     */
    protected ?ValidationBag $validationBag;

    /**
     * Set locale code and optional bundle.
     *
     * @param string $code
     * @param \Titon\Intl\LocaleBundle $localeBundle
     * @param \Titon\Intl\MessageBundle $messageBundle
     */
    public function __construct(string $code, ?LocaleBundle $localeBundle = null, ?MessageBundle $messageBundle = null) {
        $this->code = $code = self::canonicalize($code);
        $this->localeBundle = $localeBundle ?: new LocaleBundle();
        $this->messageBundle = $messageBundle ?: new MessageBundle();

        // We can infer the parent from the locale code
        if (($pos = strpos($code, '_')) !== false) {
            $this->parent = LocaleRegistry::factory(substr($code, 0, $pos));
        }
    }

    /**
     * Add a locale lookup path.
     *
     * @param string $path
     * @return $this
     */
    public function addLocalePath(string $path): this {
        $this->getLocaleBundle()->addPath(MessageLoader::DEFAULT_DOMAIN, $path . '/' . $this->getCode());

        // Pass it to the parent also
        $this->getParentLocale()?->addLocalePath($path);

        return $this;
    }

    /**
     * Add multiple locale lookup paths.
     *
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addLocalePaths(PathList $paths): this {
        foreach ($paths as $path) {
            $this->addLocalePath($path);
        }

        return $this;
    }

    /**
     * Add a message lookup path.
     *
     * @param string $domain
     * @param string $path
     * @return $this
     */
    public function addMessagePath(string $domain, string $path): this {
        $this->getMessageBundle()->addPath($domain, $path . '/' . $this->getCode());

        // Pass it to the parent also
        $this->getParentLocale()?->addMessagePath($domain, $path);

        return $this;
    }

    /**
     * Add multiple message lookup paths.
     *
     * @param string $domain
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addMessagePaths(string $domain, PathList $paths): this {
        foreach ($paths as $path) {
            $this->addMessagePath($domain, $path);
        }

        return $this;
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

        $this->addLocalePath(sprintf('%s/locales', $path));
        $this->addMessagePath($domain, sprintf('%s/messages', $path));

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
     * Convert a locale code to a specific format.
     *
     * @param string $code
     * @param int $format
     * @return string
     */
    public static function canonicalize(string $code, int $format = self::FORMAT_3): string {
        $parts = explode('-', str_replace('_', '-', strtolower($code)));
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
     * Takes a map of key-values and returns a correctly ordered and delimited locale ID.
     *
     * @uses \Locale
     *
     * @param \Titon\Intl\LocaleTagMap $tags
     * @return string
     */
    public static function compose(LocaleTagMap $tags): string {
        return (string) SystemLocale::composeLocale($tags);
    }

    /**
     * Parses a locale string and returns a map of key-value locale tags.
     *
     * @uses \Locale
     *
     * @param string $locale
     * @return \Titon\Intl\LocaleTagMap
     */
    public static function decompose(string $locale): LocaleTagMap {
        return new Map(SystemLocale::parseLocale($locale));
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
     * @return \Titon\Intl\Bag\FormatBag
     */
    public function getFormatPatterns(): FormatBag {
        if ($this->formatBag) {
            return $this->formatBag;
        }

        $bag = $this->getLocaleBundle()->loadResource(MessageLoader::DEFAULT_DOMAIN, 'formats');

        if ($parent = $this->getParentLocale()) {
            $bag = Col::merge($parent->getFormatPatterns()->all(), $bag);
        }

        return $this->formatBag = new FormatBag($bag);
    }

    /**
     * Return the inflection rules from the locale bundle.
     *
     * @return \Titon\Intl\Bag\InflectionBag
     */
    public function getInflectionRules(): InflectionBag {
        if ($this->inflectionBag) {
            return $this->inflectionBag;
        }

        $bag = $this->getLocaleBundle()->loadResource(MessageLoader::DEFAULT_DOMAIN, 'inflections');

        if ($parent = $this->getParentLocale()) {
            $bag = Col::merge($parent->getInflectionRules()->all(), $bag);
        }

        return $this->inflectionBag = new InflectionBag($bag);
    }

    /**
     * Return the locale bundle.
     *
     * @return \Titon\Intl\LocaleBundle
     */
    public function getLocaleBundle(): LocaleBundle {
        return $this->localeBundle;
    }

    /**
     * Return the message bundle.
     *
     * @return \Titon\Intl\MessageBundle
     */
    public function getMessageBundle(): MessageBundle {
        return $this->messageBundle;
    }

    /**
     * Return the metadata from the locale bundle.
     *
     * @return \Titon\Intl\Bag\MetadataBag
     */
    public function getMetadata(): MetadataBag {
        if ($this->metaBag) {
            return $this->metaBag;
        }

        $bag = $this->getLocaleBundle()->loadResource(MessageLoader::DEFAULT_DOMAIN, 'locale');

        if ($parent = $this->getParentLocale()) {
            $bag = Col::merge($parent->getMetadata()->all(), $bag);
        }

        return $this->metaBag = new MetadataBag($bag);
    }

    /**
     * Return the parent locale if it exists.
     *
     * @return \Titon\Intl\Locale
     */
    public function getParentLocale(): ?Locale {
        return $this->parent;
    }

    /**
     * Return the validation patterns from the locale bundle.
     *
     * @return \Titon\Intl\Bag\ValidationBag
     */
    public function getValidationPatterns(): ValidationBag {
        if ($this->validationBag) {
            return $this->validationBag;
        }

        $bag = $this->getLocaleBundle()->loadResource(MessageLoader::DEFAULT_DOMAIN, 'validations');

        if ($parent = $this->getParentLocale()) {
            $bag = Col::merge($parent->getValidationPatterns()->all(), $bag);
        }

        return $this->validationBag = new ValidationBag($bag);
    }

}
