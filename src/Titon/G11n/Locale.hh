<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Common\Bag;
use Titon\G11n\Bag\FormatBag;
use Titon\G11n\Bag\InflectionBag;
use Titon\G11n\Bag\MetaBag;
use Titon\G11n\Bag\ValidationBag;
use Titon\Io\Bundle\ResourceBundle;
use Titon\Io\Reader\HackReader;
use \Locale as SystemLocale;

/**
 * The `Locale` class manages all aspects of a locale code, like its region specific rules and translation messages.
 *
 * @package Titon\G11n
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
     * Resource bundle.
     *
     * @var \Titon\Io\Bundle\ResourceBundle
     */
    protected ResourceBundle $bundle;

    /**
     * Locale code.
     *
     * @var string
     */
    protected string $code;

    /**
     * Format patterns bag.
     *
     * @var \Titon\G11n\Bag\FormatBag
     */
    protected ?FormatBag $formatBag;

    /**
     * Inflection rules bag.
     *
     * @var \Titon\G11n\Bag\InflectionBag
     */
    protected ?InflectionBag $inflectionBag;

    /**
     * Metadata bag.
     *
     * @var \Titon\G11n\Bag\MetaBag
     */
    protected ?MetaBag $metaBag;

    /**
     * Parent locale.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $parent;

    /**
     * Validation rules bag.
     *
     * @var \Titon\G11n\Bag\ValidationBag
     */
    protected ?ValidationBag $validationBag;

    /**
     * Set locale code and optional bundle.
     *
     * @param string $code
     * @param \Titon\Io\Bundle\ResourceBundle $bundle
     */
    public function __construct(string $code, ?ResourceBundle $bundle = null) {
        $this->code = $code = self::canonicalize($code);
        $this->bundle = $bundle ?: new ResourceBundle();

        // Add a `HackReader` as the built-in resource files are Hack
        $bundle->addReader(new HackReader());

        // We can infer the parent from the locale code
        if (($pos = strpos($code, '_')) !== false) {
            $this->parent = LocaleRegistry::factory(substr($code, 0, $pos));
        }
    }

    /**
     * Add a resource lookup path for locales and messages.
     *
     * @param string $path
     * @return $this
     */
    public function addResourcePath(string $path): this {
        $path = rtrim($path, '/');
        $code = $this->getCode();

        $this->getResourceBundle()
            ->addPath('locales', sprintf('%s/locales/%s', $path, $code))
            ->addPath('messages', sprintf('%s/messages/%s', $path, $code));

        return $this;
    }

    /**
     * Add multiple resource lookup paths.
     *
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addResourcePaths(PathList $paths): this {
        foreach ($paths as $path) {
            $this->addResourcePath($path);
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
     * @param \Titon\G11n\LocaleTagMap $tags
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
     * @return \Titon\G11n\LocaleTagMap
     */
    public static function decompose($locale): LocaleTagMap {
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
     * @return \Titon\G11n\Bag\FormatBag
     */
    public function getFormatPatterns(): FormatBag {
        if ($this->formatBag) {
            return $this->formatBag;
        }

        if ($parent = $this->getParentLocale()) {
            $bag = clone $parent->getFormatPatterns();
        } else {
            $bag = new FormatBag();
        }

        $bag->add($this->getResourceBundle()->loadResource('locales', 'formats'));

        return $this->formatBag = $bag;
    }

    /**
     * Return the inflection rules from the locale bundle.
     *
     * @return \Titon\G11n\Bag\InflectionBag
     */
    public function getInflectionRules(): InflectionBag {
        if ($this->inflectionBag) {
            return $this->inflectionBag;
        }

        if ($parent = $this->getParentLocale()) {
            $bag = clone $parent->getInflectionRules();
        } else {
            $bag = new InflectionBag();
        }

        $bag->add($this->getResourceBundle()->loadResource('locales', 'inflections'));

        return $this->inflectionBag = $bag;
    }

    /**
     * Return the metadata from the locale bundle.
     *
     * @return \Titon\G11n\Bag\MetaBag
     */
    public function getMetadata(): MetaBag {
        if ($this->metaBag) {
            return $this->metaBag;
        }

        if ($parent = $this->getParentLocale()) {
            $bag = clone $parent->getMetadata();
        } else {
            $bag = new MetaBag();
        }

        $bag->add($this->getResourceBundle()->loadResource('locales', 'locale'));

        return $this->metaBag = $bag;
    }

    /**
     * Return the validation rules from the locale bundle.
     *
     * @return \Titon\G11n\Bag\ValidationBag
     */
    public function getValidationRules(): ValidationBag {
        if ($this->validationBag) {
            return $this->validationBag;
        }

        if ($parent = $this->getParentLocale()) {
            $bag = clone $parent->getValidationRules();
        } else {
            $bag = new ValidationBag();
        }

        $bag->add($this->getResourceBundle()->loadResource('locales', 'validations'));

        return $this->validationBag = $bag;
    }

    /**
     * Return the parent locale if it exists.
     *
     * @return \Titon\G11n\Locale
     */
    public function getParentLocale(): ?Locale {
        return $this->parent;
    }

    /**
     * Return the resource bundle.
     *
     * @return \Titon\Io\Bundle\ResourceBundle
     */
    public function getResourceBundle(): ResourceBundle {
        return $this->bundle;
    }

}
