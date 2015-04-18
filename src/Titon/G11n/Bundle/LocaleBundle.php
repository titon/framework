<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Bundle;

use Titon\Io\Bundle\AbstractBundle;
use Titon\Io\Reader\PhpReader;

/**
 * The LocaleBundle manages the loading of locale resources which contain locale specific configuration,
 * validation rules (phone numbers, zip codes, etc), inflection rules (plurals, singulars, irregulars, etc)
 * and formatting rules (dates, times, etc).
 *
 * @package Titon\G11n\Bundle
 */
class LocaleBundle extends AbstractBundle {

    /**
     * Add the PhpReader for locale bundle reading.
     *
     * @uses Titon\Io\Reader\PhpReader
     */
    public function initialize() {
        $this->addReader(new PhpReader());
    }

}
