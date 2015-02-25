<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\Bag\AbstractBag;

class ArgumentBag extends AbstractBag<string, InputDefinition> {

    public function get(string $key, ?InputDefinition $default = null): ?InputDefinition {
        $arg = null;
        if (is_null($arg = parent::get($key))) {
            foreach ($this as $val) {
                if ($key === $val->getAlias()) {
                    return $val;
                }
            }
        }

        return $arg;
    }
}