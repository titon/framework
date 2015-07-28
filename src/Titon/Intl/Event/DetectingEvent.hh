<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Event;

use Titon\Event\Event;
use Titon\Intl\Translator;

/**
 * The globalization event that occurs before the locale is detected.
 *
 * @package Titon\Intl\Event
 */
class DetectingEvent extends Event {

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
     */
    public function __construct(Translator $translator) {
        $this->translator = $translator;

        parent::__construct('intl.detecting');
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
