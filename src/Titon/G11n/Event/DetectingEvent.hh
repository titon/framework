<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Event;

use Titon\Event\Event;
use Titon\G11n\Translator;

/**
 * The globalization event that occurs before the locale is detected.
 *
 * @package Titon\G11n\Event
 */
class DetectingEvent extends Event {

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
     */
    public function __construct(Translator $translator) {
        $this->translator = $translator;

        parent::__construct('g11n.detecting');
    }

    /**
     * Return the translator.
     *
     * @return \Titon\G11n\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

}
