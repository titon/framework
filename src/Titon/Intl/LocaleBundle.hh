<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Io\Bundle\ResourceBundle;
use Titon\Io\Reader\HackReader;

/**
 * A resource bundle for loading locale metadata.
 *
 * @package Titon\Intl
 */
class LocaleBundle extends ResourceBundle {

    /**
     * Add a `HackReader` as the built-in locale files are Hack.
     */
    public function __construct() {
        $this->addReader(new HackReader());
    }

}
