<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Locator;

use Titon\Utility\Config;
use Titon\Utility\Col;
use Titon\View\Locator;

/**
 * An extension of the base `TemplateLocator` that provides locale aware lookups.
 *
 * @package Titon\View\Locator
 */
class LocaleTemplateLocator extends TemplateLocator {

    /**
     * List of locales to use during template locating.
     *
     * @var \Titon\View\LocaleList
     */
    protected LocaleList $locales = Vector {};

    /**
     * Store the lookup paths, locales, and template extension.
     * If locales are defined in the config, also store them.
     *
     * @param mixed $paths
     * @param mixed $locales
     * @param string $ext
     */
    public function __construct(mixed $paths, mixed $locales, string $ext = 'tpl') {
        parent::__construct($paths, $ext);

        if ($locales) {
            $this->addLocales(Col::toVector($locales));
        }

        if ($locales = Config::get('titon.locale.cascade')) {
            $this->addLocales(Col::toVector($locales));
        }
    }

    /**
     * Add a locale.
     *
     * @param string $locale
     * @return $this
     */
    public function addLocale(string $locale): this {
        if (!in_array($locale, $this->locales)) {
            $this->locales[] = $locale;
        }

        return $this;
    }

    /**
     * Add multiple locales.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function addLocales(LocaleList $locales): this {
        foreach ($locales as $locale) {
            $this->addLocale($locale);
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function buildTemplateLookup(string $template): PathList {
        $paths = Vector {};
        $ext = $this->getExtension();

        if ($locales = $this->getLocales()) {
            foreach ($locales as $locale) {
                $paths[] = $template . '.' . $locale . '.' . $ext;
           }
        }

        $paths[] = $template . '.' . $ext;

        return $paths;
    }

    /**
     * Return all locales.
     *
     * @return \Titon\View\LocaleList
     */
    public function getLocales(): LocaleList {
        return $this->locales;
    }

    /**
     * Set a list of locales and overwrite any previously defined locales.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function setLocales(LocaleList $locales): this {
        $this->locales->clear();

        $this->addLocales($locales);

        return $this;
    }

}
