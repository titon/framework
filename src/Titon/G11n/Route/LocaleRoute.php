<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Route;

use Titon\G11n\G11n;
use Titon\Route\Route;

/**
 * Applies locale aware routes through the G11n package.
 *
 * @package Titon\Route
 */
class LocaleRoute extends Route {

    /**
     * Prepend the locale to the front of the route before compilation.
     *
     * @return string
     */
    public function compile() {
        if ($this->isCompiled()) {
            return $this->_compiled;
        }

        $g11n = G11n::registry();

        if ($g11n->isEnabled()) {
            if (mb_substr($this->getPath(), 0, 9) !== '/<locale>') {
                $this->prepend('/<locale>');
            }

            $this->setConfig('patterns.locale', static::LOCALE);
            $this->setConfig('locale', G11n::canonicalize($g11n->getFallback()->getCode()));
        }

        return parent::compile();
    }

}
