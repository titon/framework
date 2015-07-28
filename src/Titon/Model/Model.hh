<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Model;

/**
 * This is a temp file and only exists to suppress type checker errors.
 *
 * @package Titon\Model
 */
class Model {

    protected Map<string, mixed> $attributes = Map {};

    public function __construct(Map<string, mixed> $attributes = Map {}) {
        $this->attributes = $attributes;
    }

    public function getAttributes(): Map<string, mixed> {
        return $this->attributes;
    }

}
