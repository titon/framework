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

class LocaleTemplateLocator extends TemplateLocator {

    /**
     * List of locales to use during template locating.
     *
     * @var \Titon\View\LocaleList
     */
    protected LocaleList $locales = Vector {};

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
     * Add a locale lookup.
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
     * Add multiple locale lookups.
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
     * Return all locales.
     *
     * @return \Titon\View\LocaleList
     */
    public function getLocales(): LocaleList {
        return $this->locales;
    }

    /**
     * Set a list of locales and overwrite any previously defined paths.
     *
     * @param \Titon\View\LocaleList $locales
     * @return $this
     */
    public function setLocales(LocaleList $locales): this {
        $this->locales = $locales;

        return $this;
    }

}
