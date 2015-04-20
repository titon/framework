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
use Titon\G11n\Bundle\LocaleBundle;
use Titon\G11n\Bundle\MessageBundle;
use Titon\Io\ResourceMap;
use Titon\Utility\Col;
use Titon\Utility\Config;
use \Locale as SystemLocale;

/**
 * The `Locale` class manages all aspects of a locale code, like its region specific rules and translation messages.
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
     * Set locale code.
     *
     * @param string $code
     */
    public function __construct(string $code) {
        $this->code = $code;
        $this->localeBundle = new LocaleBundle();
        $this->messageBundle = new MessageBundle();

        // Add default resource paths
        if ($paths = Config::get('titon.paths.resources')) {
            $this->addResourcePaths(Col::toVector($paths));
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

        $this->getLocaleBundle()
            ->addPath('core', sprintf('%s/locales/%s', $path, $code));

        $this->getMessageBundle()
            ->addPath('core', sprintf('%s/messages/%s', $path, $code));

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
    public static function canonicalize(string $code, int $format = self::FORMAT_1): string {
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

        $bag = $this->loadBag(new FormatBag(), 'formats', __FUNCTION__);

        invariant($bag instanceof FormatBag, 'Must be a FormatBag.');

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

        $bag = $this->loadBag(new InflectionBag(), 'inflections', __FUNCTION__);

        invariant($bag instanceof InflectionBag, 'Must be an InflectionBag.');

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

        // Do not merge the bag with the parent in this call.
        // Instead do it in `getParentLocale()`.
        return $this->metaBag = new MetaBag($this->getLocaleBundle()->loadResource('core', 'locale'));
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

        $bag = $this->loadBag(new ValidationBag(), 'validations', __FUNCTION__);

        invariant($bag instanceof ValidationBag, 'Must be a ValidationBag.');

        return $this->validationBag = $bag;
    }

    /**
     * Return the parent locale if it exists.
     *
     * @uses Titon\G11n\Locale
     *
     * @return \Titon\G11n\Locale
     */
    public function getParentLocale(): ?Locale {
        if ($this->parent) {
            return $this->parent;
        }

        $parentCode = $this->getMetadata()->getParentCode();

        if (!$parentCode) {
            return null;
        }

        $parent = new Locale($parentCode);

        // Merge metadata from parent
        $this->mergeBags($parent->getMetadata(), $this->getMetadata());

        return $this->parent = $parent;
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
     * Load the contents of a resource file into the bag and inherit the same resource from the parent.
     *
     * @param \Titon\Common\Bag $bag
     * @param string $resource
     * @param string $method
     * @return \Titon\Common\Bag
     */
    protected function loadBag(Bag $bag, string $resource, string $method): Bag {
        $bag->add($this->getLocaleBundle()->loadResource('core', $resource));

        if ($parent = $this->getParentLocale()) {
            $this->mergeBags(inst_meth($parent, $method), $bag);
        }

        return $bag;
    }

    /**
     * Inherit fields from the parent bag that do not exist in the child bag.
     *
     * @param \Titon\Common\Bag $parentBag
     * @param \Titon\Common\Bag $childBag
     * @return \Titon\Common\Bag
     */
    protected function mergeBags(Bag $parentBag, Bag $childBag): Bag {
        foreach ($parentBag as $key => $value) {
            if (!$childBag->has($key)) {
                $childBag->set($key, $value);
            }
        }

        return $childBag;
    }

}
