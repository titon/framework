<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Event;

use Titon\Event\Event;
use Titon\Intl\Locale;
use Titon\Intl\Translator;

/**
 * The globalization event that occurs after a locale is detected but before its applied.
 *
 * @package Titon\Intl\Event
 */
class DetectedEvent extends Event {

    /**
     * The detected locale.
     *
     * @var \Titon\Intl\Locale
     */
    protected Locale $locale;

    /**
     * The translator instance.
     *
     * @var \Titon\Intl\Translator
     */
    protected Translator $translator;

    /**
     * Store the event settings.
     *
     * @param \Titon\Intl\Translator $translator
     * @param \Titon\Intl\Locale $locale
     */
    public function __construct(Translator $translator, Locale $locale) {
        $this->translator = $translator;
        $this->locale = $locale;

        parent::__construct('intl.detected');
    }

    /**
     * Return the locale.
     *
     * @return \Titon\Intl\Locale
     */
    public function getLocale(): Locale {
        return $this->locale;
    }

    /**
     * Return the translator.
     *
     * @return \Titon\Intl\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

}
