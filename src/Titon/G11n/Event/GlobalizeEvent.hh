<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Event;

use Titon\Event\Event;
use Titon\G11n\Locale;
use Titon\G11n\Translator;

/**
 * The globalization event that occurs before a systems locale is set.
 *
 * @package Titon\G11n\Event
 */
class GlobalizeEvent extends Event {

    /**
     * The new locale.
     *
     * @var \Titon\G11n\Locale
     */
    protected Locale $locale;

    /**
     * The translator instance.
     *
     * @var \Titon\G11n\Translator
     */
    protected Translator $translator;

    /**
     * Store the event settings.
     *
     * @param \Titon\G11n\Translator $translator
     * @param \Titon\G11n\Locale $locale
     */
    public function __construct(Translator $translator, Locale $locale) {
        $this->translator = $translator;
        $this->locale = $locale;

        parent::__construct('g11n.globalize');
    }

    /**
     * Return the locale.
     *
     * @return \Titon\G11n\Locale
     */
    public function getLocale(): Locale {
        return $this->locale;
    }

    /**
     * Return the translator.
     *
     * @return \Titon\G11n\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

    /**
     * Set a new locale.
     *
     * @param \Titon\G11n\Locale
     * @return $this
     */
    public function setLocale(Locale $locale): this {
        $this->locale = $locale;

        return $this;
    }

}
